//Generates Optimized Query plan.
#ifndef _GeneratorLQP_H
#define _GeneratorLQP_H
#include <cstdlib>
#include "LogicalPlan.h"
#include "find.h"
#include "Processing.h"

class GeneratorLQP
{
	public:
		static LogicalPlan* GenerateLQPlan(vector<MyColumn> project_list, vector<string> from_list, Search*& search_cond);
		static LogicalPlan* GenerateOptimizedLQPlan(bool isUnique, vector<MyColumn> project_list, vector<string> from_list, Search*& search_cond, vector<MyColumn> orderby_list);
		//static LogicalPlan* GenerateLQPlan(vector<MyColumn> project_list, vector<string> from_list);
};
#endif
