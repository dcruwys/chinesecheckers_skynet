#ifndef __UCB1_H_INCLUDED__
#define __UCB1_H_INCLUDED__
#include "ChineseCheckersState.h"
#include "Agent.h"



class UCB1
{
public:
  // UCB1();
   int totalSample = 1;
   Move UCB1move(ChineseCheckersState &state, bool policy);
  // int moveDistance(Move aMove);
   double randomPolicies(Move aMove, bool policyType);
   int getTotalSamples();   
   UCB1();
   ~UCB1();
private:

};

#endif 
