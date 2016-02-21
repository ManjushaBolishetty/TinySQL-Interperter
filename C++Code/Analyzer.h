#ifndef _ANALYZER_H
#define _ANALYZER_H
#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<cstring>
#include<cstdio>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "Lexer.h"
#include "SqlProcessor.h"
#include "LogicalPlan.h"
#include "Processing.h"
#include "GeneratorLQP.h"
#include "LqpOptimizer.h"
#include "DB_config.h"

class Analyzer
{
private:
    Lexer lexer;
    SqlProcessor sqlprocessor;
    void AnalyzerTable_Create();
    string AnalyzerName_Table();
    void AnalyzerTypeList_Attribute();
    void AnalyzerName_Attribute();
    void AnalyzerType_Data();
    void AnalyzerTable_Drop();
    void AnalyzerStmt_Insert();
    void AnalyzerTuples_Insert();
    void AnalyzerList_Attribute();
    void AnalyzerList_Value();
    void Analyzer_Value();
    void AnalyzerStmt_Select();
    void AnalyzerList_Select();
    void AnalyzerSubList_Select();
    void AnalyzerList_Table();
    void AnalyzerStmt_Delete();
    void AnalyzerCond_Search();
    void AnalyzerTerm_Boolean();
    void AnalyzerFactor_Boolean();
    void AnalyzerPrimary_Boolean();
    void AnalyzerPredicate_Comparison();
    void Analyzer_Expression();
    void AnalyzerOperator_Comparison();
    void Analyzer_Term();
    void Analyzer_Factor();
    void AnalyzerName_Column();

    void OutputDiskIO(ofstream& qout_file, int output_option);
    void RestartDiskIO();

public:
    Analyzer();
    void RemoveVariable();
    Analyzer(char *inputtext);
    Analyzer(char mystring);
    void AnalyzeInput(ofstream& qout_file, int out_option);
    void DisplayMemory();
};

typedef GeneratorLQP Gen_QPL;
typedef LqpOptimizer Opt_QPL;

#endif
