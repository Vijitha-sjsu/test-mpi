#ifndef MPI_PARSER
#define MPI_PARSER

#include <string>
#include <vector>

using namespace std;
class MPIParser
{

public:
	string serializeVector(vector<string> localChunk);
	vector<string> deserializeVector(string serialized);
};

#endif