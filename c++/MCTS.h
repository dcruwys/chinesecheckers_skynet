#ifndef MCTS_H
#define MCTS_H

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <cmath>

//Are data type to be put in the tree

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
