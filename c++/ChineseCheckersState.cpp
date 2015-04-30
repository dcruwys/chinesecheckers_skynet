//===------------------------------------------------------------*- C++ -*-===//
#include "ChineseCheckersState.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include "randomtable.txt"
#include <cstdint>
#include "TT.h"

//queue used to remove backward moves
//Needs to be cleared every time getMoves
//is called to avoid wasteful memory allocs
uint64_t zHash = 0;

std::vector<Move> queue; 

Move::operator std::string() const{
  std::stringstream ss;
  ss << "MOVE FROM " << from << " TO " << to;
  return ss.str();
}
bool Move::operator<(const Move &a){
    return score < a.score;
  }

bool operator==(const Move &lhs, const Move &rhs) {
  return lhs.from == rhs.from && lhs.to == rhs.to;
}
// Lexicographic
bool operator<(const Move &lhs, const Move &rhs) {
  return lhs.from < rhs.from || (!(rhs.from < lhs.from) && lhs.to < rhs.to);
}

std::ostream &operator<<(std::ostream &out, const Move &m) {
  out << "{" << m.from << " -> " << m.to << "}";
  return out;
}


ChineseCheckersState::ChineseCheckersState() {
  reset();
}

void ChineseCheckersState::getMoves(std::vector<Move> &moves) const {
  // WARNING: This function must not return duplicate moves
  moves.clear();
  queue.clear();

  for (unsigned i = 0; i < 81; ++i) {
    if (board[i] == currentPlayer) {
      
      getMovesSingleStep(queue, i);
      //std::cout << i << std::endl;
      getMovesJumpStep(queue, i, i);
      // Need to add jump moves

      //Remove Backward moves
      //Note, the forward distance calculator
      //Might not work for player2

    }
  }
  //std::cerr << "Size of Queue = " <<  queue.size() << std::endl;
    for(auto &move : queue)
    {
        //Calculate forward distance
        //push the move to the back of the queue vector
        if(forwardDistance(move) > 0) {
          moves.push_back(move);
          //std::cerr << "zHash = " << zHash << " move = " << forwardDistance(move) << std::endl;
        }
        //std::cerr << move << std::endl;

      }  
  if(moves.size() == 0) {
        moves = queue;
        //std::cerr << "Queue is empty, returning original move list.\n A backwards or sideways move will be made" << std::endl;
  }
}
int ChineseCheckersState::forwardDistance(Move move) const {
        int fromRow = move.from / 9;
        int toRow = move.to / 9;
        int fromCol = move.from % 9;
        int toCol = move.to % 9;
        int mult = 1;
        if (getCurrentPlayer() == 2)
            mult = -1;
        return ((toRow + toCol) - (fromRow + fromCol))*mult;
  }


bool ChineseCheckersState::applyMove(Move m) {
  // Ensure the from and to are reasonable
  if (m.from > 80 || m.to > 80 || m.from == m.to)
    return false;
  //uint64_t temp = board[m.from];
  int currentPlayer = getCurrentPlayer();
  zHash ^= rands[m.from * 3 + currentPlayer];
  zHash ^= rands[m.from * 3];

  //temp = board[m.to];

  zHash ^= rands[m.to * 3 + currentPlayer];
  zHash ^= rands[m.to * 3];

//  std::cerr << "zHash = " << zHash << std::endl;

  // Check the move
  // FIXME: This should be uncommented once you have getMoves working!!
  /*
  if (!isValidMove(m))
    return false;
  */

  // Apply the move
  std::swap(board[m.from], board[m.to]);

  // Update whose turn it is
  swapTurn();

  return true;
}

bool ChineseCheckersState::undoMove(Move m) {
  // Ensure the from and to are reasonable
  if (m.from > 80 || m.to > 80 || m.from == m.to)
    return false;
  //uint64_t temp = board[m.to];
  int currentPlayer = getCurrentPlayer();

  zHash ^= rands[m.to * 3 + currentPlayer];
  zHash ^= rands[m.to * 3];

  //temp = board[m.from];

  zHash ^= rands[m.from * 3 + currentPlayer];
  zHash ^= rands[m.from * 3];
  // Undo the move
  std::swap(board[m.from], board[m.to]);
  swapTurn();

  // Check the move is valid from this state that is back one step
  if (!isValidMove(m)) {
    // Woops, it was not valid, undo our changes
    swapTurn();
    std::swap(board[m.from], board[m.to]);

    return false;
  }

  return true;
}



uint64_t ChineseCheckersState::getZHash(){
  //std::cerr << "zHash = " << zHash << std::endl;
  return zHash;
}

bool ChineseCheckersState::gameOver() const {
  return player1Wins() || player2Wins();
}

int ChineseCheckersState::winner() const {
  if (player1Wins())
    return 1;
  if (player2Wins())
    return 2;
  return -1; // No one has won
}

void ChineseCheckersState::reset() {
  board = {{1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
            0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0,
            0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2}};
  currentPlayer = 1;

}

bool ChineseCheckersState::isAdded(std::vector<Move> &moves, Move position) const{
    for(auto i : moves)
        if(i == position) return true;
    return false;
}

bool ChineseCheckersState::loadState(const std::string &newState) {
  // Tokenize newState using whitespace as delimiter
  std::stringstream ss(newState);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> tokenized(begin, end);

  // Ensure the length
  if (tokenized.size() != 82)
    return false;

  // Validate first item, whose turn it is
  if (tokenized[0] != "1" && tokenized[0] != "2")
    return false;

  try {
    currentPlayer = std::stoi(tokenized[0]);
  } catch (std::invalid_argument e) {
    return false;
  } catch (std::out_of_range e) {
    return false;
  }

  // Ensure rest of tokens are valid
  for (size_t i = 1, e = tokenized.size(); i != e; ++i) {
    try {
      int val = std::stoi(tokenized[i]);
      if (0 <= val && val <= 2)
        board[i - 1] = val;
      else
        return false;
    } catch (std::invalid_argument e) {
      return false;
    } catch (std::out_of_range e) {
      return false;
    }
  }
  return true;
}

void ChineseCheckersState::getMovesJumpStep(std::vector<Move> &moves, unsigned from, unsigned ogpostion) const {
    const unsigned row = from / 9;
    const unsigned col = from % 9;
    // Up Left
    if ((from >=2 ) && (col > 1 && board[from - 1] != 0 && board[from - 2] == 0) && !isAdded(moves,{ogpostion, from-2})){
        //std::cout << "Up Left" << std::endl;
        //temp = {from, from-2};
        moves.push_back({ogpostion, from - 2});
        getMovesJumpStep(moves, from -2, ogpostion);
    }
    // Up Right !!
    if ((from >= 18) &&(row > 1 && board[from - 9] != 0 && board[from-18] == 0) && !isAdded(moves, {ogpostion, from-18})){
        //std::cout << "Up Right" << std::endl;
        moves.push_back({ogpostion, from - 18});
        getMovesJumpStep(moves, from - 18, ogpostion);
    }
    // Left
    if ((from <= 64) && (col > 1 && row < 7 && board[from + 8] != 0 && board[from + 16] == 0) && !isAdded(moves, {ogpostion, from+16})){
        //std::cout << "Left" << std::endl;
        moves.push_back({ogpostion, from + 16});
        getMovesJumpStep(moves, from + 16, ogpostion);
    }
    // Right
    if ((from >= 16) && (col < 7 && row > 1 && board[from - 8] != 0 && board[from - 16] == 0) && !isAdded(moves, {ogpostion, from-16})){
        //std::cout << "Right" << std::endl;
        moves.push_back({ogpostion, from - 16});
        getMovesJumpStep(moves, from - 16, ogpostion);
    }
    // Down Left !!
    if ((from <= 62) && (row < 7 && board[from + 9] != 0 && board[from + 18] == 0) && !isAdded(moves, {ogpostion, from+18})){
        //std::cout << "Down Left" << std::endl;
        moves.push_back({ogpostion, from + 18});
        getMovesJumpStep(moves, from + 18, ogpostion);
    }
    // Down Right
    if ((from <= 78) && (col < 7 && board[from + 1] != 0 && board[from + 2] == 0) && !isAdded(moves, {ogpostion, from + 2})){
        //std::cout << "Down Right" << std::endl;
        moves.push_back({ogpostion, from + 2});
        getMovesJumpStep(moves, from + 2, ogpostion);
    }
    //recursive function
}


std::string ChineseCheckersState::dumpState() const {
  std::stringstream out;
  out << currentPlayer;
  for (const auto i : board)
    out << " " << i;

  return out.str();
}

void ChineseCheckersState::getMovesSingleStep(std::vector<Move> &moves, unsigned from) const {
  unsigned row = from / 9;
  unsigned col = from % 9;

  // Up Left
  if (col > 0 && board[from - 1] == 0)
    moves.push_back({from, from - 1});

  // Up Right
  if (row > 0 && board[from - 9] == 0)
    moves.push_back({from, from - 9});

  // Left
  if (col > 0 && row < 8 && board[from + 8] == 0)
    moves.push_back({from, from + 8});

  // Right
  if (col < 8 && row > 0 && board[from - 8] == 0)
    moves.push_back({from, from - 8});

  // Down Left
  if (row < 8 && board[from + 9] == 0)
    moves.push_back({from, from + 9});

  // Down Right
  if (col < 8 && board[from + 1] == 0)
    moves.push_back({from, from + 1});
}

bool ChineseCheckersState::isValidMove(const Move &m) const {
  // Ensure from and to make sense
  if (board[m.from] != currentPlayer || board[m.to] != 0)
    return false;

  // NOTE: Checking validity in this way is inefficient

  // Get current available moves
  std::vector<Move> moves;
  getMoves(moves);

  // Find the move among the set of available moves
  bool found = std::find(moves.begin(), moves.end(), m) != moves.end();

  return found;
}

Move ChineseCheckersState::translateToLocal(const std::vector<std::string> &tokens) const {
  // The numbers in the MOVE command sent by the moderator is already in the
  // format we need
  return Move{static_cast<unsigned>(std::stoi(tokens[2])),
              static_cast<unsigned>(std::stoi(tokens[4]))};
}

void ChineseCheckersState::swapTurn() {
  currentPlayer = currentPlayer == 1 ? 2 : 1;
}

bool ChineseCheckersState::player1Wins() const {
  // Wins by having all the bottom triangle filled and at least one is from the
  // first player

  bool p1inTriangle = false;
  for (const auto i : {53u, 61u, 62u, 69u, 70u, 71u, 77u, 78u, 79u, 80u}) {
    if (board[i] == 0)
      return false;
    if (board[i] == 1)
      p1inTriangle = true;
  }

  return p1inTriangle;
}

bool ChineseCheckersState::player2Wins() const {
  // Wins by having all of top triangle filled and at least one is from the
  // second player

  bool p2inTriangle = false;
  for (const auto i : {0u, 1u, 2u, 3u, 9u, 10u, 11u, 18u, 19u, 27u}) {
    if (board[i] == 0)
      return false;
    if (board[i] == 2)
      p2inTriangle = true;
  }

  return p2inTriangle;
}

Move ChineseCheckersState::heurstic(){ //it kinda works but needs to be fixed.
    std::vector<Move> moves;
    getMoves(moves);
    Move currentMax = {0, 0};
    int maxDiff = -100;
    /*if(currentPlayer == 2){
      currentMax = {80, 80};
    }*/
    for(auto &i : moves) {
      unsigned hexRowFrom = i.from / 9 + i.from % 9;
      unsigned hexRowTo = i.to / 9 + i.to % 9;

      int diff = (static_cast<int>(hexRowTo) - static_cast<int>(hexRowFrom)) * (currentPlayer == 1 ? 1 : -1);

      if (diff > maxDiff) {
        currentMax = i;
        maxDiff = diff;
      }

      /*
      if (currentPlayer == 1 && diff > maxDiff) {
        currentMax = i;
        maxDiff = diff;
      } else if (currentPlayer == 2 && -diff > maxDiff) {
        currentMax = i;
        maxDiff = -diff;
      }
      */
      /*
      if((currentPlayer == 1) && ((i.to / 9 + i.to % 9) - (i.from / 9 + i.from % 9)) 
        > ((currentMax.to / 9 + currentMax.to % 9) - (currentMax.from / 9 + currentMax.from % 9)))
          {
            currentMax = i;
          }
      if((currentPlayer == 2) && ((ChineseCheckersStateChineseCheckersStatei.to / 9 + i.to % 9) - (i.from / 9 + i.from % 9)) 
        < ((currentMax.to / 9 + currentMax.to % 9) - (currentMax.from / 9 + currentMax.from % 9)))
          {
            currentMax = i;
          }
          */
    }
    return currentMax;
}

int ChineseCheckersState::getCurrentPlayer() const{
    return currentPlayer;
}

std::array<int, 81> ChineseCheckersState::getBoard() const{
    return board;
}