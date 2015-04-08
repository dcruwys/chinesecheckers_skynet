#include "Agent.h"
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <limits>

Agent::Agent() : name("MyName") {}

Move Agent::nextMove() {
    // Somehow select your next move
    return state.heurstic();
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
    } else if(tokens[0] == "DLDFS"){
        DLDFS(state, 3);
    }
    else if (response == "LISTMOVES") {
      std::vector<Move> moves;
      state.getMoves(moves);
      for (const auto i : moves)
        std::cout << i.from << ", " << i.to << "; ";
      std::cout << std::endl;
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

int Agent::eval(ChineseCheckersState &state, int cplayer){
  int winner = state.winner();
  if(cplayer == winner)
    return std::numeric_limits<int>::max();
  else if(winner != -1)
    return std::numeric_limits<int>::min();
  return 0;
}
//basic DLDFS search, changes nothing.
void Agent::DLDFS(ChineseCheckersState &state, int depth){
  //std::cout << "Test:" << std::endl;
  if(depth == 0 || state.gameOver()){
    return;
  }
  std::vector<Move> moves;
  state.getMoves(moves);
  for(const auto i: moves){
    state.applyMove(i);
    //std::cout << state.dumpState() << std::endl;
    DLDFS(state, depth -1);
    state.undoMove(i);
  }
}

//max function for minimax. Returns the MAX player's best state.
int Agent::max(ChineseCheckersState &state, int depth){
  if(depth == 0 || state.gameOver()){
    return eval(state, state.getCurrentPlayer());
  }
  int bestValue =  std::numeric_limits<int>::min();
  std::vector<Move> moves;
  state.getMoves(moves);
  for(const auto i: moves){
    state.applyMove(i);
    //std::cout << state.dumpState() << std::endl;
    int value = max(state, depth -1);
    if(value > bestValue)
      bestValue = value;
    state.undoMove(i);
  }
  return bestValue;
}

//min function for minimax. Returns the MIN player's best state.
int Agent::min(ChineseCheckersState &state, int depth){
  if(depth == 0 || state.gameOver()){
    return eval(state, state.getCurrentPlayer());
  }
  int bestValue =  std::numeric_limits<int>::max();
  std::vector<Move> moves;
  state.getMoves(moves);
  for(const auto i: moves){
    state.applyMove(i);
    //std::cout << state.dumpState() << std::endl;
    int value = min(state, depth -1);
    if(value < bestValue)
      bestValue = value;
    state.undoMove(i);
  }
  return bestValue;
}
//Minimax calls the min and max function.
int Agent::minimax(ChineseCheckersState &state, int depth){
  if(state.getCurrentPlayer() == 1){
    return max(state, depth);
  }
  else if(state.getCurrentPlayer() == 2){
    return min(state, depth);
  }
  return 0;
}


