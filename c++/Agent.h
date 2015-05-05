//===------------------------------------------------------------*- C++ -*-===//
#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include <string>
#include <vector>

#include "ChineseCheckersState.h"

class Agent {
public:
  Agent();
  void playGame();

private:
  Move nextMove();
  void printAndRecvEcho(const std::string &msg) const;
  std::string readMsg() const;
  std::vector<std::string> tokenizeMsg(const std::string &msg) const;
  void waitForStart();
  void switchCurrentPlayer();

  bool isValidStartGameMessage(const std::vector<std::string> &tokens) const;
  bool isValidMoveMessage(const std::vector<std::string> &tokens) const;
  int eval(ChineseCheckersState &state, int depth);
  int minimax(ChineseCheckersState &state, int depth, bool max, Move &bestMove, bool &timeUp, int alpha, int beta);
  Move ideepening(ChineseCheckersState &state);
  void hhInsert(Move aMove, int depth);
  ChineseCheckersState state;
  enum Players { player1, player2 };
  Players current_player;
  Players my_player;
  std::string name;
  std::string opp_name;
};


#endif
