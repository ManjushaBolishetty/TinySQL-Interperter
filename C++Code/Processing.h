#ifndef _PROCESSING_H
#define _PROCESSING_H
#include <vector>
#include <cstdlib>
#include "find.h"
#include "DB_config.h"



using namespace std;
struct MyColumn
{
	string tblName;
	string colName;
};

class JOINOP
{
private:
	vector<string> list_tbl_push; //list of table table names to be pushed here
public:
	JOINOP();
	JOINOP(vector<string> listTblPush);
	void push_tblname(string table_name);
	vector<string> GetFromList();
};
class ProjectOP
{
private:
	vector<MyColumn> mycolumns;
public:
	ProjectOP();
	ProjectOP(vector<MyColumn> column_list);
	void append_collist(MyColumn column);
	vector<MyColumn> fetchcolumn();
	vector<string> fetchcolumnString();
	vector<string> fetchcolumnStringtable();
};
class SelectOP
{
private:
	Search* search_cond;
public:
	SelectOP();
	SelectOP(Search*& search_cond);
	bool search_cond_compute();
	bool Issearch_cond();
	void operator delete(void *pointer);
};


class OrderOP
{
private:
	vector<MyColumn> mycolumns;
public:
	OrderOP();
	OrderOP(vector<MyColumn> column_list);
	void append_column_order(MyColumn mycolumn);
	vector<MyColumn> fetchcolumn_order();
};
#endif
