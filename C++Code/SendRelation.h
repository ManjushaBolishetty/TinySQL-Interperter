#ifndef _SENDRELATION_H
#define _SENDRELATION_H
#include<vector>
#include<string>
#include"Tuple.h"
using namespace std;

struct SendRelation
{
	string relation_name;
	bool marked_For_Delete;
	vector<Tuple> tuples;
};

#endif
