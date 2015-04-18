#include "ChineseCheckersState.h"
#include "Agent.h"
#include "timer.cpp"

class ABMiniMax {
public: 
	int ABMiniMax(ChineseCheckersState &s, int depth, int player, Move &bestMove, bool &timeUp);
	int min;
	int max;
private:
	Move &bestMove;
};
