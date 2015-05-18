#include <vector>
#include "MCTS.h"
#include <algorithm>
#include <random>
#include <iterator>
#include <iostream>
//MCTS::MCTS(){}

//Are data type to be put in the tree
struct MCNode
{
   int children = 0;
   Move myMove = {0,0};
   uint32_t location = 0;
   double payOff = 0.0;
   int samples = 0;
   int totalSample = 0;
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
	  parentIndex == 0 ? 0 : 1; 
   } 
   void addPayoff(double pay){
	 payOff += pay;
   	 totalSample++;
   }
   double getAveragePayoff(){ return payOff / samples; }
   //Calculate payoff with UCB algorithm
   double getPayOff(){
	  return ((payOff / samples) + 10 * sqrt((log(totalSample)/samples)));
   } 
};

std::vector<MCNode> tree;

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
   //Choose next node
   //return SelectBestChild(tree.at(0).location);  
}
// the high-level function for computing the best move
Move MCTS::GetBestMove()
{
   //First initialize the tree
   InitizeTree(ChineseCheckersState state);
   //myLeaf = root node
   uint32_t int myLeaf = SelectLeaf(tree.at(0).location);
   if(IsLeaf(myLeaf))
	  uint32_t bestChild = SelectBestChild(myLeaf);  
   else
	  std::cerr << "Hmm...Not a leaf" << std::endl;
   //Expand(bestChild, ChineseCheckersState &state);
   return bestMove;
}

double MCTS::GetUCBVal(uint32_t node, uint32_t parent)
{
   //Not sure if we need this still   
}
/////////////////////
///END UCB1 Stuff////
////////////////////


// traverse down the tree (recursively),
// returning the value at the leaf of the sample
double MCTS::SelectLeaf(uint32_t node)
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

   MCNode best = tree.at(node);
   for(auto i = node; i < best.children + node; ++i)
	  if(tree.at(i).getPayOff > best.getPayOff) best = tree.at(i);
   //Recursion
	  
	  //ChineseCheckersState probably needs to be a copy
	  Expand(best.location, ChineseCheckersState &state);

   if(best.children != 0){
	  SelectLeaf(best.location);
   }
   return best.getPayOff;
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
    state.applyMove(i);
    state.getMoves(temp);
    MCNode newNode(temp.size(), i, moves.size(), a, node);
    tree.push_back(newNode); //add new node to tree.
    MCNode parent = tree[MCNode.parentIndex]
    while(parent != null){
      parent.payOff += a;
      parent = tree[parent.parentIndex];
    }
    state.undoMove(i);
  }
}
// play out the game, returning the evaluation at the end of the game
double MCTS::DoPlayout(uint32_t node)
{
}

////////////////////////
/////Some UCB1 Code/////
////////////////////////
unsigned getRand()
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> dis(0, 1);
   double rand = (double)dis(gen);
   std::cerr << "rand = " << rand << std::endl;
   return rand;
}
int MCTS::random(Move aMove, ChineseCheckersState &s)
{
   //Call a state
   //
   //NOTE, WE NEED a new CCState, or else more work will be needed
   
   ChineseCheckersState state(s);
   int cPlayer = state.getCurrentPlayer();
   //std::cerr << "State Cloned! Applying move, " << aMove << " checking current player in cloned: " << cPlayer << std::endl;
   //std::cerr << "CURRENT PLAYER CHECK original s: " << s.getCurrentPlayer() << std::endl;
   state.applyMove(aMove);
   //std::cerr << "Player after apply move check: " << cPlayer << std::endl;
   std::vector<Move> moves;
   state.getMoves(moves);
   //std::cerr << "Player after get move check: " << cPlayer << std::endl;
   //Increase our totalSamples, as another game is being sampled
   auto rand = getRand();
   
   //Increase totalSample size
   totalSample++;
   std::cerr << "sample increased, now = " << totalSample << std::endl;

 
  //Playout Policy 2
  //This policy return the results being evaluated by the eval function
    std::cerr << "Policy 2 being used " << std::endl;
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
		 std::cerr << "lambda compare return" << a.score < b.score << std::endl; 
		 return a.score < b.score;});
	  Move bestMove = moves.at(0);	 
	  std::cerr << "Best move from Lambda Policy 1 = " << bestMove << std::endl;
			//Apply Best Move
		 state.applyMove(bestMove);
			
	  } else {
		 //only happens 15% of the time
		 state.applyMove(moves.at((int)(getRand() * moves.size())));
	  }
   }
   //Return the eval function
   return state.eval(); 
}


