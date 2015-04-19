#ifndef TT_H_INCLUDED
#define TT_H_INCLUDED
#include <unordered_map>
#include <cstdint>
#include <limits>

class TT{
public:
	struct TTEntry
	{
		int score=0;
		int depth=0;
		int type=0;
	};
	TT();
	~TT();
	TTEntry getEntry(uint64_t key);
	void storeEntry(uint64_t key, int score, int depth, int type);
	bool inTable(uint64_t key);
    std::unordered_map<uint64_t, TTEntry> table;
private:
	
};

#endif