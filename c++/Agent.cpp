#include "Agent.h"
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <limits>
#include <chrono>
#include <thread>
#include <unordered_map>
#include "randomtable.txt" //Import array of random values
#include "TT.h"
#include <algorithm>
#include "UCB1.h"
#include "MCTS.h"
Agent::Agent() : name("tables") {}
//Opening Book Arrays
//const int obSize = 7;
Move openingbookP1[6] = {{3, 12}, {1, 21}, {18, 22}, {22, 31}, {0, 40}, {27,28}};
Move openingbookP2[6] = {{77, 68}, {79, 59}, {62, 58}, {58, 49}, {80, 40}, {53, 52}};
//History Heuristic table
int hh[81][81] = {};  //Move to in class
int turn = 0;
int rootPlayer;
int inital = 0;
bool debug = true;
//uint64_t zHash = 0;
Move Agent::nextMove() {
    // Somehow select your next move
    Move bestMove = {0, 0};
    bool timeUp = false;
    //int value = minimax(state, 3, true, bestMove, timeUp, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    //Opening book moves
    //Checks who the current player is
    //And executes those 'finely crafted' moves
    // if(turn < 6){
    //   if(state.getCurrentPlayer() == 1){
    //     bestMove = openingbookP1[turn];
    //     turn++;
    //   }
    //   if(state.getCurrentPlayer() == 2){
    //     bestMove = openingbookP2[turn];
    //     turn++;
    //   } 
    // } else {
     MCTS tree(state);
     bestMove = tree.GetBestMove();
    // }
    // if(!state.isValidMove(bestMove))
    //    bestMove = ideepening(state);
    return bestMove;
   
	//Determine the policy type for player to use
   //bool policyType = state.getCurrentPlayer() == 1 ? 0 : 1; 
   //UCB1 aUCBAgent; //Not sure if this should go in header
   //std::cerr << "Number of sims done with UCB1: " << aUCBAgent.getTotalSamples() << std::endl;
   //Return the move UCB1 found
   //return aUCBAgent.UCB1move(state, policyType);

	
}

void Agent::playGame() {
  // Identify myself
  std::cout << "#name " << name << std::endl;

  // Wait for start of game
  waitForStart();

  // Main game loop
  for (;;) {
    if (current_player == my_player) {
      // My turn

      // Check if game is over
      if (state.gameOver()) {
        std::cerr << "I, " << name << ", have lost" << std::endl;
        switchCurrentPlayer();
        continue;
      }

      // Determine next move
      const Move m = nextMove();

      // Apply it locally
      state.applyMove(m);

      // Tell the world
      printAndRecvEcho(m);

      // It is the opponents turn
      switchCurrentPlayer();
    } else {
      // Wait for move from other player
      // Get server's next instruction
      std::string server_msg = readMsg();
      const std::vector<std::string> tokens = tokenizeMsg(server_msg);

      if (tokens.size() == 5 && tokens[0] == "MOVE") {
        // Translate to local coordinates and update our local state
        const Move m = state.translateToLocal(tokens);
        state.applyMove(m);

        // It is now my turn
        switchCurrentPlayer();
      } else if (tokens.size() == 4 && tokens[0] == "FINAL" &&
                 tokens[2] == "BEATS") {
        // Game over
        if (tokens[1] == name && tokens[3] == opp_name)
          std::cerr << "I, " << name << ", have won!" << std::endl;
        else if (tokens[3] == name && tokens[1] == opp_name)
          std::cerr << "I, " << name << ", have lost." << std::endl;
        else
          std::cerr << "Did not find expected players in FINAL command.\n"
                    << "Found '" << tokens[1] << "' and '" << tokens[3] << "'. "
                    << "Expected '" << name << "' and '" << opp_name << "'.\n"
                    << "Received message '" << server_msg << "'" << std::endl;
        break;
      } else {
        // Unknown command
        std::cerr << "Unknown command of '" << server_msg << "' from the server"
                  << std::endl;
      }
    }
  }
}

// Sends a msg to stdout and verifies that the next message to come in is it
// echoed back. This is how the server validates moves
void Agent::printAndRecvEcho(const std::string &msg) const {
  // Note the endl flushes the stream, which is necessary
  std::cout << msg << std::endl;
  const std::string echo_recv = readMsg();
  if (msg != echo_recv)
    std::cerr << "Expected echo of '" << msg << "'. Received '" << echo_recv
              << "'" << std::endl;
}

// Reads a line, up to a newline from the server
std::string Agent::readMsg() const {
  std::string msg;
  std::getline(std::cin, msg); // This is a blocking read

  // Trim white space from beginning of string
  const char *WhiteSpace = " \t\n\r\f\v";
  msg.erase(0, msg.find_first_not_of(WhiteSpace));
  // Trim white space from end of string
  msg.erase(msg.find_last_not_of(WhiteSpace) + 1);

  return msg;
}

// Tokenizes a message based upon whitespace
std::vector<std::string> Agent::tokenizeMsg(const std::string &msg) const {
  // Tokenize using whitespace as a delimiter
  std::stringstream ss(msg);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> tokens(begin, end);

  return tokens;
}

void Agent::waitForStart() {
  for (;;) {
    std::string response = readMsg();
    const std::vector<std::string> tokens = tokenizeMsg(response);

    if (tokens.size() == 4 && tokens[0] == "BEGIN" &&
        tokens[1] == "CHINESECHECKERS") {
      // Found BEGIN GAME message, determine if we play first
      if (tokens[2] == name) {
        // We go first!
        opp_name = tokens[3];
        my_player = player1;
        break;
      } else if (tokens[3] == name) {
        // They go first
        opp_name = tokens[2];
        my_player = player2;
        break;
      } else {
        std::cerr << "Did not find '" << name
                  << "', my name, in the BEGIN command.\n"
                  << "# Found '" << tokens[2] << "' and '" << tokens[3] << "'"
                  << " as player names. Received message '" << response << "'"
                  << std::endl;
        std::cout << "#quit" << std::endl;
        std::exit(EXIT_FAILURE);
      }
    } else if (response == "DUMPSTATE") {
      std::cout << state.dumpState() << std::endl;
    } else if (tokens[0] == "LOADSTATE") {
      std::string new_state = response.substr(10);
      if (!state.loadState(new_state))
        std::cerr << "Failed to load '" << new_state << "'\n";
    } else if (tokens[0] == "MOVENEXT"){
        state.applyMove(nextMove());
    } else if (response == "LISTMOVES") {
      std::vector<Move> moves;
      state.getMoves(moves);
      for (const auto i : moves)
        std::cout << i.from << ", " << i.to << "; ";
      std::cout << std::endl;
    } else if (response == "EVAL") {
      // somehow evaluate the state like: eval(state, state.getCurrentPlayer())
      std::cout << eval(state, 0) << std::endl;
    } else if (tokens[0] == "MOVE") {
      // Just apply the move
      const Move m = state.translateToLocal(tokens);
      if (!state.applyMove(m))
        std::cout << "Unable to apply move " << m << std::endl;
    } else {
      std::cerr << "Unexpected message " << response << "\n";
    }
  }

  // Game is about to begin, restore to start state in case DUMPSTATE/LOADSTATE/LISTMOVES
  // were used
  state.reset();

  // Player 1 goes first
  current_player = player1;
}

void Agent::switchCurrentPlayer() {
  current_player = (current_player == player1) ? player2 : player1;
}

bool Agent::isValidStartGameMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 4 && tokens[0] == "BEGIN" && tokens[1] == "CHINESECHECKERS";
}

bool Agent::isValidMoveMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 5 && tokens[0] == "MOVE" && tokens[1] == "FROM" &&
    tokens[3] == "TO";
}
/////////////////////////////////////////
///////History Heuristic Control/////////
/////////////////////////////////////////
void Agent::hhInsert(Move aMove, int depth){
  hh[aMove.from][aMove.to] += depth;
}

/////////////////////////////////////////
///////////EVAL FUNCTION/////////////////
/////////////////////////////////////////
int Agent::eval(ChineseCheckersState &state, int depth){
  int winner = state.winner();
  if(rootPlayer == winner)
    return std::numeric_limits<int>::max() - depth;
  else if(winner != -1)
    return std::numeric_limits<int>::min() - depth;

  int p2score = 0;
  int p1score = 0;
  std::array<int, 81> board = state.getBoard();
  for(int i = 0; i < 81; ++i){
    if (board[i] != 0) {
      int currentlocation = ((i % 9) + (i / 9));
      if (board[i] == 1)
        p1score += currentlocation;
      else
        p2score += 16-currentlocation;
    }
  }
  if (rootPlayer == 2){ 
    return p2score - p1score;
  }
  return p1score - p2score;
}

void Agent::sort(std::vector<Move> &moves){
  if(rootPlayer == state.getCurrentPlayer()){
    for(auto &m : moves){
      m.score = hh[m.from][m.to];
    }
    std::sort(moves.begin(), moves.end(), compareScore);
  }
}
////////////////////////////////////////////
//////////////UCB1/////////////////////////
///////////////////////////////////////////

/*Move Agent::UCB1Simple(ChineseCheckersState &state){
   //Determine the policy type for player to use
   bool policyType = state.getCurrentPlayer() == 1 ? 0 : 1; 
   UCB1 aUCBAgent(); //Not sure if this should go in header
   return aUCBAgent.UCB1move(state, policyType);
}*/

//Minimax IS the min and max function. Contains Alpha Beta
////////////////////////////////////////////
////////Minimax With Alpha Beta/////////////
////////////////////////////////////////////
int Agent::minimax(ChineseCheckersState &state, int depth, bool max, Move &bestMove, bool &timeUp, int alpha, int beta){
  if(!timeUp){
    int value;
    //Get Zobrist Hash value
    uint64_t zHash = state.getZHash();
    //std::cerr << "zHash [minimax] state.getZHash = " << zHash << std::endl;
    //Assign a history heuristic score to all the moves
    std::vector<Move> moves;
    state.getMoves(moves);
    sort(moves);
    //Transposition table stuff
    if(state.table.inTable(zHash)){
      TT::TTEntry tte = state.table.getEntry(zHash);
      //std::cerr << "zHash [minimax] = " << zHash << " found in TT" << std::endl;
     if(tte.depth >= depth){
        if(tte.type == 0)
         return tte.score;
        else if(tte.type == -1 && tte.score > alpha)
         alpha = tte.score;
        else if(tte.type == 1 && tte.score < beta)
          beta = tte.score;
        if(alpha >= beta){
          return tte.score;
        }
      }
    } 
    if(depth == 0 || state.gameOver()){
      value = eval(state, depth);
      /*if(value <= alpha)
        state.table.storeEntry(zHash, value, depth, -1);
      else if(value >= beta)
        state.table.storeEntry(zHash, value, depth, 1);
      else
        state.table.storeEntry(zHash, value, depth, 0);*/
      return value;
   } 
    if(max){
      for(const auto i: moves){
        state.applyMove(i);
        value = minimax(state, depth-1, false, bestMove, timeUp, alpha, beta);
        state.undoMove(i);
        if(depth == inital && debug == true)
           std::cerr << "Next Move: " << i << " " << value << " " << i.score << std::endl;
        if(value > alpha){
          if(inital == depth)
            bestMove = i;
          alpha = value;
        }
        if(alpha >= beta){
          hhInsert(i, depth); 
          return value;
        }
      }
    }
    else{
      for(const auto i: moves){
        state.applyMove(i);
        value = minimax(state, depth-1, true, bestMove, timeUp, alpha, beta);
        state.undoMove(i);
        if(value < beta)
          beta = value;
        if(alpha >= beta){
          hhInsert(i, depth); 
          return value;
        }
      }
    }
    if(value <= alpha)
      state.table.storeEntry(state.getZHash(), value, depth, -1);
    else if(value  >= beta)
      state.table.storeEntry(state.getZHash(), value, depth, 1);
    else
      state.table.storeEntry(state.getZHash(), value, depth, 0);
    return value;
  }
  else 
   return std::numeric_limits<int>::min();
}

Move Agent::ideepening(ChineseCheckersState &state){
 bool timeUp = false;
 rootPlayer = state.getCurrentPlayer();
 auto duration = std::chrono::milliseconds(10000); //10s
 auto t = std::thread([&timeUp, duration](){ std::this_thread::sleep_for(duration); timeUp = true; });
 int depth = 1;
 Move bestMove = {0,0};
 Move tempMove = {0,0};
 while(!timeUp){  
    inital = depth;
    std::cerr << depth << std::endl;
    int value = minimax(state, depth, true, tempMove, timeUp, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    ++depth;
    if(value > std::numeric_limits<int>::min())
      bestMove = tempMove;
    else
	   break;
 }
  t.join();
  if(debug)
    std::cerr << "Best " << bestMove << std::endl;
  return bestMove;
}
