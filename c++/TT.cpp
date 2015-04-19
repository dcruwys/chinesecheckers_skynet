#include "TT.h"
#include <cstdint>
TT::TT () {};

TT::TTEntry TT::getEntry(uint64_t key){
	return TT::table.at(key);
}

TT::~TT(){};

void TT::storeEntry(uint64_t key, int score, int depth, int type){
	TT::TTEntry newEntry;
	newEntry.score = score;
	newEntry.depth = depth;
	newEntry.type = type;
	TT::table[key] = newEntry;
}
bool TT::inTable(uint64_t key){
	return (table.count(key) == 1);
}