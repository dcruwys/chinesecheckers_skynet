#ifndef MCTS_H
#define MCTS_H

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <cmath>

//Are data type to be put in the tree

class MCTS
{
public:
   MCTS(ChineseCheckersState &s);
   ~MCTS();
   //Initialize the Tree
   void InitializeTree(ChineseCheckersState &state);
   //Running total
   // the high-level function for computing the best move
   Move GetBestMove();
   // traverse down the tree (recursively),
   // returning the value at the leaf of the sample
   void Expand(uint32_t node);
   double getRand();
   double random(Move aMove);
   double DoPlayout(Move move, uint32_t parent);
   double SelectLeaf(uint32_t node, bool &timeUp);
   double getUCBVal(uint32_t node); 
   // Use the UCB rule to find the best child
   uint32_t SelectBestChild(uint32_t node);  
   // is the designated node a leaf
   bool IsLeaf(uint32_t node); 
   // expand the designated node and add its children to the tree
  
   	
private:
};

#endif 
