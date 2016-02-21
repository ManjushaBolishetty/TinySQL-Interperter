
#include "TupleOperations.h"

extern vector<string> VariablesFromTbl;
extern map<TColPair, FValue> Value_Map_Col;

TScan::TScan(string myrelation, int index_memblock_start, int size)
{
	relation = myrelation;
	index_memblock_begin = index_memblock_start;
	curpass_toread_numblocks = size;
	if(curpass_toread_numblocks < 1) throw string("invalid agruments");

	relation_pointer = schema_manager.getRelation(myrelation);
	if(relation_pointer == NULL)
		throw string("Error!!!");

	reln_numblocks = relation_pointer->getNumOfBlocks();
	reln_numtuples = relation_pointer->getNumOfTuples();

	tobescanned_numblocks = reln_numblocks;

	if(tobescanned_numblocks <= curpass_toread_numblocks)
		curpass_toread_numblocks = tobescanned_numblocks;

	blockIndex_curreln = 0;
	tupleIndex_curr = 0;
	memIndex_curr = index_memblock_start;

	bool pass = relation_pointer->getBlocks(blockIndex_curreln, index_memblock_begin, curpass_toread_numblocks);
	if(pass)
	{
		tuples_curr = mem.getTuples(index_memblock_begin, curpass_toread_numblocks);
	}
	else
	{
		throw string("Scanning Error!!");
	}

}

void InitializingNewTuple(Tuple& newTuple, Tuple oldTuple)
{
	Schema schema_new = newTuple.getSchema();
	Schema schema_original = oldTuple.getSchema();
	vector<string> field_names = schema_new.getFieldNames();
	vector<string>::iterator myiterator;
	for(myiterator = field_names.begin(); myiterator != field_names.end(); myiterator++)
	{
		if(schema_original.getFieldType(*myiterator) == STR20)
			newTuple.setField(*myiterator, *(oldTuple.getField(*myiterator).str));
		else
			newTuple.setField(*myiterator, oldTuple.getField(*myiterator).integer);
	}

}

Schema GetNewSchema(ProjectOP project_op, Relation* relation_pointer)
{
	vector<string> new_Column_names;
	vector<FIELD_TYPE> new_field_types;
	vector<MyColumn> columns = project_op.fetchcolumn();
	vector<MyColumn>::iterator begin_iterator = columns.begin();
	vector<MyColumn>::iterator end_iterator = columns.end();
	vector<MyColumn>::iterator iterator;
	Schema schema = relation_pointer->getSchema();

	for(iterator = begin_iterator; iterator != end_iterator; iterator++)
	{
		string column_name = iterator->colName;
		FIELD_TYPE field_type = schema.getFieldType(column_name);
		if(field_type!= INT && field_type != STR20)
			throw string("Error!!!");
		new_Column_names.push_back(column_name);
		new_field_types.push_back(field_type);
	}
	return Schema(new_Column_names, new_field_types);
}

Schema GetNewSchema(Relation*& relation1, Relation*& relation2)
{
	string relation1_name = relation1->getRelationName();
	string relation2_name = relation2->getRelationName();
	Schema schema_relation1 = relation1->getSchema();
	Schema schema_relation2 = relation2->getSchema();
	vector<string> new_Column_Names;
	vector<FIELD_TYPE> new_field_types;

	vector<string> relation1_field_names = schema_relation1.getFieldNames();
	vector<string>::iterator myiterator;
	for(myiterator = relation1_field_names.begin(); myiterator != relation1_field_names.end(); myiterator++)
	{
		string temp;
		if((*myiterator).find('.') == string::npos)
			temp = string(relation1_name).append(".").append(*myiterator);
		else
			temp = *myiterator;

		FIELD_TYPE new_field_type = schema_relation1.getFieldType(*myiterator);

		new_Column_Names.push_back(temp);
		new_field_types.push_back(new_field_type);
	}

	vector<string> relation2_field_names = schema_relation2.getFieldNames();
	for(myiterator = relation2_field_names.begin(); myiterator != relation2_field_names.end(); myiterator++)
	{
		string temp1;
		if((*myiterator).find('.') == string::npos)
			temp1 = string(relation2_name).append(".").append(*myiterator);
		else
			temp1 = *myiterator;

		FIELD_TYPE new_field_type2 = schema_relation2.getFieldType(*myiterator);

		new_Column_Names.push_back(temp1);
		new_field_types.push_back(new_field_type2);
	}
	if(new_Column_Names.size() > 8)
		throw string("No of columns exceeds the given size of 8 fields");

	return Schema(new_Column_Names, new_field_types);
}


// The  function reads all tuples from a given relation(disk block) to the memory block,
// reads these in a vector of tuples and returns them.
vector<Tuple> TScan::getTuples(string table_name, int index_memblock_end)
{
	vector<Tuple> myTuples;
	Relation *relation_pointer = schema_manager.getRelation(table_name);
	Block* block_pointer;
	if(relation_pointer == NULL)
		throw string("Given relation ").append(table_name).append(" doesn't exist");
	int num_blocks = relation_pointer->getNumOfBlocks();
	block_pointer = mem.getBlock(index_memblock_end);
	if(block_pointer == NULL)
		throw string("An error has occured while fetching the tuples!!!");

	for (int index = 0; index<num_blocks; index++)
	{
		relation_pointer->getBlock(index, index_memblock_end);
		vector<Tuple> t1;
		t1 = block_pointer->getTuples();
		myTuples.insert(myTuples.end(), t1.begin(), t1.end());
	}
    block_pointer->clear(); //clear the block
	return myTuples;
}

vector<Tuple> TScan::getUniqueTuples(string table_name, int index_memblock_end)
{
	vector<Tuple> tuples_displayed;
	vector<Tuple> tuples_buffered;
	Relation *relation_pointer = schema_manager.getRelation(table_name);
	Block* block_pointer;
	if(relation_pointer == NULL)
		throw string("Given Relation ").append(table_name).append(" doesn't exist");
	int num_blocks = relation_pointer->getNumOfBlocks();
	if(num_blocks > NUM_OF_BLOCKS_IN_MEMORY - 1)// tablesize greater than memory
	{
		vector<Tuple> vec_tup;
		Relation_Sublist sub_list_rel(table_name, relation_pointer->getSchema().getFieldNames());

		Tuple tuple = sub_list_rel.GetDistinctTuples();
		while(!tuple.isNull())
		{
			vec_tup.push_back(tuple);
			tuple = sub_list_rel.GetDistinctTuples();
		}
		return vec_tup;
		// continue with a 2 pass distinct algorithm
	}

	//1-pass Algorithm
	block_pointer = mem.getBlock(index_memblock_end);
	if(block_pointer == NULL)
		throw string("Error!!!");
	vector<Tuple>::iterator iter;
	for (int index = 0; index<num_blocks; index++) //For all the blocks of Relation
	{
		relation_pointer->getBlock(index, index_memblock_end);
		vector<Tuple> t;
		t = block_pointer->getTuples();
		for(iter = t.begin(); iter!=t.end(); iter++)
		{
			if(search_tuple(tuples_buffered, *iter) == -1)
			{
				tuples_buffered.push_back(*iter);
				tuples_displayed.push_back(*iter);
			}
		}
	}
    block_pointer->clear(); //clear the block
	return tuples_displayed;
}


vector<Tuple> TScan::getTuples(bool unique, string table_name, int index_memblock_end, vector<MyColumn> order_by)
{
	vector<Tuple> tup;
	if(unique)
	{
		tup = getUniqueTuples(table_name, index_memblock_end);
	}
	else
	{
		tup = getTuples(table_name, index_memblock_end);
	}

	// cout << "In TScan::getTuples" << endl;
	// cout << order_by.size() << endl;
	if(order_by.size() != 0)
	{
		tup = Ordering_Relation(tup, table_name, order_by);
	}
	return tup;
}

vector<Tuple> TScan::Ordering_Relation(vector<Tuple> tup, string table_name, vector<MyColumn> order_by)
{
		// cout << "In TScan::Ordering_Relation, Here for order by" << endl;
		if(tup.size()== 0) return tup;

		vector<string> column;
		vector<MyColumn>::iterator myiterator;
		for(myiterator = order_by.begin(); myiterator!=order_by.end(); myiterator++)
			column.push_back((*myiterator).colName);
		int memory_blocks = tup.size()/tup[0].getTuplesPerBlock();
		if(memory_blocks<NUM_OF_BLOCKS_IN_MEMORY)
		{
			sort(tup.begin(), tup.end(), Tuple_Comparison(column));
		}
		else
		{
			Schema schema_obj = schema_manager.getSchema(table_name);
			string tempName = TableProcessing::rand_table();
			Relation* relation_pointer = schema_manager.createRelation(tempName, schema_obj);
			SaveTupleTodisk(relation_pointer, tup, 0);
			Relation_Sublist rel_sublist(tempName, column);
			tup.clear();
			Tuple tuple = rel_sublist.GetOrderedTuples();
			while(!tuple.isNull())
			{
				tup.push_back(tuple);
				// cout<<tuple<<endl;
				tuple = rel_sublist.GetOrderedTuples();
			}
		}
		return tup;
}

vector<Tuple> TScan::getTuples(ProjectOP project_op, SelectOP &select_op, string table_name, int index_memblock_end, string temp_relation)
{
	vector<Tuple> myTuples;
	Relation *relation_pointer = schema_manager.getRelation(table_name);
	if(relation_pointer == NULL)
		throw string("Relation ").append(table_name).append("doesn'tup exist");
	Schema schema_new = GetNewSchema(project_op, relation_pointer);
	Relation* newRelation_pointer = schema_manager.createRelation(temp_relation, schema_new);
	Block* block_pointer;
	int num_blocks = relation_pointer->getNumOfBlocks();
	block_pointer = mem.getBlock(index_memblock_end);
	if(block_pointer == NULL)
		throw string("Error in scanning!!!");
	for (int index = 0; index<num_blocks; index++)
	{
		relation_pointer->getBlock(index, index_memblock_end);
		vector<Tuple> tup;
		tup = block_pointer->getTuples();
		vector<Tuple>::iterator tupleiterator;
		for(tupleiterator = tup.begin(); tupleiterator != tup.end(); tupleiterator++)
		{
			Tuple tup = *tupleiterator;
			if(select_op.Issearch_cond())
			{
				Mapping_Tuple(tup, table_name);
				bool isValidTuple = select_op.search_cond_compute();
				if(isValidTuple)
				{
					Tuple tupletype_new = newRelation_pointer->createTuple();
					InitializingNewTuple(tupletype_new, tup);
					myTuples.push_back(tupletype_new);
				}
			}
			else
			{
				Tuple tupletype_new = newRelation_pointer->createTuple();
				InitializingNewTuple(tupletype_new, tup);
				myTuples.push_back(tupletype_new);
			}
		}
	}
    block_pointer->clear(); //clear the block
	return myTuples;
}

vector<Tuple> TScan::GetUniqueTuplesCorrected(bool isUnique, ProjectOP project_op, SelectOP& select_op, string table_name, int index_memblock_end, string temp_relation, vector<MyColumn> order_by)
{
		string table_name_current = table_name;
		vector<Tuple> tuples_displayed;
		vector<Tuple> tuples_buffered;
		Relation *relation_pointer = schema_manager.getRelation(table_name);
		Schema Original_Schema = relation_pointer->getSchema();
		if(relation_pointer == NULL)
			throw string("Relation ").append(table_name).append("doesn'tup exist");
		Block* block_pointer;
		int num_blocks = relation_pointer->getNumOfBlocks();
		block_pointer = mem.getBlock(index_memblock_end);
		if(block_pointer == NULL)
			throw string("Error in fetching unique tuple: GetUniqueTuplesCorrected()!!!");
		for (int index = 0; index<num_blocks; index++)
		{
			relation_pointer->getBlock(index, index_memblock_end);
			vector<Tuple> tup;
			tup = block_pointer->getTuples();
			vector<Tuple>::iterator tupleiterator;
			for(tupleiterator = tup.begin(); tupleiterator != tup.end(); tupleiterator++)
			{
				Tuple tup = *tupleiterator;
				if(select_op.Issearch_cond())
				{
					Mapping_Tuple(tup, table_name);
					bool isValidTuple = select_op.search_cond_compute();
					if(isValidTuple)
					{
						tuples_displayed.push_back(tup);
					}
				}
				else
				{
					tuples_displayed.push_back(tup);
				}
			}
		}
		if(isUnique)
		{
				// cout << "In TScan::GetUniqueTuplesCorrected, isUnique is True" << endl;
				int memory_block_size = tuples_displayed.size();

				if(memory_block_size < NUM_OF_BLOCKS_IN_MEMORY)
				{
					tuples_buffered.clear();
					vector<Tuple> bufferedTuple;
					vector<Tuple>::iterator myiterator1;
					for(myiterator1 = tuples_displayed.begin(); myiterator1 != tuples_displayed.end(); myiterator1++)
					{
						if(search_tuple(bufferedTuple, *myiterator1, project_op.fetchcolumnString()) == -1)
						{
							bufferedTuple.push_back(*myiterator1);
							tuples_buffered.push_back(*myiterator1);
						}
					}
					tuples_displayed.clear();
					tuples_displayed = tuples_buffered;
				}
				else
				{
					string table = TableProcessing::rand_table();
					table_name_current = table;
					Relation* relation = schema_manager.createRelation(table, Original_Schema);
					SaveTupleTodisk(relation, tuples_displayed, 0);
					Relation_Sublist sellist(table, project_op.fetchcolumnString());
					tuples_displayed.clear();
					Tuple tup = sellist.GetDistinctTuples();
					while(!tup.isNull())
					{
						tuples_displayed.push_back(tup);
						tup = sellist.GetDistinctTuples();
					}
				}
		}

		if(order_by.size() != 0)
		{
				int memory_block_size = tuples_displayed.size()/tuples_displayed[0].getTuplesPerBlock();
				vector<string> orderby;
				vector<MyColumn>::iterator myiterator;
				for(myiterator = order_by.begin(); myiterator!=order_by.end(); myiterator++)
					orderby.push_back((*myiterator).colName);
				if(memory_block_size < NUM_OF_BLOCKS_IN_MEMORY)
				{
					sort(tuples_displayed.begin(), tuples_displayed.end(), Tuple_Comparison(orderby));
				}
				else
				{
					string tblname = TableProcessing::rand_table();
					table_name_current = tblname;
					Relation* rel = schema_manager.createRelation(tblname, Original_Schema);
					SaveTupleTodisk(rel, tuples_displayed, 0);
					Relation_Sublist sellist(tblname, orderby);
					tuples_displayed.clear();
					Tuple tup = sellist.GetOrderedTuples();
					while(!tup.isNull())
					{
						tuples_displayed.push_back(tup);
						tup = sellist.GetOrderedTuples();
					}
				}
		}
		//perform the project_op
			int memory_block_size = tuples_displayed.size()/tuples_displayed[0].getTuplesPerBlock();
			Schema schema_new = GetNewSchema(project_op, relation_pointer);
			//string tbName = TableProcessing::rand_table();
			Relation* newRelation_pointer = schema_manager.createRelation(temp_relation, schema_new);
			vector<Tuple>::iterator myiterator;
			tuples_buffered.clear();
			for(myiterator = tuples_displayed.begin(); myiterator != tuples_displayed.end(); myiterator++)
			{
				Tuple tup = *myiterator;
				Tuple tupletype_new = newRelation_pointer->createTuple();
				InitializingNewTuple(tupletype_new, tup);
				tuples_buffered.push_back(tupletype_new);
			}

		return tuples_buffered;
}

vector<Tuple> TScan::getTuples(bool isUnique, ProjectOP project_op, SelectOP& select_op, string table_name, int index_memblock_end, string temp_relation, vector<MyColumn> order_by)
{
	return GetUniqueTuplesCorrected(isUnique, project_op, select_op, table_name, 0, temp_relation, order_by);
}

vector<Tuple> TScan::GetSelectTuples(string table_name, int index_memblock_end, SelectOP& select_op)
{

	vector<Tuple> tuples_selected;
	vector<MyColumn> order_by;
	vector<Tuple> myTuples = getTuples(false, table_name, index_memblock_end, order_by);
	// cout << "Executed getTuples" << endl;
	vector<Tuple>::iterator myiterator;
	for(myiterator = myTuples.begin(); myiterator!=myTuples.end(); myiterator++)
	{
		Tuple tup = *myiterator;
		Mapping_Tuple(tup, table_name);
		// cout << "Executed Mapping_Tuple" << endl;
		bool isValidTuple = select_op.search_cond_compute();
		// cout << "Executed select_op.search_cond_compute()" << endl;
		if(isValidTuple)
			tuples_selected.push_back(tup);
	}
	// cout << "Leaving TScan::GetSelectTuples" << endl;
	return tuples_selected;
}

vector<Tuple> TScan::GetSelectTuples(bool isUnique, string table_name, int index_memblock_end, SelectOP& select_op, vector<MyColumn> order_by)
{
	string tempTable;
	Schema original_schema = schema_manager.getSchema(table_name);
	vector<Tuple> output_tuple =  GetSelectTuples(table_name, index_memblock_end, select_op);
	int memory_size = 0;
	if(output_tuple.size()!=0)
		memory_size = output_tuple.size()/output_tuple[0].getTuplesPerBlock();
	if(isUnique)
	{
		if(memory_size > NUM_OF_BLOCKS_IN_MEMORY)
		{
			tempTable = TableProcessing::rand_table();
			Relation* rel = schema_manager.createRelation(tempTable, original_schema);
			SaveTupleTodisk(rel, output_tuple, 0);
			output_tuple.clear();
			output_tuple = getTuples(true, tempTable, 0, order_by);
		}
		else
		{
			vector<Tuple> buffer;
			vector<Tuple> out_buffer;
			vector<Tuple>::iterator myiterator;
			for(myiterator = output_tuple.begin(); myiterator!=output_tuple.end(); myiterator++)
			{
				if(search_tuple(buffer, *myiterator) == -1)
				{
					buffer.push_back(*myiterator);
					out_buffer.push_back(*myiterator);
				}
			}
			output_tuple.clear();
			output_tuple = out_buffer;
		}

	}
	if(order_by.size() > 0)
	{
		output_tuple = Ordering_Relation(output_tuple, table_name, order_by);
	}
	return output_tuple;
}

Tuple JoinTuples(Tuple tuple_left, Tuple tuple_right, Relation*& new_relationPointer, Relation*& left_relationPointer, Relation*& right_relationPointer)
{
	Schema left_schema = tuple_left.getSchema();
	Schema right_schema = tuple_right.getSchema();

	bool IsLeft = false;
	bool IsRight = false;

	vector<string> left_field_names = left_schema.getFieldNames();
	vector<string> right_field_names = right_schema.getFieldNames();

	if(left_field_names[0].find('.') != string::npos)
		IsLeft = true;
	if(right_field_names[0].find('.') != string::npos)
		IsRight = true;

	Tuple new_tuple = new_relationPointer->createTuple();

	vector<string>::iterator myiterator;
	for(myiterator = left_field_names.begin(); myiterator!= left_field_names.end(); myiterator++)
	{
		string column_name;
		if(IsLeft)
			column_name = *myiterator;
		else
			column_name = string(left_relationPointer->getRelationName()).append(".").append(*myiterator);

		if(left_schema.getFieldType(*myiterator) == STR20)
		{
			string value = *(tuple_left.getField(*myiterator).str);
			new_tuple.setField(column_name, value);
		}
		else
		{
			int value = tuple_left.getField(*myiterator).integer;
			new_tuple.setField(column_name, value);
		}
	}

	for(myiterator = right_field_names.begin(); myiterator!= right_field_names.end(); myiterator++)
	{
		string column_name;
		if(IsRight)
			column_name = *myiterator;
		else
			column_name = string(right_relationPointer->getRelationName()).append(".").append(*myiterator);
		if(right_schema.getFieldType(*myiterator) == STR20)
		{
			string value = *(tuple_right.getField(*myiterator).str);
			new_tuple.setField(column_name, value);
		}
		else
		{
			int value = tuple_right.getField(*myiterator).integer;
			new_tuple.setField(column_name, value);
		}
	}
	return new_tuple;
}

vector<Tuple> TScan::GetProd(string tbl1, string tbl2, string tempTable, bool saveToDB)
{
	//cout << "In TScan::GetProd" << endl;
	vector<Tuple> join_tuples;
	Relation* relation_pointer_s = schema_manager.getRelation(tbl1);
	Relation* relation_pointer_r = schema_manager.getRelation(tbl2);
	Relation *relation_bulk,*relation_singleFetch;
	bool IsOnePass = false;
	int size_relS = relation_pointer_s->getNumOfBlocks();
	int size_relR = relation_pointer_r->getNumOfBlocks();

	int small_size;
	int big_size;
	if(size_relR<size_relS)
	{
		relation_bulk = relation_pointer_r;
		relation_singleFetch = relation_pointer_s;
		small_size = size_relR;
		big_size = size_relS;
	}
	else
	{
		relation_bulk = relation_pointer_s;
		relation_singleFetch = relation_pointer_r;
		small_size = size_relS;
		big_size = size_relR;
	}
	Schema schema_new = GetNewSchema(relation_bulk, relation_singleFetch);
	Relation* new_relationPointer = schema_manager.createRelation(tempTable, schema_new);
	bool lastPass = false;
	int start_Index = 0;
	while(1)
	{
		if(lastPass) break;

		int block_toGet;

		if(small_size < NUM_OF_BLOCKS_IN_MEMORY - 1 )
		{
			lastPass = true;
			block_toGet = small_size;
		}
		else
		   block_toGet = NUM_OF_BLOCKS_IN_MEMORY - 1;

		relation_bulk->getBlocks(start_Index, 1, block_toGet); //reading blocks of smaller relation
		vector<Tuple> smaller_rel_tuples = mem.getTuples(1, block_toGet); // get all the tuples in a vector
		vector<Tuple>::iterator small_myiterator;
		int blocks_to_read = big_size; // Num. of blocks in bigger relation
		int index = 0; // index for bigger relation
		while(blocks_to_read > 0) //while all blocks for the relation are not read
		{
			relation_singleFetch->getBlock(index, 0); //get a single block from the bigger relation in index zero.
			vector<Tuple> bigger_rel_tuples = mem.getTuples(0, 1);
			vector<Tuple>::iterator big_myiterator;
			for(big_myiterator = bigger_rel_tuples.begin(); big_myiterator!= bigger_rel_tuples.end(); big_myiterator++)
			{
				for(small_myiterator = smaller_rel_tuples.begin(); small_myiterator!= smaller_rel_tuples.end(); small_myiterator++)
				{

					Tuple tup = JoinTuples(*small_myiterator, *big_myiterator, new_relationPointer, relation_bulk, relation_singleFetch);
					join_tuples.push_back(tup);
				}
			}
			blocks_to_read--;
			index++;
		}

		small_size = small_size - block_toGet;
		start_Index = start_Index + block_toGet;
	}

	if(saveToDB)
	{
		SaveTupleTodisk(new_relationPointer, join_tuples, 0); //use memory index 0 to save the tuples
	}
	return join_tuples;
}

void SaveTupleTodisk(Relation*& relation_pointer, vector<Tuple> tuples, int memory_index)
{
	if(tuples.size() == 0) return;
	bool success = true;
	Block* block_pointer = mem.getBlock(memory_index);
	block_pointer->clear();
	Schema schema = relation_pointer->getSchema();
	int noOfTuplesPerBlock = schema.getTuplesPerBlock();
	int rel_blockindex = 0; //position from where to insert the block in relation

	vector<Tuple>::iterator myiterator;
	for(myiterator = tuples.begin(); myiterator!=tuples.end(); myiterator++)
	{
		block_pointer->appendTuple(*myiterator);
		if(block_pointer->isFull())//flush memory if the block is full
		{
			relation_pointer->setBlock(rel_blockindex, memory_index);
			block_pointer->clear();
			rel_blockindex++;
		}
	}
	if(!block_pointer->isEmpty())
	{
		relation_pointer->setBlock(rel_blockindex, memory_index);
	}
}

Schema GetNewSchema(ProjectOP project_op, JOINOP join_op, Relation*& relation_pointer_old)
{
	vector<string> mytables = join_op.GetFromList();
	vector<MyColumn> column = project_op.fetchcolumn();

	vector<Schema> myschema;

	vector<string> new_column_names;
	vector<FIELD_TYPE> new_field_types;

	vector<MyColumn>::iterator myiterator;
	Schema schema = relation_pointer_old->getSchema();

	vector<string>::iterator table_iterator;
	for(table_iterator = mytables.begin(); table_iterator != mytables.end(); table_iterator++)
	{
		myschema.push_back(schema_manager.getSchema(*table_iterator));
	}
	vector<Schema>::iterator schema_iterator;
	vector<MyColumn>::iterator start_iterator = column.begin();
	vector<MyColumn>::iterator end_iterator = column.end();

	for(myiterator = start_iterator; myiterator != end_iterator; myiterator++)
	{
		string column_name = myiterator->colName;
		string table_name = myiterator->tblName;
		if(table_name == NOT_A_TABLE) //if table name is not give
		{
			int num_tables = 0;
			table_iterator = mytables.begin();
			schema_iterator = myschema.begin();
			while(table_iterator != mytables.end())
			{
				if((*schema_iterator).fieldNameExists(column_name))
				{
					table_name = *table_iterator;
					num_tables++;
				}
				table_iterator++;
				schema_iterator++;
			}
			if(num_tables != 1)
				throw string("Error! Zero or More than one schema matches for column").append(string(column_name));
		}
		string col_name;
		col_name = table_name;
		col_name.append(".");
		col_name.append(column_name);
		FIELD_TYPE field_type = schema.getFieldType(col_name);
		if(field_type!= INT && field_type != STR20)
			throw string("Error!!!");

		new_column_names.push_back(col_name);
		new_field_types.push_back(field_type);
	}
	return Schema(new_column_names, new_field_types);
}

SendRelation TScan::ExecuteProjectOP(ProjectOP project_op, JOINOP join_op,
					SendRelation sendRelation, bool saved)
{
	SendRelation return_relation;
	vector<Tuple> tuples = sendRelation.tuples;
	Relation *relation_pointer_old = schema_manager.getRelation(sendRelation.relation_name);
	string tempTable = TableProcessing::rand_table();
	return_relation.relation_name = tempTable;
	return_relation.marked_For_Delete = true;
	Schema schema_new = GetNewSchema(project_op, join_op, relation_pointer_old);
	Relation* newRelation_pointer = schema_manager.createRelation(tempTable, schema_new);

	vector<Tuple>::iterator tuple_iterator;

	for(tuple_iterator = tuples.begin(); tuple_iterator != tuples.end(); tuple_iterator++)
	{
		Tuple tup = newRelation_pointer->createTuple();
		InitializingNewTuple(tup, *tuple_iterator);
		return_relation.tuples.push_back(tup);
	}

	return return_relation;
}

string TScan::Fetch_Table_Name(string column_name)
{
	if(VariablesFromTbl.size() == 0)
		throw string("No table exists to verify the mycolumns against");

	vector<string>::iterator table_iterator;

	vector<Schema> myschema;
	int count = 0;
	string table_name;
	for(table_iterator = VariablesFromTbl.begin(); table_iterator != VariablesFromTbl.end(); table_iterator++)
	{
		Schema schema_t = schema_manager.getSchema(*table_iterator);
		if(schema_t.fieldNameExists(column_name))// not sure recheck
		{
			table_name = *table_iterator;
			count++;
		}
	}
	if(count == 1)
		return table_name;
	else
		throw string("Multiple or No table matches to column name :").append(string(column_name));

}

vector<Tuple> TScan::ExecuteSelectOP(SelectOP& select_op, SendRelation sendRelation)
{
	vector<Tuple> tuples_selected;
	vector<Tuple> tuples = sendRelation.tuples;

	vector<Tuple>::iterator tuple_iterator;

	for(tuple_iterator = tuples.begin(); tuple_iterator != tuples.end(); tuple_iterator++)
	{
		Tuple tup = *tuple_iterator;
		Mapping_Tuple(tup);
		bool isValidTuple = select_op.search_cond_compute();
		if(isValidTuple)
			tuples_selected.push_back(tup);
	}
	return tuples_selected;
}

Relation_Sublist::Relation_Sublist(string relationname, vector<string> column)
{
	relation_name = relationname;
	num_sublists = 0;
	queVector_mem = false;
	relation_pointer = schema_manager.getRelation(relationname);
	relation_schema = relation_pointer->getSchema();
	if(column.size() == 0)
		mycolumns = relation_schema.getFieldNames();
	else
		mycolumns = column;
	num_blocks = relation_pointer->getNumOfBlocks();

	int blocks_toread_left = num_blocks;
	int index_toread = 0;
	while(blocks_toread_left > 0)
	{
		int main_memory_index = 0;
		if(blocks_toread_left > NUM_OF_BLOCKS_IN_MEMORY)
		{
			relation_pointer->getBlocks(index_toread, 0, NUM_OF_BLOCKS_IN_MEMORY);
			InMemorySort(0, NUM_OF_BLOCKS_IN_MEMORY, mycolumns);
			relation_pointer->setBlocks(index_toread, 0, NUM_OF_BLOCKS_IN_MEMORY);
      index_of_sublists.push_back(index_toread);
			num_sublists++;
			index_toread = index_toread + NUM_OF_BLOCKS_IN_MEMORY;
			blocks_toread_left = blocks_toread_left - NUM_OF_BLOCKS_IN_MEMORY;
		}
		else
		{
			relation_pointer->getBlocks(index_toread, 0, blocks_toread_left);
			InMemorySort(0, blocks_toread_left, mycolumns);
			relation_pointer->setBlocks(index_toread, 0, blocks_toread_left);
			index_of_sublists.push_back(index_toread);
			num_sublists++;
			index_toread = index_toread + blocks_toread_left;
			num_lastlist_block = blocks_toread_left;
			blocks_toread_left = 0; //terminating condition
		}
	}
}

Tuple Relation_Sublist::GetDistinctTuples()
{
	if(queVector_mem== false)
	{
		queueVector_mem = vector<deque<Tuple>>(num_sublists);
		//queue of tuple for each sublist in MM
		for(int index=0; index<num_sublists; index++)
		{
			map_block_relIndex[index] = 0;
			int index1 = index_of_sublists[index] + map_block_relIndex[index];
			relation_pointer->getBlock(index1, index);
			map_block_relIndex[index] = map_block_relIndex[index] + 1;
			Block* block_pointer = mem.getBlock(index);
			vector<Tuple> tuples = block_pointer->getTuples();
			vector<Tuple>::iterator myiterator;
			for(myiterator = tuples.begin(); myiterator != tuples.end(); myiterator++)
				queueVector_mem[index].push_back(*myiterator);
		}
		queVector_mem = true;
	}

	Tuple tuple_null = relation_pointer->createTuple();
	tuple_null.null();
	vector<Tuple> tuple_for_comparison;
	for(int index=0; index<num_sublists; index++)
	{
		tuple_for_comparison.push_back(tuple_null);
	}

	for(int index=0; index<num_sublists; index++)
	{
		bool Is_valid_tuple = true;
		if(queueVector_mem[index].empty())
		{
			if(index == num_sublists - 1) //if index is the last sublist
			{
				if(map_block_relIndex[index] == num_lastlist_block)
					tuple_for_comparison[index].null();
				Is_valid_tuple = false;
			}
			else if(map_block_relIndex[index] == NUM_OF_BLOCKS_IN_MEMORY)
			{
				tuple_for_comparison[index].null();
				Is_valid_tuple = false;
			}
			else
			{
				int index_1 = index_of_sublists[index] + map_block_relIndex[index];
				map_block_relIndex[index] = map_block_relIndex[index] + 1;
				relation_pointer->getBlock(index_1, index);
				Block* block_pointer = mem.getBlock(index);
				vector<Tuple> tuples = block_pointer->getTuples();
				vector<Tuple>::iterator myiterator;
				for(myiterator = tuples.begin(); myiterator != tuples.end(); myiterator++)
					queueVector_mem[index].push_back(*myiterator);
				Is_valid_tuple = true;
			}
		}

		if(Is_valid_tuple)
		{
			tuple_for_comparison[index] = queueVector_mem[index].front();
		}
	}
	Tuple tup = search_min_tuple(tuple_for_comparison, mycolumns);
	if(!tup.isNull())
	{
		for(int index=0; index<num_sublists; index++)
		{
			// cout << "Index: " << index << endl;
			 Remove_All(index, tup);
		}
	}
	return tup;
}

bool Relation_Sublist::GetQueue(int index)
{
	if(index == num_sublists - 1) //index is last sublist
	{
		if(map_block_relIndex[index] == num_lastlist_block)
	    	return false;
	}
	else if(map_block_relIndex[index] == NUM_OF_BLOCKS_IN_MEMORY)
	{
		return false;
	}
	int i = index_of_sublists[index] + map_block_relIndex[index];
	map_block_relIndex[index] = map_block_relIndex[index] + 1;
	relation_pointer->getBlock(i, index);
	Block* block_pointer = mem.getBlock(index);
	vector<Tuple> tuples = block_pointer->getTuples();
	vector<Tuple>::iterator myiterator;
	for(myiterator = tuples.begin(); myiterator != tuples.end(); myiterator++)
		queueVector_mem[index].push_back(*myiterator);
}
void Relation_Sublist::Remove_All(int index, Tuple tup)
{
   for(;;)
   {
	   if(queueVector_mem[index].empty())
	   {
		   bool extracted = GetQueue(index);
		   if(extracted == false)
			   return;
	   }
	   while(!queueVector_mem[index].empty())
	   {
		   Tuple tup2 = queueVector_mem[index].front();
		   if(IsEqualTuples(tup, tup2, mycolumns))
		   {
			   queueVector_mem[index].pop_front();
		   }
		   else
			   return;
	   }
   }
   cout<<"Fetching infinitely"<<endl;
}

void Relation_Sublist::Remove(int index)
{
	 queueVector_mem[index].pop_front();
	 if(queueVector_mem[index].empty())
	 {
		 bool extracted = GetQueue(index);
		 if(extracted == false)
			  return;
	 }
}

Tuple Relation_Sublist::GetOrderedTuples()
{
	if(queVector_mem== false)
	{
		queueVector_mem = vector<deque<Tuple>>(num_sublists);
		for(int index=0; index<num_sublists; index++)
		{
			map_block_relIndex[index] = 0;
			int index_2 = index_of_sublists[index] + map_block_relIndex[index];
			// cout << "Here\n" << endl;
			relation_pointer->getBlock(index_2, index);
			map_block_relIndex[index] = map_block_relIndex[index] + 1;
			Block* block_pointer = mem.getBlock(index);
			vector<Tuple> tuples = block_pointer->getTuples();
			vector<Tuple>::iterator myiterator;
			for(myiterator = tuples.begin(); myiterator != tuples.end(); myiterator++)
				queueVector_mem[index].push_back(*myiterator);
		}
		queVector_mem = true;
  }
	Tuple tuple_null = relation_pointer->createTuple();
	tuple_null.null();
	vector<Tuple> tuple_for_comparison;
	for(int index=0; index<num_sublists; index++)
	{
		tuple_for_comparison.push_back(tuple_null);
	}

	for(int index=0; index<num_sublists; index++)
	{
		bool Is_valid_tuple = true;
		if(queueVector_mem[index].empty())
		{

			if(index == num_sublists - 1) //if index is the lat sublist
			{
				if(map_block_relIndex[index] == num_lastlist_block)
					tuple_for_comparison[index].null();
				Is_valid_tuple = false;
			}
			else if(map_block_relIndex[index] == NUM_OF_BLOCKS_IN_MEMORY)
			{
				tuple_for_comparison[index].null();
				Is_valid_tuple = false;
			}
			else
			{
				int index_2 = index_of_sublists[index] + map_block_relIndex[index];
				map_block_relIndex[index] = map_block_relIndex[index] + 1;
				relation_pointer->getBlock(index_2, index);
				Block* block_pointer = mem.getBlock(index);
				vector<Tuple> tuples = block_pointer->getTuples();
				vector<Tuple>::iterator myiterator;
				for(myiterator = tuples.begin(); myiterator != tuples.end(); myiterator++)
					queueVector_mem[index].push_back(*myiterator);
				Is_valid_tuple = true;
			}
		}
		if(Is_valid_tuple)
		{
			tuple_for_comparison[index] = queueVector_mem[index].front();
		}
	}

	Tuple tup = search_min_tuple(tuple_for_comparison, mycolumns);
	if(!tup.isNull())
	{
		int pos = search_tuple(tuple_for_comparison, tup, mycolumns);
		if(pos != -1)
			Remove(pos);
	}
	return tup;
}
vector<Tuple> TScan::GetUniqueOrderedTuples(vector<Tuple> Tuples, bool isUnique, ProjectOP project_op, string table_name, int index_memblock_end, vector<MyColumn> order_by)
{

		string table_name_current = table_name;
		vector<Tuple> tuples_displayed = Tuples;
		vector<Tuple> tuples_buffered;
		Relation *relation_pointer = schema_manager.getRelation(table_name);
		Schema original_schema = relation_pointer->getSchema();
		if(relation_pointer == NULL)
			throw string("Relation ").append(table_name).append("doesn't exist");
		bool select_star = false;
		vector<string>fieldNames;
		if(project_op.fetchcolumn().front().colName == "*")
		{

			select_star = true;
		}
		if(select_star)
		{
			fieldNames = original_schema.getFieldNames();
		}
		else
		{
			fieldNames = project_op.fetchcolumnStringtable();
		}
		vector<string> column_list;
		int num_blocks = relation_pointer->getNumOfBlocks();
		if(isUnique)
		{
				int memory_block_size = tuples_displayed.size()/tuples_displayed[0].getTuplesPerBlock();
				if(memory_block_size < NUM_OF_BLOCKS_IN_MEMORY) //
				{
					tuples_buffered.clear();
					vector<Tuple> bufferedTuple;
					vector<Tuple>::iterator myiterator1;
					for(myiterator1 = tuples_displayed.begin(); myiterator1 != tuples_displayed.end(); myiterator1++)
					{
						if(select_star)
						{
							if(search_tuple(bufferedTuple, *myiterator1) == -1)
							{
								bufferedTuple.push_back(*myiterator1);
								tuples_buffered.push_back(*myiterator1);
							}
						}
						else
						{
							if(search_tuple(bufferedTuple, *myiterator1, project_op.fetchcolumnStringtable()) == -1)
							{
								bufferedTuple.push_back(*myiterator1);
								tuples_buffered.push_back(*myiterator1);
							}
						}
					}
					tuples_displayed.clear();
					tuples_displayed = tuples_buffered;
				}
				else
				{
					string Name = TableProcessing::rand_table();
					table_name_current = Name;
					Relation* rel = schema_manager.createRelation(Name, original_schema);
					SaveTupleTodisk(rel, tuples_displayed, 0);
					Relation_Sublist sublist(Name, fieldNames);
					tuples_displayed.clear();
					Tuple tup = sublist. GetDistinctTuples();
					while(!tup.isNull())
					{
						tuples_displayed.push_back(tup);
						tup = sublist.GetDistinctTuples();
					}
				}
		}
		if(order_by.size() != 0)
		{
			int memory_block_size = tuples_displayed.size()/tuples_displayed[0].getTuplesPerBlock();
				vector<string> orderby;
				vector<MyColumn>::iterator myiterator;
				for(myiterator = order_by.begin(); myiterator!=order_by.end(); myiterator++)
				{
					string tble_name = (*myiterator).tblName;
					if((*myiterator).tblName == NOT_A_TABLE)
						tble_name = TScan::Fetch_Table_Name((*myiterator).colName);
					orderby.push_back(string(tble_name).append(".").append((*myiterator).colName));
				}
				if(memory_block_size < NUM_OF_BLOCKS_IN_MEMORY)
				{
					sort(tuples_displayed.begin(), tuples_displayed.end(), Tuple_Comparison(orderby));
				}
				else
				{
					string Name = TableProcessing::rand_table();
					table_name_current = Name;
					Relation* rel = schema_manager.createRelation(Name, original_schema);
					SaveTupleTodisk(rel, tuples_displayed, 0);
					Relation_Sublist sublist(Name, orderby);
					tuples_displayed.clear();
					Tuple tup = sublist.GetOrderedTuples();
					int index = 0;
					while(!tup.isNull())
					{
						tuples_displayed.push_back(tup);
						tup = sublist.GetOrderedTuples();

					}
				}
		}
		return tuples_displayed;
}
