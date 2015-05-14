#ifndef MCTS_H
#define MCTS_H

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <cmath>

//Are data type to be put in the tree
struct MCNode
{
   int myChildren = 0;
   Move myMove = {0,0};
   uint32_t location = 0;
   double payOff = 0.0;
   int samples = 0;
   uint32_t parentIndex = 0;
   //Constructor
   MCNode(){};
   MCNode(int children, Move m, uint32_t location, double payOff, uint32_t parentIndex)
   {
	  this->children = children;
	  this->m = m;
	  this->location = location;
	  this->payOff = payOff;
	  this->parentIndex = parentIndex;
	  //If root node samples = 0, else 1;
	  totalSample = parentIndex == 0 ? 0 : 1;
   } 
   void addPayoff(double pay){
	 payOff += pay;
   	 totalSample++;
   }
   auto getAveragePayoff() return payOff / samples;
   //Calculate payoff with UCB algorithm
   double getPayOff()
	  return ((payOff / samples) + 10 * sqrt((log(totalSamples)/samples)));
   
};

class MCTS
{
public:
   //Initialize the Tree
   std::vector<Move> InitializeTree(ChineseCheckersState &state);
   //Running total
   int total = 0;
   // the high-level function for computing the best move
   Move GetBestMove();
   // traverse down the tree (recursively),
   // returning the value at the leaf of the sample
   double SelectLeaf(uint32_t node); 
   // Use the UCB rule to find the best child
   uint32_t SelectBestChild(uint32_t node); 
   // Get the UCB value of a given node
   double GetUCBVal(uint32_t node, uint32_t parent); 
   // is the designated node a leaf
   bool IsLeaf(uint32_t node); 
   // expand the designated node and add its children to the tree
   void Expand(uint32_t node, ChineseCheckersState &state);
   // play out the game, returning the evaluation at the end of the game
   double DoPlayout(uint32_t node);

   int random(Move aMove, ChineseCheckersState &s);
   //totalSamples
   int totalSamples = 0;

private:
};

#endif 
