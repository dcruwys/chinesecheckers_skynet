#include <vector>
#include "MCTS.h"
#include <algorithm>
#include <random>
#include <iterator>
#include <iostream>


//totalSamples
int totalSamples = 0; 

//Our data type to be put in the tree
struct MCNode
{
   int children = 0;
   Move myMove = {0,0};
   uint32_t location = 0;
   double payOff = 0.0;
   double UCB1Value = 0.0;
   int samples = 0;
   uint32_t parentIndex = 0;
   //Constructor
   MCNode(){};
   MCNode(int children, Move m, uint32_t location, double payOff, uint32_t parentIndex)
   {
	  this->children = children;
	  this->myMove = m;
	  this->location = location;
	  this->payOff = payOff;
	  this->parentIndex = parentIndex;
	  //If root node samples = 0, else 1;
	  //parentIndex == 0 ? 0 : 1;
   } 
   void addPayoff(double pay){
	 payOff += pay;
   	 samples++;
   }
   double getAveragePayoff(){ return payOff / samples; }
   //Calculate payoff with UCB algorithm
   double getUCB1Value(){
	  //UCB1Value = (AvgPayoff of me) + k * sqrt(ln(TotalSamples) / # of me)
	  return ((payOff / samples) + 10 * sqrt((log(totalSamples) / samples)));
   } 
};

std::vector<MCNode> tree;

MCTS::MCTS(){}
MCTS::~MCTS(){}

void MCTS::InitializeTree(ChineseCheckersState &s)
{

   //Initialize stuff
   //Initializes the tree

   std::vector<Move> moves;
   ChineseCheckersState state(s);
   state.getMoves(moves);
   
   tree.clear();
   //Initialize the root of the tree to the first Node,
   //and num challenge
   MCNode rootNode(moves.size(), moves.at(0), 0, 0.0, 0);
   tree.push_back(rootNode); //Add to tree
   std::cerr << "Initialization Info: size of getMoves() = " << moves.size() << ", Root nodes move = "
	  << rootNode.myMove << ", TreeSize = " << tree.size() << std::endl; 
   //Choose next node
   //return SelectBestChild(tree.at(0).location);  
}
// the high-level function for computing the best move
Move MCTS::GetBestMove(ChineseCheckersState &state)
{
   //First initialize the tree
   InitializeTree(state);
   uint32_t bestChild = 0;
   //myLeaf = root node
   uint32_t myLeaf = SelectLeaf(tree.at(0).location, state);
   if(IsLeaf(myLeaf))
	  bestChild = SelectBestChild(myLeaf);  
   else
	  std::cerr << "Hmm...Not a leaf" << std::endl;
   //Expand(bestChild, ChineseCheckersState &state);
   MCNode best = tree[bestChild];
   return best.myMove;
}

/////////////////////
///END UCB1 Stuff////
////////////////////


// traverse down the tree (recursively),
// returning the value at the leaf of the sample
double MCTS::SelectLeaf(uint32_t node, ChineseCheckersState &state)
{
  /* std::vector<Move> tempList;
   int counter = node;
   for(Move m : moves)
   {
	  //Get the [payOff] integer returned from our UCB1 function
	  int payOff = random(m, state);
	  state.applyMove(move);
	  state.getMoves(tempList);
	  MCNode node2(tempList.size(), m, tree.size(), payOff, tree.get(node));
	  //reassign
	  tree[counter] = node2;
	  state.undoMove(m);
	  counter++;
   }*/
   //This part can definetly be optimized by storing stuff in memory
   std::vector<Move> movesList;
   //Probably horribly inefficient
   //std::vector<MCNode> tempTree;
   //tempTree.clear();
   //tempTree(&tree);

   state.applyMove(tree.at(node).myMove);
   state.getMoves(movesList);
   for(uint32_t i = 1; i < movesList.size(); ++i)
   {
	  MCNode aNode(0, movesList.at(i), node + i, random(movesList.at(i), state), node);
	  totalSamples++;
	  aNode.samples++;
	  tree.push_back(aNode);
   }
   //std::cerr << "tempTree created, size = " << tree.size() << std::endl;


   MCNode best = tree.at(node+1);
   std::cerr << "node = " << node << ", movesList size = " << movesList.size() << ", Size of tree = " << tree.size() << std::endl;
   if(tree.size() != 1){
	  for(auto i = node+1; i < (best.children + node); ++i){

		 std::cerr << "tree.at(" << i << ").getPayOff = " << tree.at(i).getUCB1Value() << ", treeSize = " << tree.size() << std::endl;
		 std::cerr << "CurrentUCBVal vs Best: " << tree.at(i).getUCB1Value() << " ?? " << best.getUCB1Value() << std::endl;
		 if(tree.at(i).getUCB1Value() > best.getUCB1Value()) {
		  best = tree.at(i);
		  std::cerr << "New Best Move! Move = " << best.myMove << ", UCB1Value = " << best.getUCB1Value() << std::endl;
		 }
	  }
   }
   std::cerr << "BestMove = " << best.myMove << std::endl;
   //Recursion
	  
	  //ChineseCheckersState probably needs to be a copy
   //Expand(best.location, state);

   if(best.children != 0){
	  SelectLeaf(best.location, state);
   }
   return best.getUCB1Value();
}
// Use the UCB rule to find the best child
// Seems that Select leaf kind of does this
uint32_t MCTS::SelectBestChild(uint32_t node)
{
   //start at parent
   MCNode bestChild = tree.at(node);
   for(int i = node; i < bestChild.children + node; ++i){
	  if(tree.at(i).getAveragePayoff() > bestChild.getAveragePayoff())
		 bestChild = tree.at(i);
   }
   return bestChild.location;
}
// is the designated node a leaf
bool MCTS::IsLeaf(uint32_t node)
{
   return tree.at(node).children == 0 ? 0 : 1;
}
// expand the designated node and add its children to the tree
void MCTS::Expand(uint32_t node, ChineseCheckersState &state)
{
  std::vector<Move> moves;
  state.getMoves(moves);
  std::vector<Move> temp;
  for(const auto i: moves){
    int a = random(i, state);
	std::cerr << "Random yields: " << a << std::endl;
    state.applyMove(i);
    state.getMoves(temp);
    MCNode newNode(temp.size(), i, moves.size(), a, node);
    tree.push_back(newNode); //add new node to tree.
    MCNode parent = tree[newNode.parentIndex];
    while(parent.location != 0){
      parent.payOff += a;
      parent = tree[parent.parentIndex];
    }
    state.undoMove(i);
  }
}
////////////////////////
/////Some UCB1 Code/////
//
unsigned MCTS::getRand()
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> dis(0, 1);
   double rand = (double)dis(gen);
   //std::cerr << "rand = " << rand << std::endl;
   return rand;
}
int MCTS::random(Move aMove, ChineseCheckersState &s)
{
   //Call a state
   //
   //NOTE, WE NEED a new CCState, or else more work will be needed
   
   ChineseCheckersState state(s);
   int cPlayer = state.getCurrentPlayer();
   state.applyMove(aMove);
   std::vector<Move> moves;
   state.getMoves(moves);
   state.
   //std::cerr << "Player after get move check: " << cPlayer << std::endl;
   //Increase our totalSamples, as another game is being sampled
   auto rand = getRand();
   
   //Increase totalSample size
   //totalSamples++;
//   std::cerr << "sample increased, now = " << totalSamples << std::endl;

 
  //Playout Policy 2
  //This policy return the results being evaluated by the eval function
   //Allow to play to x moves
   const int movesToPlay = 5;
   for (int i = 0; i < movesToPlay; ++i){
	  rand = getRand(); //Generate a new random number

	  //Check if the game is over, if so run eval function
	  if(state.gameOver()) return state.eval();

	  state.getMoves(moves); //We need to get the moves each time this time
	  if(rand > 0.15){
	  //Note, Again, this Might be wrong!!!
	  std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b){
		std::cerr << "lambda compare return" << a.score << " " << b.score << std::endl; 
		 return a.score < b.score;});
	  Move bestMove = moves.at(0);	 
	  std::cerr << "Best move from Lambda Policy 1 = " << bestMove << std::endl;
			//Apply Best Move
		 state.applyMove(bestMove);
			
	  } else {
		 //only happens 15% of the time
		 state.applyMove(moves.at((int)(getRand() * (moves.size()-1))));
	  }
   }
   //Return the eval function
   return state.eval(); 
}

