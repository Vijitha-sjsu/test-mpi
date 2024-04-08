#include "MPIParser.h"
#include <iostream>
using namespace std;

string MPIParser::serializeVector(vector<string> localChunk)
{
	std::string serialized;
	for (const auto &str : localChunk)
	{
		// Prepend the length of the string before the string data
		serialized += std::to_string(str.size()) + ":" + str;
	}

	return serialized;
}

vector<string> MPIParser::deserializeVector(string serialized)
{
	std::vector<std::string> vec;
	size_t pos = 0;
	while (pos < serialized.size())
	{
		size_t colonPos = serialized.find(':', pos);
		size_t strLen = std::stoi(serialized.substr(pos, colonPos - pos));
		pos = colonPos + 1; // Move past the colon
		vec.push_back(serialized.substr(pos, strLen));
		pos += strLen;
	}
	return vec;
}