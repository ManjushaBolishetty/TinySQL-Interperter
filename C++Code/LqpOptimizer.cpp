#include"LqpOptimizer.h"

LogicalPlan* LqpOptimizer::LogicalQueryplanOptimizer(LogicalPlan*& logical_query_plan)
{
		return logical_query_plan->FetchOptimizedQP();
}
