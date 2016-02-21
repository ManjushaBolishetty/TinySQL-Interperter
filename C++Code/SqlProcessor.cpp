#include "SqlProcessor.h"
#include "SendRelation.h"

SqlProcessor::SqlProcessor()
{
	// cout<<"SqlProcessor Initialized"<<endl;
}

bool SqlProcessor::CreateTbl(string table_name, vector<string> attribute_namelist, vector<enum FIELD_TYPE> datatype_list, ofstream& qout_file, int out_option)
{
	Schema schema(attribute_namelist,datatype_list);
	Relation* relation_pointer=schema_manager.createRelation(table_name,schema);
	if(relation_pointer == NULL)
	{
		if(out_option == 1) {
			cout<<"Table Creation FAILED!!!!"<<endl;
		}
		else if(out_option == 2) {
			qout_file<<"Table Creation FAILED!!!!"<<endl;
		}
		else {}
		return false;
	}
	if(out_option == 1) {
		cout<<"Table: "<<table_name<<" created sucessfully"<<endl;
	}
	else if(out_option == 2) {
		qout_file<<"Table: "<<table_name<<" created sucessfully"<<endl;
	}
	else {}
	return true;
}

bool SqlProcessor::TupleInitialization(Tuple& tuple, vector<string> attribute_list, vector<string> value_list)
{
	bool pass = true;
	vector<string>::iterator attribute_iterator;
	vector<string>::iterator value_iterator;
	Schema scheme_of_tuple = tuple.getSchema();
	int index = 0;
	for(attribute_iterator = attribute_list.begin(), value_iterator = value_list.begin(); attribute_iterator != attribute_list.end(); attribute_iterator++, value_iterator++)
	{
		if(scheme_of_tuple.getFieldType(*attribute_iterator) == STR20)
		{
			string value = *value_iterator;
			if(*value_iterator ==  "NULL")
				value = "";
			pass = tuple.setField(*attribute_iterator, value);
		}
		else
		{
			int value;
			if(*value_iterator == "NULL")
			{
				pass = tuple.setField(*attribute_iterator, 0);
			}
			else
			{
				bool alldigit = true;// assume all are digits
				for(int index=0; index<(*value_iterator).length(); index++)
					if(!isdigit((*value_iterator).at(index))) // if not a digit then alldigit is set to false.
						alldigit = false;

				if(alldigit)
				{
					value = atoi((*value_iterator).c_str());//string to integer conversion
					pass = tuple.setField(*attribute_iterator, value);
				}
				else
					pass = tuple.setField(*attribute_iterator, *value_iterator);
			}

		}
		if(!pass)
			return pass;
		index++;
	}
	return pass;
}

bool SqlProcessor::InsertTbl(string table_name, vector<string> attribute_list, vector<string> value_list)
{//A tuple can have a maximum of 8 fields
	bool pass = true;
	//fetch the relation from schema - use tablename
	Relation* relation = schema_manager.getRelation(table_name);
	if(relation == NULL) return false;
	Tuple tuple = relation->createTuple(); //creating empty tuple in the relation n fetch reference to the tuple
	pass = TupleInitialization(tuple, attribute_list, value_list); // set fields in the tuple
	if(!pass) return pass;
	//next Setup a block in memory (usually the 0th block), Clear this memory block
	//and insert or append tuple to this block
	pass = Append_Tuple_toRel(relation, 0, tuple);
	return pass;
}

bool SqlProcessor::Insert_After_Select_Tbl(string table_name, vector<Tuple> tuple_list) {

	bool pass = true;
	Relation* relation = schema_manager.getRelation(table_name);
	if(relation == NULL) return false;
	vector<Tuple>::iterator iteration;
	for(iteration = tuple_list.begin(); iteration != tuple_list.end(); iteration++) {
		pass = Append_Tuple_toRel(relation, 0, *iteration);
		// cout<<"A record inserted in table: "<<table_name<<endl;
		if(!pass) {
			break;
		}
	}
	return pass;
}

bool SqlProcessor::Append_Tuple_toRel(Relation* relation_pointer, int mem_blockindex, Tuple& tuple)
{
	bool pass = true;
	Block* block_pointer;
	if (relation_pointer->getNumOfBlocks()==0)
	{
		block_pointer=mem.getBlock(mem_blockindex);
		block_pointer->clear();
		pass = block_pointer->appendTuple(tuple);
		if(!pass) return pass;
		pass = relation_pointer->setBlock(relation_pointer->getNumOfBlocks(),mem_blockindex);
		if(!pass) return pass;
	}
	else
	{
		pass = relation_pointer->getBlock(relation_pointer->getNumOfBlocks()-1,mem_blockindex);
		if(!pass) return pass;
		block_pointer=mem.getBlock(mem_blockindex);

		if (block_pointer->isFull())
		{
			block_pointer->clear();
			pass = block_pointer->appendTuple(tuple);
			pass = relation_pointer->setBlock(relation_pointer->getNumOfBlocks(),mem_blockindex); //write back to the relation
			if(!pass) return pass;
		}
		else
		{
			block_pointer->appendTuple(tuple);

			relation_pointer->setBlock(relation_pointer->getNumOfBlocks()-1,mem_blockindex); //write back to the relation
		}
	}
	return pass;
}

void Output(SendRelation myRelation, ofstream& qout_file, int out_option)
{
	if(out_option == 1) {
		cout << endl;
	}
	else if(out_option == 2) {
		qout_file << endl;
	}
	else {}
	string relation_name = myRelation.relation_name;
	Schema schema = schema_manager.getSchema(relation_name);
	vector<string> field_names = schema.getFieldNames();
	if(out_option == 1) {
		copy(field_names.begin(),field_names.end(),ostream_iterator<string,char>(cout,"\t"));
		cout << endl;
	}
	else if(out_option == 2) {
		copy(field_names.begin(),field_names.end(),ostream_iterator<string,char>(qout_file,"\t"));
		qout_file << endl;
	}
	else {}
	vector<Tuple>::iterator iteration;
	for(iteration = myRelation.tuples.begin(); iteration != myRelation.tuples.end(); iteration++) {
		if(out_option == 1) {
			iteration->printTuple(cout);
			cout << endl;
		}
		else if(out_option == 2) {
			iteration->printTuple(qout_file);
			qout_file << endl;
		}
		else {}
	}
	if(out_option == 1) {
		cout << endl;
	}
	else if(out_option == 2){
		qout_file << endl;
	}
	else {}
}

bool SqlProcessor::SelectTbl(LogicalPlan*& lp, ofstream& qout_file, int out_option)
{
	// cout << "Entering SqlProcessor::SelectTbl" << endl;
	SendRelation retnrel = lp->Execute();
	Output(retnrel, qout_file, out_option);
	if(retnrel.marked_For_Delete)
		schema_manager.deleteRelation(retnrel.relation_name);
	// cout << "Leaving SqlProcessor::SelectTbl" << endl;
	return true;
}

SendRelation SqlProcessor::InsertSelectTbl(LogicalPlan*& lp)
{
	SendRelation retnrel = lp->Execute();
	// Output(retnrel);
	if(retnrel.marked_For_Delete)
		schema_manager.deleteRelation(retnrel.relation_name);
	return retnrel;
}

bool SqlProcessor::DropTbl(string table_name, ofstream& qout_file, int out_option)
{
	Relation* relation = schema_manager.getRelation(table_name);
	if(relation == NULL)
		return false;
	int num_of_blocks = relation->getNumOfBlocks();
	if(num_of_blocks !=0)
	{
		//Delete all the blocks from the relation
		relation->deleteBlocks(0);
	}

	bool pass =  schema_manager.deleteRelation(table_name);
	if(pass) {
		if(out_option == 1) {
			cout<<"Dropped table successfully: "<<table_name<<endl;
		}
		else if(out_option == 2) {
			qout_file<<"Dropped table successfully: "<<table_name<<endl;
		}
		else {}
	}
	return pass;
}

bool SqlProcessor::DeleteTbl(string table_name, Search*& search_cond)
{
	int rindex = 0;// index for relation
	vector<int> changed_block_index;// block indexes that got affected
	vector<Tuple>::iterator myiterator;
	Relation *relation_pointer = schema_manager.getRelation(table_name);
	int block_count_Relation = relation_pointer->getNumOfBlocks();
	int tuple_count = relation_pointer->getNumOfTuples();
	if(block_count_Relation == 0) return true;

	if(search_cond == NULL)
	{
		if(tuple_count>0)
			relation_pointer->deleteBlocks(0);
		return true;
	}
	// cout << "In SqlProcessor::DeleteTbl, before block_pointer" << endl;
	Block* block_pointer = mem.getBlock(0);
	bool moved_to_Fsave = false;
	vector<Tuple> fSaved_rows;
	// cout << "In SqlProcessor::DeleteTbl, before loop" << endl;
	for(rindex = 0; rindex < block_count_Relation; rindex++)
	{
		// cout << "In SqlProcessor::DeleteTbl, rindex: " << rindex << endl;
		relation_pointer->getBlock(rindex, 0);
		vector<Tuple> tuples = block_pointer->getTuples();
		vector<Tuple> rowst_save;
		for(myiterator = tuples.begin(); myiterator!= tuples.end(); myiterator++)
		{
			Mapping_Tuple(*myiterator, table_name);
			if(!search_cond->Compute())
			{
				rowst_save.push_back(*myiterator);
			}
		}
		if(tuples.size() != rowst_save.size()) //Means some tuples got deleted becoz they have fullfilled search condition
		{
			changed_block_index.push_back(rindex);
			//As soon as get one block to edit start saving it to final saved buffer
			moved_to_Fsave = true;
		}
		if(moved_to_Fsave)// Move to final save
		{
			fSaved_rows.insert(fSaved_rows.end(), rowst_save.begin(), rowst_save.end());
      //finally saved rows
		}
	}
	// cout << fSaved_rows.size() << endl;
	// cout << "In SqlProcessor::DeleteTbl, after loop" << endl;

	if(fSaved_rows.size() == 0)
	{
		// cout << "In SqlProcessor::DeleteTbl, in if" << endl;
		if(tuple_count>0)
			relation_pointer->deleteBlocks(0);
		return true;
	}
	int start_block_index = changed_block_index.front();
	bool success = true;
	Block* blk_pointer = mem.getBlock(1);  // use any memory index 
	blk_pointer->clear();
	Schema schema = relation_pointer->getSchema();
	int rows_per_block = schema.getTuplesPerBlock();
	int relation_block_index = 0; //Where to insert the block in relation
	// cout << "In SqlProcessor::DeleteTbl, before loop 2" << endl;

	for(myiterator = fSaved_rows.begin(); myiterator!=fSaved_rows.end(); myiterator++)
	{
		blk_pointer->appendTuple(*myiterator);
		if(blk_pointer->isFull())//full flush it to memory if block pointer is full
		{
			relation_pointer->setBlock(start_block_index, 1);
			blk_pointer->clear();
			start_block_index++;
		}
	}
	// cout << "In SqlProcessor::DeleteTbl, after loop 2" << endl;

	//Flush out the last few tuples
	if(!blk_pointer->isEmpty())
	{
		relation_pointer->setBlock(start_block_index, 1);
	}

	relation_pointer->deleteBlocks(start_block_index);

	return success;
  // delete any remaining blocks in the relation
 //  	cout << "Leaving SqlProcessor::DeleteTbl" << endl;
}
