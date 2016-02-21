#ifndef _TUPLE_PROCESSING_H
#define _TUPLE_PROCESSING_H
#include <iostream>
#include <string>
#include <deque>
#include <queue>
#include <algorithm>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Tuple.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "DB_Config.h"

using namespace std;

typedef pair<string, string> TColPair;
typedef pair<FIELD_TYPE, Field> FValue;


class Tuple_Comparison {
	private:
		vector<string> field_names;
	public:
		Tuple_Comparison(vector<string> fieldlist);
		bool operator()(Tuple& tuple1, Tuple& tuple2);
};

bool Size_Comparison(string first_relation, string second_relation);

bool InMemorySort(int start_index, int num_blocks, vector<string> field_name); //inmemory sort for tuples in main memory

bool IsEqualTuples(Tuple tup1, Tuple tup2); //check if two tuples are same if yes return true else false
bool IsEqualTuples(Tuple tup1, Tuple tup2, vector<string> fieldnames);

int search_tuple(vector<Tuple> tuples, Tuple mytuple); // get the 1st index of the tuple n search
int search_tuple(vector<Tuple> tuples, Tuple mytuple, vector<string> field_name);


//Function to map the column values in a global space
void Mapping_Tuple(Tuple map_tuple);
void Mapping_Tuple(Tuple map_tuple, string table_name);
Tuple search_min_tuple(vector<Tuple> tuples, vector<string> field_names);

#endif
