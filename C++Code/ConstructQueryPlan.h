//Program to construct the Logical query plan
#ifndef _ConstructQueryPlan_H
#define _ConstructQueryPlan_H
#include <algorithm>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "SendRelation.h"
#include "Processing.h"
#include "TupleOperations.h"
#include "TableProcessing.h"
#include "TupleProcessing.h"

enum IDENTIFY_LEAF {TERMINAL = 1, NON_TERMINAL}; // all terminals are leaf and non-terminal are non leaf
class ConstructQueryPlan
{
private:
		SendRelation sendRelation;
		IDENTIFY_LEAF type;
		ConstructQueryPlan* from_left_QP;
		ConstructQueryPlan* from_right_QP;
		static int saveToDB;
public:
		ConstructQueryPlan(SendRelation send_reln);
		ConstructQueryPlan(ConstructQueryPlan*& left_QP, ConstructQueryPlan*& right_QP);
		SendRelation Compute();
		static ConstructQueryPlan* Create_optimum_QP(vector<string> from_list);
		void operator delete(void *pointer);
		static void set_num_SaveCount(int count);
	  static int get_num_SaveCount();
};

#endif
