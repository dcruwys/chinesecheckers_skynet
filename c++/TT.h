#ifndef TT_H_INCLUDED
#define TT_H_INCLUDED
#include <unordered_map>
#include <cstdint>
#include <limits>

class TT{
public:
	TT();
	~TT();
	struct TTEntry
	{
		int score=0;
		int depth=0;
		int alpha=0;
		int beta=0;
	};
	TTEntry getEntry(uint64_t key);
	void storeEntry(uint64_t key, int score, int depth, int alpha, int beta);
    std::unordered_map<uint64_t, TTEntry> table;
};

#endif