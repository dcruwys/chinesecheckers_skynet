#include <vector>
#include "MCTS.h"
#include <algorithm>
#include <random>
#include <iterator>
#include <iostream>

struct MCNode
{
   int children = 0;
   Move myMove = {0,0};
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


///////////////////
//End struct stuff
///////////////////

std::vector<MCNode> tree;
ChineseCheckersState state;

MCTS::MCTS(ChineseCheckersState &s)
{
   //Deep Copy of state
   state(s);
   //Initialize Tree
   tree.clear();
   std::vector<Move> myMoves;
   state.getMoves(myMoves);
   
   MCNode rootNode(myMoves.size(), moves.at(0), 0, 0.0, 0);
   tree.push_back(rootNode);


}
MCTS::~MCTS(){};

Move getBestMove(){};

double selectLeaf(uint32_t node)
{
   if(!state.GameOver())
	  return selectLeaf(SelectBestChild(node));
   return tree.at(node).value;
}

uint32_t SelectBestChild(uint32_t node)
{
   std::vector<Move> moves;
   state.getMoves(moves);

   for(int i = 0; i < moves.size(); ++i){
	  //totalSamples++;
	  tree.push_back(0, moves.at(i), node+i, 1.0, node);
   }
   std::sort(tree.at(node), tree.end, [](const MCNode &a, const MCNode &b){
		 return GetUCBVal(a, node) > GetUCBVal(b, node)
   }
   Expand(node+1);
   return node+1;
}

double getUCBVal(uint32_t node)
{
 return tree.at(node).getUCB1Value;  
}

void Expand(uint32_t node)
{
   int player = state.getCurrentPlayer();
   state.applyMove(tree.at(node).myMove);
   
   std::vector<Move> movesList;
   if(player == state.getCurrentPlayer()){
	  std::cerr << "CurrentPlayer Check passed" << std::endl;
	  state.getMoves(movesList);

	  for(int i = node; i < m.size(); ++i){
		 auto value = DoPlayout(m, node); //what is the parent?
		 totalSamples++;
		 tree.pushBack(0, m, node+i, 1.0, node); 
	  }
   std::cerr << "Children added, new tree size = " << tree.size() << std::endl;
}



double DoPlayout(Move move, uint32_t parent)
{
   //Simulate
   double eval = 0.0;
   if(!state.gameOver()){
	 state.applyMove(move); 
     eval = randomPlayout(node);
	 state.undoMove(move);
   }
   //Backpropigation

  uint32_t currentNode = tree[parent];
  while(currentNode.location != 0)
  {
	 tree.at(parent).payOff += eval;
	 tree.at(parent) = tree.at(parent.parentIndex); 
  }
}




unsigned MCTS::getRand()
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> dis(0, 1);
   double rand = (double)dis(gen);
   //std::cerr << "rand = " << rand << std::endl;
   return rand;
}
int MCTS::random(Move aMove)
{
   //Call a state
   //
   //NOTE, WE NEED a new CCState, or else more work will be needed
   
   //ChineseCheckersState state(s);
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






