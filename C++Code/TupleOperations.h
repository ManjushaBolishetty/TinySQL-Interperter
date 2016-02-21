#ifndef _TUPLE_OPERATIONS_H
#define _TUPLE_OPERATIONS_H
#include <iostream>
#include <queue>
#include <string>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Tuple.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Processing.h"
#include "DB_config.h"
#include "SendRelation.h"
#include "TableProcessing.h"
#include "TupleProcessing.h"


using namespace std;

extern MainMemory mem;
extern Disk disk;
extern SchemaManager schema_manager;

class TScan {

	private:
		string relation;
		int index_memblock_begin; //memeory block starting index
		int index_memblock_end;// memeory block ending index
		int curpass_toread_numblocks; // number of blocks to read in the current pass.
		int reln_numblocks; //number of blocks in a relation -reln
		int reln_numtuples;// number of tuples in a given relation
		int tobescanned_numblocks;// number blocks that remain to be scanned.
		Relation* relation_pointer;
		int blockIndex_curreln; //block idnex of current relation
		int tupleIndex_curr; // index of current tuple
		int memIndex_curr; //index of current memory.
		vector<Tuple> tuples_curr;

	public:
		TScan(string myrelation, int index_memblock_begin, int index_memblock_end);
		// The  function reads all tuples from a given relation(disk block) to the memory block,
		// reads these in a vector of tuples and returns them.
		static vector<Tuple> getTuples(string table_name, int blockIndex_mem);
		static vector<Tuple> getTuples(bool distinct, string table_name, int blockIndex_mem, vector<MyColumn> order_by);
		static vector<Tuple> getUniqueTuples(string table_name, int blockIndex_mem);

		//  fetches all the tuples from a relation on a disk to memory block
		// reads that in a tuple, returns only tuples with matching condition.
		static vector<Tuple> GetSelectTuples(string table_name, int blockIndex_mem, SelectOP& selection_op);
		static vector<Tuple> GetSelectTuples(bool isunique, string table_name, int blockIndex_mem, SelectOP& selection_op, vector<MyColumn> order_by);

		bool GetNextTuple(Tuple& tuple);
		//table scan fetches all the colmn in the ptoject operation.
		static vector<Tuple> getTuples(ProjectOP project_op, SelectOP& select_op, string table_name, int blockIndex_mem, string temp_relation);

		static vector<Tuple> getTuples(bool isunique, ProjectOP project_op, SelectOP& select_op, string table_name, int blockIndex_mem, string temp_relation, vector<MyColumn> order_by);
		static vector<Tuple> GetProd(string tbl1, string tbl2, string tempT, bool saveToDb);
		static SendRelation ExecuteProjectOP(ProjectOP project_op, JOINOP join_op,
							SendRelation return_relation, bool save);
		static string Fetch_Table_Name(string column_name);
		static vector<Tuple> ExecuteSelectOP(SelectOP& select_op, SendRelation return_relation);
	  	static vector<Tuple> GetUniqueTuplesCorrected(bool isunique, ProjectOP project_op, SelectOP& select_op, string table_name, int blockIndex_mem, string temp_relation, vector<MyColumn> order_by);
		static vector<Tuple> Ordering_Relation(vector<Tuple> tuple, string table_name, vector<MyColumn> order_by);
	  	static vector<Tuple> GetUniqueOrderedTuples(vector<Tuple> tuples, bool isunique, ProjectOP project_op, string table_name, int blockIndex_mem, vector<MyColumn> order_by);
};

class Relation_Sublist {
	private:
		bool queVector_mem;
		int num_lastlist_block;
		void Remove_All(int index, Tuple tuple);
		void Remove(int index);
		bool GetQueue(int index);
	public:
		Relation* relation_pointer;
		string relation_name;
		int num_blocks;
		Schema relation_schema;
		int num_sublists;
		vector<string> mycolumns;
		vector<int> index_of_sublists;
		vector<deque<Tuple> > queueVector_mem;
		map<int, int> map_block_relIndex;
		Relation_Sublist(string relation_name, vector<string> mycolumns);
		Tuple GetDistinctTuples();
		Tuple GetOrderedTuples();
};

void SaveTupleTodisk(Relation*& relation_pointer, vector<Tuple> tuples, int index_mem);

#endif
