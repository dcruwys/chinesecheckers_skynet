#include <iostream>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <random>
#include <iterator>
#include <algorithm>
#include "Agent.h"
#include <thread>
#include "UCB1.h"
UCB1::UCB1 () {};
UCB1::~UCB1 () {};

//Struct for an arm to be searched in the bandit algorithm.
//All values initialized to 0. Basically we're treating
//each of the players positions on the board as an arm [fixed].
struct arm {
   Move m = {0,0};
   double payoff = 0.0;
   double avgScore = 0.0;
   double totalScore = 0.0;
   int timesPlayed = 0;
 
   arm(){};
   
   arm(Move m, double payoff, double avgScore, double totalScore, int timesPlayed)
   {
	  this->m = m;
	  this->payoff = payoff;
	  this->avgScore = avgScore;
	  this->totalScore = totalScore;
	  this->timesPlayed = timesPlayed;
   }
};


//Lambda functions:
//Upperbound score, right hand side of the UCB1 algorithm
auto ucbFunc = [](int totalSample, double payOff, int numPlays){ return payOff + sqrt(2 * log (totalSample) / numPlays);};
//updates the average score
auto updateAvgScore = [](double totalScore, double aScore, int timesPlayed){ return (totalScore += aScore) / timesPlayed;};

//Also need to deal with timer or whatever
Move UCB1::UCB1move(ChineseCheckersState &state, bool policy){

   std::vector<Move> actions;
   state.getMoves(actions);
   std::cerr << "Vector actions contains: " << actions.size() << std::endl;
   //Initialize 10 arms, one arm per player board piece
   std::vector<arm> theArms;
   std::cerr << "another current player check: " << state.getCurrentPlayer() << std::endl;
   std::cerr << "Vector theArms has been created, contains items: " << theArms.size() << std::endl;
   
   //Start Timer - Note, this but not be needed here
   bool timeUp = false;
   auto duration = std::chrono::milliseconds(3000); //1s
   auto time = std::thread([&timeUp, duration](){ std::this_thread::sleep_for(duration); timeUp = true; });
   //bool whichStrategy = state.getCurrentPlayer() == 2 ? 0 : 1; //Decides which strategy to use
   //Loop through each item and get the UCB1 values
   //Also update the values of each arm.
  for(int i = 0; i < actions.size(); ++i){ 
	  //Reward should be changing until it converges
	  //on the best path
	  //double payOff = randomHelperFunction(actions.at(i)); //Impliment this
	  arm tempArm; 
	  tempArm.m = actions.at(i);//Store move
	  std::cerr << "tempArm move = " << actions.at(i) << std::endl;
    //update the avgScore
	  tempArm.avgScore = updateAvgScore(tempArm.totalScore, 1.0, 1.0);
	  std::cerr << "avgScore = " << tempArm.avgScore << std::endl;
    tempArm.timesPlayed = 1.0;
	  tempArm.totalScore = ucbFunc(totalSample, tempArm.payoff, tempArm.timesPlayed);
	 std::cerr << "totalScore = " << tempArm.totalScore << std::endl;

    //Done, return to theArms list;
	  theArms.push_back(tempArm);  
    std::cerr << "Item added to theArms vector" << std::endl;
   }
   std::cerr << "all items added to theArms, size =  " << theArms.size() << std::endl;
  //Keep track of bestArm
  arm bestArm = theArms.at(0);
  std::cerr << "bestArm = " << actions.at(0) << std::endl;

  std::cerr << "CURRENT PLAYER CHECK: " << state.getCurrentPlayer() << std::endl;
  if(!timeUp){ //Replace true with timeup checker
	 //Get best arm by searching the list of arms 
	
     for(auto anArm : theArms) { if(anArm.totalScore > bestArm.totalScore) {bestArm = anArm; std::cerr << "New BestArm!" << std::endl; } }
	
	 //Call on the UCB1 policy functions
	 double payOff = randomPolicies(bestArm.m, policy, state); //
	 std::cerr << "payoff = " << payOff << std::endl;
 
	 //int payOff = 1; //Don't leave this...UCB1 will be useless
	 bestArm.totalScore += payOff;
   bestArm.avgScore = ucbFunc(totalSample, payOff, bestArm.timesPlayed); //update the avg score with ucb1 functions
        std::cerr << "totalScore = " << bestArm.totalScore <<  " avgScore = " << bestArm.avgScore << std::endl;

  }
  time.join(); //End thread;
  std::cerr << "Done! Armsize = " << theArms.size() << std::endl;
  std::cerr << "CURRENT PLAYER CHECK: " << state.getCurrentPlayer() << std::endl;
  //Return the move of our best arm
  return bestArm.m;
}

/*int retPolicyResults(int results){
   return results;
}
int UCB1::moveDistance(Move aMove){
   return abs(aMove.from + aMove.to);
}*/
//Get Random Number
//Using current time as a seed
//auto getRand = [](std::srand(std::time(0));){return std::rand();}
unsigned getRand()
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<> dis(0, 1);
   double rand = (double)dis(gen);
   std::cerr << "rand = " << rand << std::endl;
   return rand;
}
//Helper Functions
double UCB1::randomPolicies(Move aMove, bool policyType, ChineseCheckersState &s){
   //Call a state
   //
   //NOTE, WE NEED a new CCState, or else more work will be needed
   ChineseCheckersState state(s);
   int cPlayer = state.getCurrentPlayer();
   std::cerr << "State Cloned! Applying move, " << aMove << " checking current player in cloned: " << cPlayer << std::endl;
   std::cerr << "CURRENT PLAYER CHECK original s: " << s.getCurrentPlayer() << std::endl;
   state.applyMove(aMove);
   std::cerr << "Player after apply move check: " << cPlayer << std::endl;
   std::vector<Move> moves;
   state.getMoves(moves);
   std::cerr << "Player after get move check: " << cPlayer << std::endl;
  //Increase our totalSamples, as another game is being sampled
  auto rand = getRand();
  totalSample++;
  std::cerr << "sample increased, now = " << totalSample << std::endl;
  //Playout Policy 1
  if(policyType == 0){
    std::cerr << "Policy type 1" << std::endl;
   //This policy returns a 0 for loss or a 1 for win
	  while(!state.gameOver()){
		 std::cerr << "Is game over: " << state.gameOver() << std::endl;
		 //Only call the random part 15% of the time,
		 //The below part occures the other 85% of the time
		 if(rand > 0.15)
		 {
      std::cerr << "Our random number is greater than 0.15" << std::endl;
			//Avoiding for loops with lambdas. Note, might be wrong!!!!!
			std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b){return a.score < b.score;});
			Move bestMove = moves.at(0);
			std::cerr << "Best move from Lambda Policy 1 = " << bestMove << std::endl;
			//Apply Best Move
			state.applyMove(bestMove);
			std::cerr << "CURRENT PLAYER CHECK original s: " << state.getCurrentPlayer() << std::endl;

			
		 } else {
			//only happens 15% of the time
      //
			std::cerr << rand << " < 0.15" << std::endl;
      std::cerr << "Checking current player: " << state.getCurrentPlayer() << std::endl;
      std::cerr << "Policy 1: Attempting to applymove in vector location: " << (int)(rand * moves.size())
        << "\n Move attempting to be applied is: " << moves.at((int)(rand * moves.size())) << std::endl;
			state.applyMove(moves.at((int)(rand * moves.size())));
		 }
		 
	  }
	  //Return results
	  if(state.winner() == state.getCurrentPlayer()) return 1.0; else return 0.0; 
  }

  //Playout Policy 2
  if(policyType == 1){
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
	  std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b){return  a.score < b.score;});
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
  std::cerr << "Something went wrong. Neither Policy seems to have returned anything" << std::endl;
  return 0.0; // In case something weird happens
}
int UCB1::getTotalSamples(){
   return totalSample; 
}
