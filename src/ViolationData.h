#ifndef VIOLATION_DATA
#define VIOLATION_DATA

#include <map>
#include <vector>
#include <string>

using namespace std;
struct Violation
{
	string violationCode, streetName, violationInFrontOfOrOpposite;
};

struct PrecinctAggregation
{
	vector<Violation> violations;
};

typedef map<string, PrecinctAggregation> PrecinctDataMap;

#endif