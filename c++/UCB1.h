#ifndef __UCB1_H_INCLUDED__
#define __UCB1_H_INCLUDED__
#include "ChineseCheckersState.h"
#include "Agent.h"

class UCB1
{
public:
  // UCB1();
   int totalSample = 1;
   Move UCB1move(ChineseCheckersState &state);
  // int moveDistance(Move aMove);
   int randomPolicies(Move aMove, bool policyType);

private:

};

#endif 
