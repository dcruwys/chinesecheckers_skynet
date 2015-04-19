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
  int eval(ChineseCheckersState &state, int cplayer);
  int max(ChineseCheckersState &state, int depth, Move &bestMove, bool &timeUp, int alpha, int beta);
  int min(ChineseCheckersState &state, int depth, Move &bestMove, bool &timeUp, int alpha, int beta);
  int minimax(ChineseCheckersState &state, int depth, int cplayer, Move &bestMove, bool &timeUp);
  void ideepening(ChineseCheckersState &state, int cplayer, Move &bestMove);

  ChineseCheckersState state;
  enum Players { player1, player2 };
  Players current_player;
  Players my_player;
  std::string name;
  std::string opp_name;
};


#endif
