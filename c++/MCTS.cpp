#include <vector>
#include "MCTS.h"
#include <algorithm>
#include <random>
#include <iterator>
#include <iostream>
#include <chrono>
#include <thread>


int totalSamples = 0;

struct MCNode
{
   int children = 0;
   Move move = {0,0};
   uint32_t location = 0;
   double payOff = 1.0;
   double UCB1Value = 1.0;
   int samples = 1;
   uint32_t parentIndex = 0;
   //Constructor
   MCNode(){};
   MCNode(int children, Move m, uint32_t location, double payOff, uint32_t parentIndex)
   {
	  this->children = children;
	  this->move = m;
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


///////////////////
//End struct stuff
///////////////////

std::vector<MCNode> tree;
ChineseCheckersState state;
MCTS::MCTS(ChineseCheckersState &s)
{
   //Deep Copy of state
   state = s;
   //Initialize Tree
   tree.clear();
   std::vector<Move> moves;
   state.getMoves(moves);
   bool timeUp = false;
   MCNode rootNode(moves.size(), moves.at(0), 0, 0.0, 0);
   tree.push_back(rootNode);
   auto duration = std::chrono::milliseconds(1000); //10s
   auto t = std::thread([&timeUp, duration](){ std::this_thread::sleep_for(duration); timeUp = true; });
   SelectLeaf(rootNode.location, timeUp);

}
MCTS::~MCTS(){};

Move MCTS::GetBestMove(){
   std::cerr << "Tree at(0) = " << tree.at(0).move << ", Tree at(1) = " << tree.at(1).move << ", Tree at(2) = " << tree.at(2).move << std::endl; 
   return tree.at(0).move;
}

double MCTS::getRand()
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> dis(0, 1);
   double rand = (double)dis(gen);
   //std::cerr << "rand = " << rand << std::endl;
   return rand;
}
double MCTS::random(Move aMove)
{
   //Call a state
   //
   //NOTE, WE NEED a new CCState, or else more work will be needed
   
   //ChineseCheckersState state(s);
   int cPlayer = state.getCurrentPlayer();
   state.applyMove(aMove);
   std::vector<Move> moves;
   state.getMoves(moves);
 //  state.
   //std::cerr << "Player after get move check: " << cPlayer << std::endl;
   //Increase our totalSamples, as another game is being sampled
   double rand = getRand();
   double value = (double) state.eval();
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
		 return (double)( a.score < b.score);});
	  Move bestMove = moves.at(0);	 
	  std::cerr << "Best move from Lambda Policy 1 = " << bestMove << std::endl;
			//Apply Best Move
		 state.applyMove(bestMove);
     value = (double) state.eval();
     state.undoMove(bestMove);
			
	  } else {
		 //only happens 15% of the time
     Move randomMove = moves.at((int)(getRand() * (moves.size()-1)));
		 state.applyMove(randomMove);
     value = (double) state.eval();
     state.undoMove(randomMove);
	  }
   }
   //Return the eval function
   return (double) state.eval(); 
}
double MCTS::getUCBVal(uint32_t node)
{
 return tree.at(node).getUCB1Value();  
}


uint32_t MCTS::SelectBestChild(uint32_t node)
{
   std::vector<Move> moves;
   state.getMoves(moves);
   std::cerr << "Selecting best child starting from node: " << node << std::endl;
   for(int i = 0; i < moves.size(); ++i){
	  //totalSamples++;
	  MCNode n(0, moves.at(i), node+i, 1.0, node);
	  n.UCB1Value = n.getUCB1Value();
	  tree.push_back(n);
   }
   std::cerr << "Tree size is now = " << tree.size() << std::endl;
   std::sort(tree.begin(), tree.end(), [](const MCNode &a, const MCNode &b) -> bool {
		 return a.UCB1Value > b.UCB1Value;
   });
   std::cerr << "tree(node+1) move = " << tree.at(node+1).move << std::endl;
   Expand(node+1);
   return node+1;
}

double MCTS::SelectLeaf(uint32_t node, bool &timeUp)
{
  while(!timeUp){
   std::cerr << "selectLeaf nodes recursively" << std::endl;
   if(!state.gameOver())
	  return SelectLeaf(SelectBestChild(node), timeUp);
   std::cerr << "gameOver, leaf payoff is = " << tree.at(node).payOff << std::endl;
  }
   return tree.at(node).payOff;
}

void MCTS::Expand(uint32_t node)
{
   int player = state.getCurrentPlayer();
   state.applyMove(tree.at(node).move);
   
   std::vector<Move> movesList;
   if(player == state.getCurrentPlayer()){
	  std::cerr << "CurrentPlayer Check passed" << std::endl;
	  state.getMoves(movesList);

	  for(int i = node; i < movesList.size(); ++i){
		 auto value = DoPlayout(movesList.at(i), node); //what is the parent?
		 totalSamples++;
		 MCNode n(0, movesList.at(i), node+i, 1.0, node);
		 tree.push_back(n); 
	  }
   std::cerr << "Children added, new tree size = " << tree.size() << std::endl;
   }
}



double MCTS::DoPlayout(Move move, uint32_t parent)
{
   //Simulate
   double eval = 0.0;
   if(!state.gameOver()){
	 state.applyMove(move); 
     eval = random(move);
	 state.undoMove(move);
   }
   //Backpropigation

  MCNode currentNode = tree[parent];
  while(currentNode.location != 0)
  {
	 currentNode.payOff += eval;
	 parent = currentNode.parentIndex; 
  }
  return eval;
}










