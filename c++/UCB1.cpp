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
//UCB1::UCB1();

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
//Confidence Bound

//Lambda functions:
//Upperbound score, right hand side of the UCB1 algorithm
auto ucbFunc = [](int totalSample, double payOff, int numPlays){ return payOff + sqrt(2 * log (totalSample) / numPlays);};
//updates the average score
auto updateAvgScore = [](double totalScore, double aScore, int timesPlayed){ return (totalScore += aScore) / timesPlayed;};

//Also need to deal with timer or whatever
Move UCB1::UCB1move(ChineseCheckersState &state){

   std::vector<Move> actions;
   state.getMoves(actions);
   //Initialize 10 arms, one arm per player board piece
   std::vector<arm> theArms;
   
   //Start Timer - Note, this but not be needed here
   bool timeUp = false;
   auto duration = std::chrono::milliseconds(1000); //1s
   auto time = std::thread([&timeUp, duration](){ std::this_thread::sleep_for(duration); timeUp = true; });

   //Loop through each item and get the UCB1 values
   //Also update the values of each arm.
  for(int i = 0; i < actions.size(); ++i){ 
	  //Reward should be changing until it converges
	  //on the best path
	  //double payOff = randomHelperFunction(actions.at(i)); //Impliment this
	  arm tempArm; 
	  tempArm.m = actions.at(i);//Store move
	  //update the avgScore
	  tempArm.avgScore = updateAvgScore(tempArm.totalScore, 1.0, 1.0);
	  tempArm.timesPlayed = 1.0;
	  tempArm.totalScore = ucbFunc(totalSample, tempArm.payoff, tempArm.timesPlayed);
	  //Done, return to theArms list;
	  theArms.push_back(tempArm);  
   }
  //Keep track of bestArm
  arm bestArm = theArms.at(0);
  
  while(!timeUp){ //Replace true with timeup checker
	 //Get best arm by searching the list of arms 
	
     for(auto anArm : theArms) { if(anArm.totalScore > bestArm.totalScore) {bestArm = anArm; std::cerr << "New BestArm!" << std::endl; } }
	 //double payOff = randomHelperFunction(bestArm.m); //Again, need to impliment this
	 int payOff = 1; //Don't leave this...UCB1 will be useless
	 bestArm.totalScore += payOff;
     bestArm.avgScore = ucbFunc(totalSample, payOff, bestArm.timesPlayed); //update the avg score with ucb1 functions
  }
  time.join(); //End thread;
  std::cerr << "Done! Armsize = " << theArms.size() << std::endl;
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
   unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
   std::default_random_engine generator (seed);
   std::uniform_real_distribution<double> distribution (0.0,1.0);
   return distribution(generator);
}
//Helper Functions
int UCB1::randomPolicies(Move aMove, bool policyType){
   //Call a state
   //
   //NOTE, WE NEED a new CCState, or else more work will be needed
  ChineseCheckersState state = new ChineseCheckersState();
  //Get some data
  state.applyMove(aMove);
  std::vector<Move> moves;
  state.getMoves();

  //Increase our totalSamples, as another game is being sampled
  totalSample++;
  
  //Playout Policy 1
  if(policyType == 0){
   //This policy returns a 0 for loss or a 1 for win
	  while(!state.gameOver()){
		 //Only call the random part 15% of the time,
		 //The below part occures the other 85% of the time
		 if(getRand() > 0.15)
		 {
			//Avoiding for loops with lambdas. Note, might be wrong!!!!!
			std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b){return a.score < b.score;});
			Move bestMove = moves.at(0);
			std::cerr << "Best move from Lambda Policy 1 = " << bestMove << std::endl;
			//Apply Best Move
			state.applyMove(bestMove);
			
		 } else {
			//only happens 15% of the time
			state.applyMove(moves.at((int)(getRand()*moves.size())));
		 }
		 
	  }
	  //Return results
	  if(state.winner() == state.getCurrentPlayer()) return 1; else return 0; 
  }

  //Playout Policy 2
  if(policyType == 1){
   //This policy return the results being evaluated by the eval function

   //Allow to play to x moves
   const int movesToPlay = 75;
   for (int i = 0; i < movesToPlay; ++i){
	  //Check if the game is over, if so run eval function
	  if(state.gameOver()) return state.eval();
	  state.getMoves(); //We need to get the moves each time this time
	  if(getRand() > 0.15){
	  //Note, Again, this Might be wrong!!!
	  std::sort(moves.begin(), moves.end(), [](const Move &a, const Move &b){return  a.score < b.score;});
	  Move bestMove = moves.at(0);	 
	  std::cerr << "Best move from Lambda Policy 1 = " << bestMove << std::endl;
			//Apply Best Move
		 state.applyMove(bestMove);
			
	  } else {
		 //only happens 15% of the time
		 state.applyMove(moves.at((int)(getRand()*moves.size())));
	  }
   }
   //Return the eval function
   return state.eval();
  }
  std::cerr << "Something went wrong. Neither Policy seems to have returned anything" << std::endl;
  return 0; // In case something weird happens
}
