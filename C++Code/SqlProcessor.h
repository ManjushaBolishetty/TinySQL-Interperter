#ifndef _SqlProceesor_H
#define _SqlProceesor_H
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <fstream>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "TupleProcessing.h"
#include "find.h"
#include "LogicalPlan.h"
using namespace std;

extern MainMemory mem;
extern Disk disk;
extern SchemaManager schema_manager;

class SqlProcessor
{
private:
	bool TupleInitialization(Tuple& tuple, vector<string> attribute_list, vector<string> value_list);
	bool Append_Tuple_toRel(Relation* rel_pointer, int mem_blockindex, Tuple& tuple);

public:
	SqlProcessor();

	bool CreateTbl(string table_name, vector<string> attribute_namelist, vector<enum FIELD_TYPE> datatype_list, ofstream& qout_file, int out_option);

	bool InsertTbl(string tableName, vector<string> attributeList, vector<string> valueList);

	bool SelectTbl(LogicalPlan*& lp, ofstream& qout_file, int out_option);

	SendRelation InsertSelectTbl(LogicalPlan*& lp);

	bool Insert_After_Select_Tbl(string table_name, vector<Tuple> tuple_list);

	bool DropTbl(string table_name, ofstream& qout_file, int out_option);

	bool DeleteTbl(string table_name, Search*& selOpn);

};

#endif
