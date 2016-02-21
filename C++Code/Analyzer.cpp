#include "Analyzer.h"
ofstream logoutput;
//Output file stream - Log file
extern vector<string> VariablesFromTbl;
//externs tells the compiler the variable or object is declared and may be defined elsewhere.
/// objects
LogicalPlan *selLPQ;
Expressions* exp_Term = NULL;
Expressions* exp_Factor = NULL;
Expressions* exp_Expression = NULL;

Compare*   pred_Comparison = NULL;
Search* boolPriSearch_cond = NULL;
Search* boolFacSearch_cond = NULL;
Search* boolTerSearch_cond = NULL;
Search* search_cond = NULL;

string tblname = "";
string attrname = "";
enum FIELD_TYPE data_type;
string myval;
MyColumn mycol;


vector<enum FIELD_TYPE> list_DataType;
vector<string> list_AttrName;
vector<string> list_attr; ////used in insert statement only => insert-statement ::= INSERT INTO table-name(attribute-list) insert-tuples
// used in insert statement alone=> insert-statement ::= INSERT INTO table-name(attribute-list) insert-tuples
							   //value-list ::= value | value, value-list
vector<string> list_value;
vector<MyColumn> list_selsub;
vector<string> list_tbl;

vector<MyColumn> list_ordby;
bool isunique = false;

void Analyzer::OutputDiskIO(ofstream& qout_file, int out_option)
{
	//After each operation display the number of Disk I/O's
	if(out_option == 1) {
		cout<<"Number of Disk I/O: "<<disk.getDiskIOs()<<"   Elapsed time in milliseconds: "<<disk.getDiskTimer()<<endl;
		cout<<"**************************************"<<endl;
		cout<<endl;
	}
	else if(out_option == 2) {
		qout_file<<"Number of Disk I/O: "<<disk.getDiskIOs()<<"   Elapsed time in milliseconds: "<<disk.getDiskTimer()<<endl;
		qout_file<<"**************************************"<<endl;
		qout_file<<endl;
	}
	else {}
}

void Analyzer::RestartDiskIO()
{
  // Reset the disk I/O counter.
  // Every time before you do a SQL operation, reset the counter.
	disk.resetDiskIOs();
  // Every time before you do a SQL operation, reset the timmer.
	disk.resetDiskTimer();
}

void Analyzer::RemoveVariable()
//Clearing all the gloal variables and setting them to null.
{
    tblname = "";
    attrname = "";
    list_DataType.clear();
    list_AttrName.clear();
    list_attr.clear();
    myval="";
    list_value.clear();
    mycol.colName = ""; // from Processing.h
    mycol.tblName= ""; //from Processing.h
    list_selsub.clear();
    list_tbl.clear();
    isunique = false;
    list_ordby.clear();
    if(selLPQ != NULL)
		delete selLPQ;
	selLPQ = NULL;
    search_cond = NULL;
    exp_Term =NULL;
    exp_Factor = NULL;
    exp_Expression = NULL;
    pred_Comparison = NULL;
    boolPriSearch_cond = NULL;
    boolFacSearch_cond = NULL;
    boolTerSearch_cond = NULL;
    VariablesFromTbl.clear();
  }

Analyzer::Analyzer():lexer(), sqlprocessor()
//Instantiating the Parser.
{
	 cout<<"Parser Starting..."<<endl;
}

Analyzer::Analyzer(char* inputtext):lexer(inputtext)
{
	if(!logoutput.is_open())
	{
		logoutput.open("Database_Log.txt");
		if (!logoutput)
		{
  			cerr << "The file Database_Log.txt could not be opened" << endl;
  		}
  	}
  	else
  	{
  		logoutput<<"\n\n*******************************************"<<endl;
  		logoutput<<"***********************************************\n\n"<<endl;
  	}
}

void Analyzer::AnalyzeInput(ofstream& qout_file, int out_option)
{
    logoutput<<"Analyzing <Input>"<<endl;
    MyToken mytoken;
	bool not_insert = false;
	bool found_insert = false;
	bool found_insert_select = false;
	// int token_count = 0;
	char* statement_start;
	char* select_start;
	SendRelation select_results;
	char* sql_statement_start;
	// cout << "Printing Statement Start: " << statement_start[0] << endl;
	// cout << "First while loop done" << endl;
	// cout << found_insert_select << endl;
	// cout << "Printing reset Point: " << *lexer.getCurrentCharacter() << endl;

    while(mytoken.TokenType != EOF)
    {
        bool pass = true;
        mytoken = lexer.GetNextToken();

        switch(mytoken.TokenType)
        {
            case CREATE:
				// Printing SQL statement on screen or file
				if(out_option == 1) {
					cout << "CREATE";
				}
				else if(out_option == 2) {
					qout_file << "CREATE";
				}
				else {}
				sql_statement_start = lexer.getCurrentCharacter();
				if(*sql_statement_start == '\n') {
					sql_statement_start++;
				}
				while(*sql_statement_start != '\n') {
					if(out_option == 1) {
						cout << *sql_statement_start;
					}
					else if(out_option == 2) {
						qout_file << *sql_statement_start;
					}
					else {}
					sql_statement_start++;
				}
				if(out_option == 1) {
					cout << endl;
				}
				else if(out_option == 2) {
					qout_file << endl;
				}
				// ----------------------------------------
			    logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
			    AnalyzerTable_Create();
			    //cout<<"CREATE on table parsed successfully \n"<<endl;
			    RestartDiskIO();
			    pass = sqlprocessor.CreateTbl(tblname,list_AttrName,list_DataType,qout_file,out_option);
			    OutputDiskIO(qout_file, out_option);
			    RemoveVariable();
			    break;

			case DROP:
			// Printing SQL statement on screen or file
				if(out_option == 1) {
					cout << "DROP";
				}
				else if(out_option == 2) {
					qout_file << "DROP";
				}
				else {}
				sql_statement_start = lexer.getCurrentCharacter();
				if(*sql_statement_start == '\n') {
					sql_statement_start++;
				}
				while(*sql_statement_start != '\n') {
					if(out_option == 1) {
						cout << *sql_statement_start;
					}
					else if(out_option == 2) {
						qout_file << *sql_statement_start;
					}
					else {}
					sql_statement_start++;
				}
				if(out_option == 1) {
					cout << endl;
				}
				else if(out_option == 2) {
					qout_file << endl;
				}
				else {}
				// ----------------------------------------
		        logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		        AnalyzerTable_Drop();
		        //cout<<"DROP on table parsed successfully \n"<<endl;
		        RestartDiskIO();
		        pass = sqlprocessor.DropTbl(tblname,qout_file,out_option);
		        OutputDiskIO(qout_file, out_option);
		        RemoveVariable();
		        break;

			case SELECT:
				// Printing SQL statement on screen or file
				if(out_option == 1) {
					cout << "SELECT";
				}
				else if(out_option == 2) {
					qout_file << "SELECT";
				}
				else {}
				sql_statement_start = lexer.getCurrentCharacter();
				if(*sql_statement_start == '\n') {
					sql_statement_start++;
				}
				while(*sql_statement_start != '\n') {
					if(out_option == 1) {
						cout << *sql_statement_start;
					}
					else if(out_option == 2) {
						qout_file << *sql_statement_start;
					}
					else {}
					sql_statement_start++;
				}
				if(out_option == 1) {
					cout << endl;
				}
				else if(out_option) {
					qout_file << endl;
				}
				else {}
				// ----------------------------------------
		        logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		        AnalyzerStmt_Select();
                //cout<<"SELECT on table parsed successfully"<<endl;
		        VariablesFromTbl = list_tbl;
				// cout << list_ordby.size() << endl;
				// cout << isunique << endl;
				selLPQ = Gen_QPL::GenerateOptimizedLQPlan(isunique,list_selsub, list_tbl, search_cond,list_ordby);
				// cout << "Created selLPQ" << endl;
		        RestartDiskIO();
				// cout << "Restarted DiskIO" << endl;
		        pass = sqlprocessor.SelectTbl(selLPQ, qout_file, out_option);
		        OutputDiskIO(qout_file, out_option);
		        RemoveVariable();
		        break;

    		case DELETE:
		   		// Printing SQL statement on screen or file
				if(out_option == 1) {
					cout << "DELETE";
				}
				else if(out_option == 2) {
					qout_file << "DELETE";
				}
				else {}
			   	sql_statement_start = lexer.getCurrentCharacter();
			   	if(*sql_statement_start == '\n') {
					sql_statement_start++;
			   	}
			   	while(*sql_statement_start != '\n') {
					if(out_option == 1) {
						cout << *sql_statement_start;
					}
					else if(out_option == 2) {
						qout_file << *sql_statement_start;
					}
					else {}
				   	sql_statement_start++;
			   	}
				if(out_option == 1) {
					cout << endl;
				}
				else if(out_option) {
					qout_file << endl;
				}
				else {}
			   	// ----------------------------------------
				logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		        AnalyzerStmt_Delete();
		        //cout<<"DELETE on table parsed successfully"<<endl;
			    VariablesFromTbl.push_back(tblname);
			    RestartDiskIO();
			    pass = sqlprocessor.DeleteTbl(tblname,search_cond);
			    OutputDiskIO(qout_file, out_option);
			    RemoveVariable();
			    break;

            case INSERT:
				// Printing SQL statement on screen or file
				if(out_option == 1) {
					cout << "INSERT";
				}
				else if(out_option == 2) {
					qout_file << "INSERT";
				}
				else {}
				sql_statement_start = lexer.getCurrentCharacter();
				if(*sql_statement_start == '\n') {
					sql_statement_start++;
				}
				while(*sql_statement_start != '\n') {
					if(out_option == 1) {
						cout << *sql_statement_start;
					}
					else if(out_option) {
						qout_file << *sql_statement_start;
					}
					else {}
					sql_statement_start++;
				}
				if(out_option == 1) {
					cout << endl;
				}
				else if(out_option) {
					qout_file << endl;
				}
				else {}
				// ----------------------------------------
				logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
				statement_start = lexer.getCurrentCharacter();
				while(*lexer.getCurrentCharacter() != '\n')
				{
					mytoken = lexer.GetNextToken();
					// cout << mytoken.TokenValue << endl;
					if(mytoken.TokenType == SELECT) {
						// lexer.RestorePreviousToken();
						select_start = lexer.getCurrentCharacter();
						found_insert_select = true;
						break;
					}
					if(mytoken.TokenType == VALUES) {
						found_insert_select = false;
						break;
					}
				}
				// cout << "Found INSERT SELECT: " << found_insert_select << endl;
				if(found_insert_select == true) {
					lexer.LexerReset(select_start);
					AnalyzerStmt_Select();
					VariablesFromTbl = list_tbl;
					selLPQ = Gen_QPL::GenerateOptimizedLQPlan(isunique,list_selsub, list_tbl, search_cond,list_ordby);
					RestartDiskIO();
					select_results = sqlprocessor.InsertSelectTbl(selLPQ);
					pass = sqlprocessor.Insert_After_Select_Tbl(select_results.relation_name, select_results.tuples);
					OutputDiskIO(qout_file, out_option);
					// for (int i=0;i<fields.size();i++) {
				    //   if (schema.getFieldType(i)==INT)
				    //     out << fields[i].integer << "\t";
				    //   else
				    //     out << *(fields[i].str) << "\t";
				    // }
					// pass = sqlprocessor.InsertTbl(relation_name,field_names,list_value);
				}
				else
				{
					lexer.LexerReset(statement_start);
					AnalyzerStmt_Insert();
	                // cout<<"INSERT on table parsed successfully"<<endl;
					RestartDiskIO();
	                pass = sqlprocessor.InsertTbl(tblname,list_attr,list_value);
					OutputDiskIO(qout_file, out_option);
				}
				// cout << "Finished Insert" << endl;
		        RemoveVariable();
		        break;

			case UNDEFINEDTOK:
				logoutput<<"Found end of the file"<<endl;
				break;

			case EOF:
				logoutput<<"Found EOF"<<endl;
				break;

            default:
				logoutput<<"Found unidentified symbol"<<endl<<mytoken.TokenValue;
            	throw string("Enter a valid starting for the statement: ").append(mytoken.TokenValue);
				break;
        }
        if(pass == false)
        {
          throw string("\nStop !!! Inputfile could not be parsed");
        }
    }
}


void Analyzer::AnalyzerStmt_Delete()
//DELETE FROM table-name [WHERE search-condition]
{
	logoutput<<"Parsing <Delete>"<<endl;
	//Get the next token
  	MyToken mytoken = lexer.GetNextToken();
  	if(mytoken.TokenType == FROM)
  	{
  		AnalyzerName_Table();
  		logoutput<<"Found TableName: "<<tblname<<endl;
  		mytoken = lexer.GetNextToken();
  		if(mytoken.TokenType == WHERE)
  		{
  			logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
  			AnalyzerCond_Search();
  		}
  		else
  			lexer.RestorePreviousToken();
  	}
  	else
  		throw string("Looking for the Keyword FROM");
}

void Analyzer::AnalyzerTable_Create()
//create-table-statement ::= CREATE TABLE table-name(attribute-type-list)
{
	logoutput<<"Analyzing <CreateTableStatement>"<<endl;
	MyToken mytoken;
	mytoken = lexer.GetNextToken();
	switch(mytoken.TokenType)
	{
	case TABLE:
		logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		string relName = AnalyzerName_Table();
		mytoken = lexer.GetNextToken();
		switch(mytoken.TokenType)
		{
		case OPENINGPAREN:
		  logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
			AnalyzerTypeList_Attribute();
			mytoken = lexer.GetNextToken();
			if(mytoken.TokenType == ENCLOSINGPAREN)
			{
				logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
			}
			else
			{
				throw string("), closing parenthesis expected");
			}
			break;
		default:
			RemoveVariable();// clears all the variables, objects created and used
			throw string("(, opening parenthesis expected");
		}
		break;
	}
}

string Analyzer::AnalyzerName_Table()
//Parsing the tablename ::= letter[digit | letter]*
{
	logoutput<<"Analyzing <TableName>"<<endl;
	MyToken mytoken;
	mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == IDENTIFIER)
	{
		logoutput<<"Found Tablename IDENTIFIER: " <<mytoken.TokenValue<<endl;
		tblname = mytoken.TokenValue;
		return mytoken.TokenValue;
	}
	else
	{
		throw string("Tablename is expected");
	}
}

void Analyzer::AnalyzerTypeList_Attribute()
//attribute-type-list ::= attribute-name data-type | attribute-name data-type, attribute-type-list
{
	logoutput<<"Analyzing <AttributeTypeList>"<<endl;
	MyToken mytoken;
	AnalyzerName_Attribute();
	list_AttrName.push_back(attrname);
	AnalyzerType_Data();
	list_DataType.push_back(data_type);
	mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == COMMA)
	{
		logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
		AnalyzerTypeList_Attribute();
	}
	else
		  lexer.RestorePreviousToken();
}

void Analyzer::AnalyzerName_Attribute()
//attribute-name ::= letter[digit | letter]
{
	logoutput<<"Analyzing <AttributeName>"<<endl;
	MyToken mytoken;
	mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == IDENTIFIER)
	{
		attrname = mytoken.TokenValue;
		logoutput<<"Found IDENTIFIER: "<<mytoken.TokenValue<<endl;
	}
	else
	{
		throw string("Attribute Name - Identifier required");
	}
}


void Analyzer::AnalyzerType_Data()
{ 	//data-type ::= INT | STR20
	logoutput<<"Analyzing <DataType>"<<endl;
	MyToken mytoken;
	mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == INT_T)
	{
		logoutput<<"Found DataType:"<<mytoken.TokenValue<<endl;
		data_type = INT;
	}
	else if(mytoken.TokenType == STR20_T)
	{
		logoutput<<"Found DataType:"<<mytoken.TokenValue<<endl;
		data_type = STR20;
	}
	else
	{
		throw string("Data Type Missing!");
	}
}

void Analyzer::DisplayMemory()
{

//	logoutput<<"Memeory has  " << mem.getMemorySize() << " blocks" << endl;
	//logoutput << mem << endl ;
}

void Analyzer::AnalyzerTable_Drop()
//Drop table - 	drop-table-statement ::= DROP TABLE table-name
{
	logoutput<<"Analyzing <DropTableStatement>"<<endl;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType== TABLE)
	{
		logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		AnalyzerName_Table();
		logoutput<<"Table "<<tblname<<"will be dropped"<<endl;
	}
	else
	{
		throw string("Need Keyword TABLE");
	}
}

void Analyzer::AnalyzerStmt_Insert()
//INSERT INTO table-name(attribute-list) insert-tuples
{
	logoutput<<"Analyzing <InsertStatement>"<<endl;
	MyToken mytoken;
	mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == INTO)
	{
		logoutput<<"Found Keyword: "<<mytoken.TokenValue<<endl;
		string tblname = AnalyzerName_Table();
		mytoken = lexer.GetNextToken();
		if(mytoken.TokenType == OPENINGPAREN)
		{
			logoutput<<"Found Syntax: "<<mytoken.TokenValue<<endl;
			AnalyzerList_Attribute();
			mytoken = lexer.GetNextToken();
			if(mytoken.TokenType== ENCLOSINGPAREN)
			{
				logoutput<<"Found Syntax: "<<mytoken.TokenValue<<endl;
				AnalyzerTuples_Insert();
			}
			else
				throw string("Need the Syntax )");
		}
		else
			throw string("Need the syntax (");
	}
	else
	{
		throw string("Need the keyword INTO");
	}
}

void Analyzer::AnalyzerTuples_Insert()
{	//insert-tuples ::= VALUES (value-list) | select-statement
	logoutput<<"Analyzing <InsertTuples>"<<endl;
	MyToken mytoken = lexer.GetNextToken();

	if(mytoken.TokenType == VALUES)
	{
		logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		mytoken = lexer.GetNextToken();
		if(mytoken.TokenType == OPENINGPAREN)
		{
			logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
			AnalyzerList_Value();
			mytoken = lexer.GetNextToken();
			if(mytoken.TokenType == ENCLOSINGPAREN)
			{
				logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
				return;
			}
			else
				throw string("Missing syntax )");
		}
		else
			throw string("Missing Syntax (");
	}
	else if(mytoken.TokenType == SELECT)
	{
		logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
	  AnalyzerStmt_Select();
	}
	else
		throw string("Keyword VALUES required");
}

void Analyzer::AnalyzerList_Attribute()
{//attribute-list ::= attribute-name | attribute-name, attribute-list
	logoutput<<"Analyzing <AttributeList>"<<endl;
  	AnalyzerName_Attribute();
	// cout<<attrname<<endl;
	list_attr.push_back(attrname);
	MyToken mytoken = lexer.GetNextToken();
	// cout<<mytoken.TokenType<<endl;
	if(mytoken.TokenType == COMMA)
	{
		logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
		AnalyzerList_Attribute();
	}
	else
		lexer.RestorePreviousToken();
}

void Analyzer::AnalyzerList_Value()
{//value-list ::= value | value, value-list
	logoutput<<"Analyzing <ValueList>"<<endl;
	Analyzer_Value();
	list_value.push_back(myval);
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == COMMA)
	{
		logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
		AnalyzerList_Value();
	}
	else
	   lexer.RestorePreviousToken();
}

void Analyzer::Analyzer_Value()
{
	logoutput<<"Analyzing <Value>"<<endl;
	//value ::= literal | integer | NULL
	MyToken mytoken = lexer.GetNextToken();
	// cout<<mytoken.TokenType<<mytoken.TokenValue<<endl;
	switch(mytoken.TokenType)
	{
	case LITERAL:
		logoutput<<"Found LITERAL:"<<mytoken.TokenValue<<endl;
		myval =  mytoken.TokenValue;
		break;
	case INTEGER:
		logoutput<<"Found INTEGER:"<<mytoken.TokenValue<<endl;
		myval =  mytoken.TokenValue;
		break;
	case NULL_T:
		logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		myval =  mytoken.TokenValue;
		break;
	default:
		throw string("Looking for a LITERAL/ INTEGER/ NULL");
	}
}

void Analyzer::AnalyzerStmt_Select()
{	//select-statement ::= SELECT [DISTINCT] select-list FROM table-list [WHERE search-condition]
	//[ORDER BY colume-name]
	logoutput<<"Analyzing <SelectStatement>"<<endl;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == DISTINCT)
	{
		logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		isunique = true;
		AnalyzerList_Select();
	}
	else
	{
		lexer.RestorePreviousToken();
		AnalyzerList_Select();
	}

	mytoken =lexer.GetNextToken();
	if(mytoken.TokenType == FROM)
	{
		logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
		AnalyzerList_Table();
		mytoken = lexer.GetNextToken();
		if(mytoken.TokenType == WHERE)
		{
			logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
			AnalyzerCond_Search();
		}
		else
			lexer.RestorePreviousToken();

		mytoken =lexer.GetNextToken();
		if(mytoken.TokenType == ORDER)
		{
			logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
			mytoken = lexer.GetNextToken();
			if(mytoken.TokenType == BY)
			{
				logoutput<<"Found Keyword:"<<mytoken.TokenValue<<endl;
				AnalyzerName_Column();
				// cout << mycol.colName << endl;
				list_ordby.push_back(mycol);
			}
			else
				throw string("Looking for Keyword - BY");
		}
		else
			lexer.RestorePreviousToken();
	}
	else
		throw string("Looking for Keyword - FROM");
}

void Analyzer::AnalyzerList_Select()
{//select-list ::= * | select-sublist
	logoutput<<"Analyzing <SelectList>"<<endl;
	MyToken mytoken = lexer.GetNextToken();
	// cout << mytoken.TokenType << endl;
	if(mytoken.TokenType == MULTIPLY) // if select * from table
	{
		mycol.colName = mytoken.TokenValue;//colName Processing.h
		mycol.tblName = NOT_A_TABLE;//tblName from Processing.h
		list_selsub.push_back(mycol);//append the contents in mycol to the vector list_selsub
		logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
	}
	else
	{
		lexer.RestorePreviousToken();
		AnalyzerSubList_Select();
	}
}

void Analyzer::AnalyzerSubList_Select()
{	//select-sublist ::= column-name | column-name, select-sublist
	logoutput<<"Analyzing <ParseSelectSublist>"<<endl;
	AnalyzerName_Column();
	list_selsub.push_back(mycol);
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == COMMA)
	{
		logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
		AnalyzerSubList_Select();
	}
	else
		lexer.RestorePreviousToken();
}

void Analyzer::AnalyzerList_Table()
{//table-list ::= table-name | table-name, table-list
	logoutput<<"Found <TableList>"<<endl;
	AnalyzerName_Table();
	list_tbl.push_back(tblname);
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == COMMA)
	{
		logoutput<<"Found Syntax:"<<mytoken.TokenValue<<endl;
	   AnalyzerList_Table();
	}
	else
		lexer.RestorePreviousToken();
}

void Analyzer::AnalyzerName_Column()
{	//column-name ::= [table-name.]attribute-name
	logoutput<<"Parsing <ColumnName>"<<endl;
	MyToken mytoken;
	string colName;
	string tblName1;
	//table-name ::= letter[digit | letter]*
	MyToken mytoken1;
	mytoken1 = lexer.GetNextToken();
	if(mytoken1.TokenType == IDENTIFIER)
	{
		//display tablename here
		logoutput<<"Found a Tablename IDENTIFIER: " <<mytoken1.TokenValue<<endl;
		tblName1 = mytoken1.TokenValue;
	}
	else
	{
		throw string("Tablename/Columnname is required");
	}
	//----------------
	mytoken = lexer.GetNextToken();// Read PERIOD(".")
	if(mytoken.TokenType == PERIOD)
	{
		mytoken = lexer.GetNextToken();
		if(mytoken.TokenType == IDENTIFIER)
		{
			colName = mytoken.TokenValue;
			logoutput<<"Found Column Name: "<<tblName1<<"."<<colName<<endl;
		}
		else
			throw ("Column-name is required after the PERIOD(.)");
	}
	else
	{
		//If no PERIOD operator then the IDENTIFIER fetched in the 1ST position is the column-name,not table-name.
		colName = tblName1;
		tblName1 = NOT_A_TABLE;
		logoutput<<"Found Columname: "<<colName<<endl;
		//Revert to previous mytoken since there were no PERIOD to read.
		lexer.RestorePreviousToken();
	}

	logoutput<<tblName1<<"."<<colName<<endl;
	mycol.tblName = tblName1;
	mycol.colName = colName;
}

void Analyzer::Analyzer_Factor()
{//factor ::= column-name | literal | integer | ( expression )
	string col_Name;
	logoutput<<"Analyzing <Factor>"<<endl;
	MyToken mytoken = lexer.GetNextToken();
	switch(mytoken.TokenType)
	{
	case IDENTIFIER: //Column Name
		lexer.RestorePreviousToken();
		AnalyzerName_Column();
		col_Name = mycol.tblName;
		col_Name.append(".");
		col_Name.append(mycol.colName);
		exp_Factor = new Expressions(col_Name,COLNAME_VALUE);
		break;
	case LITERAL:
		logoutput<<"Found a LITERAL: "<<mytoken.TokenValue<<endl;
		logoutput<< "Literal: "<<mytoken.TokenValue<<endl;
		exp_Factor = new Expressions(mytoken.TokenValue,LITERAL_VALUE);
		break;
	case INTEGER:
		logoutput<<"Found an INTEGER: "<<mytoken.TokenValue<<endl;
		logoutput<< "Integer: "<<mytoken.TokenValue<<endl;
		exp_Factor = new Expressions(mytoken.TokenValue,INTEGER_VALUE);
		break;
	case OPENINGPAREN:
		logoutput<<"Found a Syntax: "<<mytoken.TokenValue<<endl;
		Analyzer_Expression();
		mytoken = lexer.GetNextToken();
		if(mytoken.TokenType == ENCLOSINGPAREN)
		{
			logoutput<<"Found a Syntax: "<<mytoken.TokenValue<<endl;
			exp_Factor = exp_Expression;
		}
		else
			throw string("Looking for a Syntax )");
		break;
	default:
		throw string("Need an IDENTIFIER/ LITERAL/ INTEGER or an (EXPRESSION)");
	}

}
void Analyzer:: Analyzer_Term()
{//term ::= factor | factor * term | factor / term
	logoutput<<"Analyzing <Term>"<<endl;
	Analyzer_Factor();
	exp_Term = exp_Factor;
	Expressions *leftFactorNode = exp_Factor;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == MULTIPLY)
	{
		logoutput<<"Found an Operator: "<<mytoken.TokenValue<<endl;
		Expressions* left = exp_Term;
		Analyzer_Term();
		Expressions* right = exp_Term;
		exp_Term = new Expressions(left, mytoken.TokenValue, right);
	}
	else if(mytoken.TokenType == DIVISION)
	{
		logoutput<<"Found an Operator: "<<mytoken.TokenValue<<endl;
		Expressions* left = exp_Term;
		Analyzer_Term();
		Expressions* right = exp_Term;
		exp_Term = new Expressions(left, mytoken.TokenValue, right);
	}
	else
		lexer.RestorePreviousToken();
}

void Analyzer::Analyzer_Expression()
{//expression ::= term | term + expression | term - expression
	logoutput<<"Analyzing <Expression>"<<endl;
	Analyzer_Term();
	exp_Expression =  exp_Term;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == ADDITION)
	{
		logoutput<<"Found an Operator: "<<mytoken.TokenValue<<endl;
		Expressions* left = exp_Expression;
		Analyzer_Expression();
		Expressions* right = exp_Expression;
		exp_Expression = new Expressions(left, mytoken.TokenValue, right);
	}
	else if(mytoken.TokenType == MINUS)
	{
		logoutput<<"Found an Operator: "<<mytoken.TokenValue<<endl;
		Expressions* left = exp_Expression;
		Analyzer_Expression();
		Expressions* right = exp_Expression;
		exp_Expression = new Expressions(left, mytoken.TokenValue, right);
	}
	else
		lexer.RestorePreviousToken();
}
void Analyzer::AnalyzerFactor_Boolean()
{	//boolean-factor ::= [NOT] boolean-primary
	//boolean-factor ::= [NOT] (boolean-primary)
	logoutput<<"Parsing <BooleanFactor>"<<endl;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == NOT)
	{
		logoutput<<"Found a Keyword: "<<mytoken.TokenValue<<endl;
		AnalyzerPrimary_Boolean();
		Search* null_node = NULL;
		boolFacSearch_cond = new Search(null_node, mytoken.TokenValue, boolPriSearch_cond);
	}
	else
	{
		lexer.RestorePreviousToken();
		AnalyzerPrimary_Boolean();
		boolFacSearch_cond = boolPriSearch_cond;
	}
}


void Analyzer::AnalyzerPrimary_Boolean()
{//boolean-primary ::= comparison-predicate |"[" search-condition "]"
	logoutput<<"Parsing <BooleanPrimary>"<<endl;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == OPENINGSQPAREN)
	{
		logoutput<<"Found a Syntax: "<<mytoken.TokenValue<<endl;
		AnalyzerCond_Search();
		mytoken = lexer.GetNextToken();
		if(mytoken.TokenType == ENCLOSINGSQPAREN)
		{
			logoutput<<"Found a Syntax: "<<mytoken.TokenValue<<endl;
			boolPriSearch_cond = search_cond;
		}
		else
			throw string("Looking for a Syntax )");
	}
	else
	{
		lexer.RestorePreviousToken();
		AnalyzerPredicate_Comparison();
		boolPriSearch_cond  = new Search(pred_Comparison);
	}
}

void Analyzer::AnalyzerPredicate_Comparison()
{//comparison-predicate ::= expression comp-op expression
	logoutput<<"Analyzing <Comparision-Predicate>"<<endl;
	Analyzer_Expression();
	Expressions* left = exp_Expression;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == COMPOPERATOR)
	{
		logoutput<<"Found a COMPARISON OPERATOR Syntax: "<<mytoken.TokenValue<<endl;
		Analyzer_Expression();
		Expressions* right = exp_Expression;
		pred_Comparison = new Compare(left, mytoken.TokenValue, right);
	}
	else
		throw string ("Looking for a Logical Operator =,> or <");
}

void Analyzer::AnalyzerCond_Search()
{	//search-condition ::= boolean-term | boolean-term OR search-condition
	AnalyzerTerm_Boolean();
	logoutput<<"Analyzing <Search Condition>"<<endl;
	search_cond = boolTerSearch_cond;
	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == OR)
	{
		logoutput<<"Found a Keyword: OR"<<endl;
		Search* left = search_cond;
	  AnalyzerCond_Search();
		Search* right = search_cond;
		search_cond = new Search(left, mytoken.TokenValue, right);
	}
	else
	  lexer.RestorePreviousToken();
}

void Analyzer::AnalyzerTerm_Boolean()
{ //boolean-term ::= boolean-factor | boolean-factor AND boolean-term
	logoutput<<"Analyzing <BooleanTerm>"<<endl;
	AnalyzerFactor_Boolean();
	boolTerSearch_cond = boolFacSearch_cond;
  	MyToken mytoken = lexer.GetNextToken();
	if(mytoken.TokenType == AND)
	{
		logoutput<<"Found a Keyword: AND"<<endl;
		Search* left = boolTerSearch_cond;
		AnalyzerTerm_Boolean();
		Search* right = boolTerSearch_cond;
		// cout << mytoken.TokenValue << endl;
		boolTerSearch_cond = new Search(left,mytoken.TokenValue, right);
	}
	else
		lexer.RestorePreviousToken();
}
