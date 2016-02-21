#ifndef _TABLEPROCESSING_H
#define _TABLEPROCESSING_H
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
class TableProcessing
{
	static int index ;
public:
	static string rand_table()
	{
		index = index+1;
		char buffer[10];
		stringstream mystring;
		mystring << index;
		string mystring_index = mystring.str();
		mystring_index.append("tablename");
		return mystring_index;
	}
};

#endif
