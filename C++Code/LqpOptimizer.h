#ifndef _LqpOptimizer_H
#define _LqpOptimizer_H
#include<cstdlib>
#include "LogicalPlan.h"

class LqpOptimizer
{
	public:
		static LogicalPlan* LogicalQueryplanOptimizer(LogicalPlan*& lqp);
};

#endif
