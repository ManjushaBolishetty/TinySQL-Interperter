#include "TupleProcessing.h"


extern MainMemory mem;
extern Disk disk;
extern SchemaManager schema_manager;
extern vector<string> VariablesFromTbl;
extern map<TColPair, FValue> Column_Value_Mapping;

Tuple_Comparison::Tuple_Comparison(vector<string> fieldlist)
{
	field_names = fieldlist;
}

bool Tuple_Comparison::operator()(Tuple& tuple1, Tuple& tuple2)
{
	if(tuple1.isNull() && tuple2.isNull())
		return false;
	if((!tuple1.isNull()) && tuple2.isNull())
		return true;
	if(tuple1.isNull() && (!tuple2.isNull()))
		return false;

	vector<string>::iterator start = field_names.begin();
	vector<string>::iterator stop = field_names.end();
	vector<string>::iterator index;
	Schema schema_tuple1 = tuple1.getSchema();
	Schema schema_tuple2 = tuple2.getSchema();

//	cout<< "comparing "<<tuple1<<"  "<<tuple2<<endl;
	if(schema_tuple1 != schema_tuple2)
		throw string("Error!! Tuple comparison failed - Tuple_Comparison::operator()");

	for(index = start; index!=stop; index++)
	{
		string column_name = *index;
		FIELD_TYPE field_type = schema_tuple1.getFieldType(column_name);
		if(field_type == INT)
		{
			int value1, value2;
			value1 = tuple1.getField(column_name).integer;
			value2 = tuple2.getField(column_name).integer;
			if(value1 != value2)
				return value1<value2;
		}
		else //FIELD_TYPE == STR20
		{
			string value1, value2;
			value1 = *(tuple1.getField(column_name).str);
			value2 =  *(tuple2.getField(column_name).str);
			if(value1.compare(value2) < 0) return true;
			else if(value1.compare(value2) > 0) return false;

		}
	}
	return false;
}
//compare two tables based on number of blocks
bool Size_Comparison(string first_relation, string second_relation)
{
	int blocks_first_relation = schema_manager.getRelation(first_relation)->getNumOfBlocks();
	int blocks_second_relation = schema_manager.getRelation(second_relation)->getNumOfBlocks();
	return blocks_first_relation < blocks_second_relation;
};

bool InMemorySort(int start_index, int num_blocks, vector<string> field_names)
{// InMemorySort for 2-pass algorithm
//count of blocks to sort in main memory
	vector<Tuple> mem_tuples = mem.getTuples(start_index, num_blocks);
	sort(mem_tuples.begin(), mem_tuples.end(), Tuple_Comparison(field_names));
	for(int index=start_index; index< num_blocks; index++)
	{
		mem.getBlock(index)->clear(); // clearing the blocks
	}
	bool success = mem.setTuples(start_index, mem_tuples);
	if(!success)
		cout<<"Error in InMemorySort"<<endl;
	return success;
}

bool IsEqualTuples(Tuple tup1, Tuple tup2)
{
	if(tup1.isNull() || tup2.isNull()) return false;
	Schema schemat1 = tup1.getSchema();
	Schema schemat2 = tup2.getSchema();

	if(schemat1!=schemat2) return false;

	vector<string> field_names = schemat1.getFieldNames();

	vector<string>::iterator myiterator;

	for(myiterator = field_names.begin(); myiterator!=field_names.end(); myiterator++)
	{
		if(schemat1.getFieldType(*myiterator) == INT)
		{
			if(tup1.getField(*myiterator).integer != tup2.getField(*myiterator).integer)
				return false;
		}
		else
		{
			string val1 = *(tup1.getField(*myiterator).str);
			string val2 = *(tup2.getField(*myiterator).str);
			if(val1 != val2)
				return false;
		}
	}

	return true;
}

bool IsEqualTuples(Tuple tup1, Tuple tup2, vector<string> fnames)
{
	if(tup1.isNull() || tup2.isNull())
		return false;
	Schema schemat1 = tup1.getSchema();
	Schema schemat2 = tup2.getSchema();

	if(schemat1!=schemat2) return false;

	vector<string> field_names = schemat1.getFieldNames();

	vector<string>::iterator myiterator;

	for(myiterator = fnames.begin(); myiterator!=fnames.end(); myiterator++)
	{
		if(schemat1.getFieldType(*myiterator) == INT)
		{
			if(tup1.getField(*myiterator).integer != tup2.getField(*myiterator).integer)
				return false;
		}
		else
		{
			string val1 = *(tup1.getField(*myiterator).str);
			string val2 = *(tup2.getField(*myiterator).str);
			if(val1 != val2)
				return false;
		}
	}
	return true;
}

int search_tuple(vector<Tuple> tuples, Tuple mytuple, vector<string> field_names)
{
	int index;
	for(index = 0; index<tuples.size(); index++)
	{
		if(IsEqualTuples(tuples[index], mytuple, field_names))
			return index;
	}
	return -1;
}

int search_tuple(vector<Tuple> tuples, Tuple mytuple)
{
	int index;
	for(index = 0; index<tuples.size(); index++)
	{
		if(IsEqualTuples(tuples[index], mytuple))
			return index;
	}
	return -1;
}

void Mapping_Tuple(Tuple map_tuple)
{
	Schema schema_tuple = map_tuple.getSchema();
	vector<string> myColmNames = schema_tuple.getFieldNames();
	vector<string>::iterator myiterator;
	for(myiterator = myColmNames.begin(); myiterator!=myColmNames.end(); myiterator++)
	{
		int period_position = (*myiterator).find_first_of('.');
		int len = (*myiterator).length();
		if(period_position == string::npos) throw string("Error in column Mapping");
		string tbl_name = (*myiterator).substr(0, period_position);
		string col_name = (*myiterator).substr(period_position+1, len - period_position - 1);
		FIELD_TYPE fieldtype = schema_tuple.getFieldType(*myiterator);
		Field myvalue = map_tuple.getField(*myiterator);
		TColPair tcolpair = TColPair(tbl_name, col_name);
		FValue value = FValue(fieldtype, myvalue);
		Column_Value_Mapping[tcolpair] = value;
	}
}

void Mapping_Tuple(Tuple map_tuple, string table_name)
{
	Schema schema_tuple = map_tuple.getSchema();
	vector<string> map_column_names = schema_tuple.getFieldNames();
	vector<string>::iterator myiterator;
	for(myiterator = map_column_names.begin(); myiterator!=map_column_names.end(); myiterator++)
	{
		FIELD_TYPE field_typ = schema_tuple.getFieldType(*myiterator);
		Field value = map_tuple.getField(*myiterator);
		TColPair tcolpair1 = TColPair(table_name, *myiterator);
		TColPair tcolpair2 = TColPair(NOT_A_TABLE, *myiterator);
		FValue values = FValue(field_typ, value);

		Column_Value_Mapping[tcolpair1] = values;
		Column_Value_Mapping[tcolpair2] = values;
	}
}


Tuple search_min_tuple(vector<Tuple> tuples, vector<string> field_names)
{
	vector<Tuple>::iterator myiterator;
	myiterator = min_element(tuples.begin(), tuples.end(), Tuple_Comparison(field_names));
	return *myiterator;
}
