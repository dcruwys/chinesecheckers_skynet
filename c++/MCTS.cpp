#include <vector>
#include "MCTS.h"

MCTS::MCTS(){}

//Are data type to be put in the tree
struct MCNode
{
   int myChildren = 0;
   Move myMove = {0,0};
   uint32_t location = 0;
   double myPayoff = 0.0;
   int totalSamples = 0;
   //Constructor
   MCNode(){};
   MCNode(int children, Move m, uint32_t location, double payOff, int samples)
   {
	  this->children = children;
	  this->m = m;
	  this->location = location;
	  this->payOff = payOff;
	  this->samples = samples;
   }
   int getChildren() return children;
   Move getMove() return m;
   uint32_t getLocation() return location;
   double getPayOff() return payOff;
   int getSamples() return samples;
};

//Initializes the tree
std::vector<MCNode> MCTS::InitializeTree(ChineseCheckersState &state)
{
   //Initialize stuff
   std::vector<Move> moves;
   state.getMoves(moves);
   std::vector<MCNode> tree;
   //Initialize the root of the tree to the first Node,
   //and num challenge 
   MCNode rootNode(moves.size(), moves.at(0), 0, 0.0, 0);
   tree.push_back(); //Add to tree

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
double MCTS::random(Move aMove, ChineseCheckersState &s){
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
  totalSample++;
  //std::cerr << "sample increased, now = " << totalSample << std::endl;

 
  //Playout Policy 2
  //This policy return the results being evaluated by the eval function
    std::cerr << "Policy 2 being used " << std::endl;
   //Allow to play to x moves
   const int movesToPlay = 75;
   for (int i = 0; i < movesToPlay; ++i){
	  //Check if the game is over, if so run eval function
	  if(state.gameOver()) return (double) state.eval();

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
		 state.applyMove(moves.at((double)(getRand() * moves.size())));
	  }
   }
   //Return the eval function
   return (double) state.eval();
 
}



// the high-level function for computing the best move
Move MCTS::GetBestMove()
{

   return bestMove;
}
// traverse down the tree (recursively),
// returning the value at the leaf of the sample
double MCTS::SelectLeaf(uint32_t node)
{
   return 0.0;
}
// Use the UCB rule to find the best child
uint32_t MCTS::SelectBestChild(uint32_t node)
{
   return 0;
}
// is the designated node a leaf
bool MCTS::IsLeaf(uint32_t nde)
{
   return 0;
}
// expand the designated node and add its children to the tree
void MCTS::Expand(uint32_t node)
{
}
// play out the game, returning the evaluation at the end of the game
double MCTS::DoPlayout(uint32_t node)
{
}
