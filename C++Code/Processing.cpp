#include "Processing.h"
#include "TupleOperations.h"

SelectOP::SelectOP()
{
	search_cond = NULL;
}

SelectOP::SelectOP(Search*& selection_cond)
{
	search_cond = selection_cond;
}

bool SelectOP::Issearch_cond()
{
	if(search_cond == NULL)
		return false;
	return true;
}

bool SelectOP::search_cond_compute()
{
	return search_cond->Compute();
}


void SelectOP::operator delete(void *pointer)
{
	if(pointer!=NULL)
	{
	if(((SelectOP*)pointer)->search_cond != NULL)
		delete ((SelectOP*)pointer)->search_cond;
	free(pointer);
	}
}

JOINOP::JOINOP(){}
JOINOP::JOINOP(vector<string> listTblPush)
{
	list_tbl_push=listTblPush;
}

vector<string> JOINOP::GetFromList()
{
	return list_tbl_push;
}
void JOINOP::push_tblname(string table_name)
{
	list_tbl_push.push_back(table_name);
}


ProjectOP::ProjectOP(){}
ProjectOP::ProjectOP(vector<MyColumn> column_list)
{
	mycolumns = column_list;
}
void ProjectOP::append_collist(MyColumn column)
{
	mycolumns.push_back(column);
}

vector<MyColumn> ProjectOP::fetchcolumn()
{
	return mycolumns;
}
vector<string> ProjectOP::fetchcolumnString()
{
	vector<string> string_col_list;
	vector<MyColumn>::iterator myiterator;
	for(myiterator = mycolumns.begin(); myiterator!=mycolumns.end(); myiterator++)
		string_col_list.push_back((*myiterator).colName);
		// cout << (*myiterator).colName << endl;
	return string_col_list;
}

vector<string> ProjectOP::fetchcolumnStringtable()
{
	vector<string> string_col_list;
	vector<MyColumn>::iterator myiterator;
	for(myiterator = mycolumns.begin(); myiterator!=mycolumns.end(); myiterator++)
	{
		if((*myiterator).tblName == NOT_A_TABLE)
		{
			string tabName = TScan::Fetch_Table_Name((*myiterator).colName);
			string_col_list.push_back(string(tabName).append(".").append(string((*myiterator).colName)));
		}
		else
			string_col_list.push_back(string((*myiterator).tblName).append(".").append((*myiterator).colName));
	}
	return string_col_list;
}

OrderOP::OrderOP(){}
OrderOP::OrderOP(vector<MyColumn> column_list)
{
	mycolumns = column_list;
}
void OrderOP::append_column_order(MyColumn mycolumn)
{
	mycolumns.push_back(mycolumn);
}

vector<MyColumn> OrderOP::fetchcolumn_order()
{
	return mycolumns;
}
