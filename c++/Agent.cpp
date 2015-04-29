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

Agent::Agent() : name("tables") {}
bool finished = false;

//uint64_t zHash = 0;

Move Agent::nextMove() {
    // Somehow select your next move
    Move bestMove = {0,0};
    bool timeUp = false;
    bestMove = ideepening(state);
    //minimax(state, 3, true, bestMove, timeUp, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    return bestMove;
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
      std::cout << eval(state) << std::endl;
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

int Agent::eval(ChineseCheckersState &state){
  int cplayer = state.getCurrentPlayer();
  int winner = state.winner();
  if(cplayer == winner)
    return std::numeric_limits<int>::max();
  else if(winner != -1)
    return std::numeric_limits<int>::min();

  int p2score = 0;
  int p1score = 0;
  std::array<int, 81> board = state.getBoard();
  for(int i = 0; i < 81; ++i){
    if (board[i] != 0) {
      int currentlocation = (i % 9 + i / 9);
      if (board[i] == 1)
        p1score += 16 - currentlocation;
      else
        p2score += currentlocation;
    }
  }

  if (cplayer == 2){
    return p2score - p1score;
  }
  return p1score - p2score;
}

//Minimax calls the min and max function.
int Agent::minimax(ChineseCheckersState &state, int depth, bool max, Move &bestMove, bool &timeUp, int alpha, int beta){
  int value;
  if(!timeUp){
    if(state.table.inTable(state.getZHash())){
      TT::TTEntry tte = state.table.getEntry(state.getZHash());
      if(tte.depth >= depth){ // should I set a move here?
        if(tte.type == 0)
          return tte.score;
        if(tte.type == -1 && tte.score > alpha)
          alpha = tte.score;
        else if(tte.type == 1 && tte.score < beta)
          beta = tte.score;
        if(alpha >= beta)
          return tte.score;
      }
    }
    if(depth == 0 || state.gameOver()){
      finished = true;
      value = eval(state);
      if(value <= alpha)
        state.table.storeEntry(state.getZHash(), value, depth, 1);
      else if(value >= beta)
        state.table.storeEntry(state.getZHash(), value, depth, -1);
      else
        state.table.storeEntry(state.getZHash(), value, depth, 0);
      return value - depth;
    } 
    if(max){
      value = std::numeric_limits<int>::min();
      std::vector<Move> moves;
      state.getMoves(moves);
      Move tempMove = {0,0};
      for(const auto i: moves){
        state.applyMove(i);
        int minimaxValue = minimax(state, depth-1, false, tempMove, timeUp, alpha, beta);
        state.undoMove(i);
        if(minimaxValue > value){
          value = minimaxValue;
          bestMove = i;
        }
        alpha = std::max(alpha, value);
        if(beta <= alpha)
          break; //PRUNE!!
      }
    }
    else{
      value = std::numeric_limits<int>::max();
      std::vector<Move> moves;
      state.getMoves(moves);
      Move tempMove = {0,0};
      for(const auto i: moves){
        state.applyMove(i);
        int minimaxValue = minimax(state, depth-1, true, tempMove, timeUp, alpha, beta);
        state.undoMove(i);
        if(minimaxValue < value){
          value = minimaxValue;
        }
        beta = std::min(beta, value);
        if(beta <= alpha)
          break; //PRUNE!!
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
  else{
    //std::cerr << "FINISHED: " << bestMove << std::endl;
    return eval(state);
  }
}

Move Agent::ideepening(ChineseCheckersState &state){
  bool timeUp = false;
  Move bestMove = {0,0};
  Move tempMove = {0,0};
  auto duration = std::chrono::milliseconds(10000 - 200); //10s
  auto t = std::thread([&timeUp, duration](){ std::this_thread::sleep_for(duration); timeUp = true; });
 int depth = 0;
 while(!timeUp){  
    finished = false;
    std::cerr << depth << std::endl;
    minimax(state, depth, true, tempMove, timeUp, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    ++depth;
    if(finished){
      bestMove = tempMove;
     std::cerr << bestMove << std::endl;
     std::cerr << tempMove << std::endl;
    }
  }
  t.join();
  return bestMove;
}



//We need to make a seperate program to generate
//Random numbers and save them to a file for this
//Program to read.


