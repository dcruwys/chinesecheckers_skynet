#include "ABMiniMax.h"
#include <vector>
#include <limits>
#include <algorithm>
using namespace timer;
int ABMiniMax(ChineseCheckersState &s, int depth, int alpha, int beta, int cplayer, Move &bestMove, bool MaximizingPlayer)
{
	if(state.getCurrentPlayer() == cplayer || !timer::timeUp())
	{
		//maximize!!!
		int value = std::numeric_limits<int>::min();
	    if(depth == 0 || s.getCurrentPlayer() == cplayer || timer::timeUp())
		{
			return Agent::eval(s, s.getCurrentPlayer())-depth);
		}
		std::vectore<Move> moves;
		s.getMoves(moves);
		Move temp = {0,0};
		for(const auto i : moves)
		{
			s.applyMove(i);
			//Call minimize!
			value = std::max(value, ABMiniMax(s, depth-1, alpha, beta, s.getCurrentPlayer(), bestMove, false));
			s.undoMove(i)
			if(value > alpha){ bestMove = i; alpha = value; }
    		if(beta <= alpha) break;
		}
		return alpha;
	} 
	else 
	{
		//Minimize
		int value = std::numeric_limits<int>::max();
		if(depth == 0 || s.gameOver() || timer::timeUp())
		{
			return Agent::eval(state, state.getCurrentPlayer())-depth;
		}
			std::vector<Move> moves;
			s.getMoves(moves);
			Move temp = {0, 0};
			for(const auto i: moves)
			{
				s.applyMove(i);
				//Maximize
				value = std::max(value, ABMiniMax(s, depth-1, alpha, beta, s.getCurrentPlayer(), temp, true));
				alpha = std::max(alpha, value);
				s.undoMove(i)
				if(value < beta) beta = value;
				if(beta <= alpha) break;
			}
		return beta;
	}
}