#include <vector>

MCTS::MCTS(){}

//Are data type to be put in the tree
struct MCNode(int children, Move m, uint32_t location, double payOff, int samples)
{
   int myChildren = children;
   Move myMove = m;
   uint32_t myLocation = location;
   double myPayoff = payOff;
   int totalSamples = samples;
};

std::vector<Move> tree;
//Figure this bit out later
ChineseCheckersState &state;
std::vector<Move> moves;
state.getMoves(moves);
// the high-level function for computing the best move
Move MCTS::GetBestMove()
{

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
