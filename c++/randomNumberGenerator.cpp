#include <iostream>
#include <random>
#include <fstream>
#include <cstdint>
#include <limits>

using namespace std;

const int numLoc = 81;
const int numValues = 3;

//uint64_t rands[numLoc][numValues];
//81*3
//For pos 0, entry 0 => randNumber
int main()
{
	ofstream outFile;
	outFile.open("randomtable.txt");
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> 
        dis(0, std::numeric_limits<uint64_t>::max());

    outFile << "const static rands[] = {\n";
	for(int x = 0; x < numLoc; x++){
		for(int y = 0; y < numValues; y++){
			//std::cout<< dis(gen()) << std::endl;
			uint64_t nextRand = dis(gen);
			//nextRand = nextRand<<32;
			//nextRand |= random();
			outFile << nextRand << ",\n";
			// rands[x][y] = nextRand; 
		}
	}
	outFile << "};\n";
		outFile.close();
}