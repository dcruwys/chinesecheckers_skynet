#include "TT.h"
#include <cstdint>

TT::TTEntry TT::getEntry(uint64_t key){
	return TT::table.at(key);
}

void TT::storeEntry(uint64_t key, int score, int depth, int alpha, int beta){
	TT::TTEntry newEntry;
	newEntry.score = score;
	newEntry.depth = depth;
	newEntry.alpha = alpha;
	newEntry.beta = beta;
	TT::table[key] = newEntry;
}

