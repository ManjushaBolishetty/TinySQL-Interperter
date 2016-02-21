//Program creates a simple logical query plan without subqueries
#ifndef _LOGICAL_PLAN_H
#define _LOGICAL_PLAN_H
#include <vector>
#include "Processing.h"
#include "TupleOperations.h"
#include "ConstructQueryPlan.h"
#include "TableProcessing.h"
#include "SendRelation.h"


enum OPtype {SELECT_OP, PROJECT, ORDERBY, VECTORPRODUCT,
						Optimum_PROJECT_S_PRODUCT, //Optimized (Single table)Product Followed by Project - A simple "select */a,b,c from table" case
						Optimum_PROJECT_S_PRODUCT_SELECT, //select */a,b,c from <tblename> where <condition>
						Optimum_PROJECT_M_PRODUCT, // No Condition, More than one table involved
						Optimum_PROJECT_M_PRODUCT_SELECT
						};

class LogicalPlan
{
		private:
				bool isUnique;
				vector<MyColumn> order_BY;
				SendRelation sendRelation;
				OPtype operationType;
				OrderOP orderByOp;
				ProjectOP projectOp;
				SelectOP selectOp;
				JOINOP productOp;
				ConstructQueryPlan* construct_QP;
				LogicalPlan* left_LogicalQP;
				LogicalPlan* right_LogicalQP;
		public:
				LogicalPlan();
				LogicalPlan(LogicalPlan*& left, LogicalPlan *&right);
				void Add_Left_LPQ(LogicalPlan*& lp);
				void Add_Right_LPQ(LogicalPlan*& rp);
				static LogicalPlan* CreateSelectQueryPlan(SelectOP selectop);
				static LogicalPlan* CreateProjectQueryPlan(ProjectOP projectop);
				static LogicalPlan* CreateJoinQueryPlan(JOINOP joinop);
				static LogicalPlan* CreateProjSelJoinQueryPlan(bool isDistinct, ProjectOP projectop, SelectOP& selectop, JOINOP joinop, vector<MyColumn> orderBy);
				static LogicalPlan* CreateProjJoinQueryPlan(bool isDistinct, ProjectOP projectop, JOINOP joinop, vector<MyColumn> orderBy);
				LogicalPlan* FetchOptimizedQP();
				SendRelation Execute();
				void operator delete(void *p);
	};
#endif
