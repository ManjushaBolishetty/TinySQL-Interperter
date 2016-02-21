#include"LogicalPlan.h"


LogicalPlan::LogicalPlan()
{
		left_LogicalQP = NULL;
		right_LogicalQP = NULL;
		construct_QP = NULL;
		isUnique = false;
}

LogicalPlan::LogicalPlan(LogicalPlan*& left, LogicalPlan *&right)
{
		left_LogicalQP = left;
		right_LogicalQP = right;
}

void LogicalPlan::Add_Left_LPQ(LogicalPlan*& lp)
{
		left_LogicalQP = lp;
}
void LogicalPlan::Add_Right_LPQ(LogicalPlan*& rp)
{
		right_LogicalQP = rp;
}

LogicalPlan* LogicalPlan::CreateSelectQueryPlan(SelectOP selectop)
{//logical query plan for select operation
		LogicalPlan* lp = new LogicalPlan();
		lp->operationType = SELECT_OP;
		lp->selectOp = selectop;
		return lp;
}

LogicalPlan* LogicalPlan::CreateProjectQueryPlan(ProjectOP projectop)
{//logical query plan for Projection operation
		LogicalPlan* lp = new LogicalPlan();
		lp->operationType = PROJECT;
		lp->projectOp = projectop;
		return lp;
}

LogicalPlan* LogicalPlan::CreateJoinQueryPlan(JOINOP joinop)
{ //logical query plan for cross product
		LogicalPlan* lp = new LogicalPlan();
		lp->operationType = VECTORPRODUCT;
		lp->productOp = joinop;
		return lp;
}

LogicalPlan* LogicalPlan::CreateProjSelJoinQueryPlan(bool isDistinct, ProjectOP projectop, SelectOP& selectop, JOINOP joinop, vector<MyColumn> ordrBy)
{//logical query plan for Projection,selection and join operations
		LogicalPlan* lp = new LogicalPlan();
		lp->operationType = Optimum_PROJECT_S_PRODUCT_SELECT;
		lp->projectOp = projectop;
		lp->selectOp = selectop;
		lp->productOp = joinop;
		lp->isUnique = isDistinct;
		lp->order_BY = ordrBy;
		vector<string> from_list = joinop.GetFromList();
		if(from_list.size() >1)
		{
				ConstructQueryPlan::set_num_SaveCount(from_list.size() - 2);
				ConstructQueryPlan* construct_QP = ConstructQueryPlan::Create_optimum_QP(from_list);
				lp->construct_QP = construct_QP;
				lp->operationType = Optimum_PROJECT_M_PRODUCT;
		}
		return lp;
}

LogicalPlan* LogicalPlan::CreateProjJoinQueryPlan(bool isDistinct, ProjectOP projectop, JOINOP joinop, vector<MyColumn> orderBy)
{//logical query plan for Projection and join operation
		LogicalPlan* lp = new LogicalPlan();
		vector<string> table_names = joinop.GetFromList();
		lp->isUnique = isDistinct;
		lp->order_BY = orderBy;
		if(table_names.size() == 1)
		{
				// cout << "In LogicalPlan::CreateProjJoinQueryPlan, Here for DISTINCT" << endl;
				//No Optimization required
				lp->productOp = joinop;
				lp->projectOp = projectop;
				lp->operationType = Optimum_PROJECT_S_PRODUCT;
		}
		else
		{
				//From Query Optimized for table size
				ConstructQueryPlan* construct_QP = ConstructQueryPlan::Create_optimum_QP(table_names);
				ConstructQueryPlan::set_num_SaveCount(table_names.size() - 2);
				//Save temp tables to database
				lp->productOp = joinop;
				lp->projectOp = projectop;
				lp->construct_QP = construct_QP;
				lp->operationType = Optimum_PROJECT_M_PRODUCT;
		}
		return lp;
}

LogicalPlan* LogicalPlan::FetchOptimizedQP()
	{
		LogicalPlan* optimized_plan = new LogicalPlan();
		switch(operationType)
		{
		case PROJECT:
			optimized_plan->projectOp = projectOp;

			if(left_LogicalQP != NULL) //must be 1 for a project opration.
			{
				LogicalPlan*& arguments_lp = left_LogicalQP;
				if(arguments_lp->operationType == VECTORPRODUCT)
				{
					vector<string> table_names = arguments_lp->productOp.GetFromList();
					if(table_names.size() == 1)
					{
						//A simple case of one table select
						//Create a single logical plan by merging product and project operation
						optimized_plan->productOp = arguments_lp->productOp;
						optimized_plan->operationType = Optimum_PROJECT_S_PRODUCT;

					}
				}
				else if(arguments_lp->operationType == SELECT_OP)
				{

					SelectOP& selectop = arguments_lp->selectOp;
					optimized_plan->selectOp = selectop;
					if(arguments_lp->left_LogicalQP != NULL)
					{
						LogicalPlan*& argument_lp2 = arguments_lp->left_LogicalQP;
						if(argument_lp2->operationType == VECTORPRODUCT)
						{
							vector<string> table_names = argument_lp2->productOp.GetFromList();
							if(table_names.size() == 1)
							{
								//A simple case of one table select
								//Create a single logical plan by merging product and project operation
								optimized_plan->productOp = argument_lp2->productOp;
								optimized_plan->operationType = Optimum_PROJECT_S_PRODUCT;
							}
						}
						else {
							throw string("Select Argument must have a product");
						}
					}
					else
					{
						throw string("Exception - Select Operation failed - Invalid arguments");
					}
				}
			}
			else
				throw string("Exception - Project Operation failed - Invalid arguments");
			break;
		}
		return optimized_plan;
	}

SendRelation LogicalPlan::Execute()
{
	// cout << "Entering LogicalPlan::Execute" << endl;
	// cout << operationType << endl;
	SendRelation retn_relation;
	switch(operationType)
	{
	case Optimum_PROJECT_S_PRODUCT: //1 table 1 product
			if(projectOp.fetchcolumn().front().colName == "*")
			{

				string relation = productOp.GetFromList().front();
				vector<Tuple> tuples = TScan::getTuples(isUnique, relation, 0, order_BY);
				retn_relation.marked_For_Delete = false;
				retn_relation.relation_name = relation;
				retn_relation.tuples = tuples;
			}
			else
			{
				//If project list has few columns
				// cout << "isUnique value: "<< isUnique << endl;
				string relation = productOp.GetFromList().front();
				string tempTable = TableProcessing::rand_table();
				vector<Tuple> tuples = TScan::getTuples(isUnique, projectOp, selectOp, relation, 0, tempTable, order_BY);
				retn_relation.relation_name = tempTable;
				retn_relation.marked_For_Delete = true;
				retn_relation.tuples = tuples;
			}
			break;
	case Optimum_PROJECT_S_PRODUCT_SELECT: //Single product.. only one table
			if(projectOp.fetchcolumn().front().colName == "*")
			{
				string relation = productOp.GetFromList().front();
				// cout << "Executed productOp.GetFromList().front()" << endl;
				// cout << relation << endl;
				// cout << isUnique << endl;
				vector<Tuple> tuples = TScan::GetSelectTuples(isUnique, relation, 0, selectOp, order_BY);
				// cout << "Executed TScan::GetSelectTuples" << endl;
				retn_relation.marked_For_Delete = false; //it is permanent table, do not delete it after printing its information
				retn_relation.relation_name = relation;
				retn_relation.tuples = tuples;
			}
			else
			{
				//If project list has few columns
				string relation = productOp.GetFromList().front();
				string tempTable = TableProcessing::rand_table();
				vector<Tuple> tuples = TScan::getTuples(isUnique, projectOp, selectOp, relation, 0, tempTable, order_BY);
				retn_relation.relation_name = tempTable; //delete this relation after print
				retn_relation.marked_For_Delete = true;
				retn_relation.tuples = tuples;
			}
			break;
	case Optimum_PROJECT_M_PRODUCT:
			sendRelation = construct_QP->Compute();
			bool persist = false;
			if(selectOp.Issearch_cond())
				sendRelation.tuples = TScan::ExecuteSelectOP(selectOp, sendRelation);
			sendRelation.tuples = TScan::GetUniqueOrderedTuples(sendRelation.tuples, isUnique, projectOp, sendRelation.relation_name, 0, order_BY);
			if(projectOp.fetchcolumn().front().colName != "*")
				sendRelation = TScan::ExecuteProjectOP(projectOp, productOp, sendRelation, persist);
			retn_relation = sendRelation;
			break;
	}
	return retn_relation;
}

void LogicalPlan::operator delete(void *pointer)
{
		if(pointer!=NULL)
		{
				if(((LogicalPlan*)pointer)->left_LogicalQP != NULL)
				delete ((LogicalPlan*)pointer)->left_LogicalQP;

				if(((LogicalPlan*)pointer)->right_LogicalQP != NULL)
				delete ((LogicalPlan*)pointer)->right_LogicalQP;
				free(pointer);
		}
}
