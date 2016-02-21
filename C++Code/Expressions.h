//Program to evaluate expressions
#ifndef _EXPRESSIONS_H
#define _EXPRESSIONS_H

#include<string>
#include<cstdlib>
#include<map>

#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "DB_config.h"

using namespace std;

typedef pair<string,string>  TColPair;
typedef pair<FIELD_TYPE,Field> FValue;

enum EXPOPERATOR{ePlus=1,eMinus,eMultiply,eDivision,eSelf};
enum VALUETYPE {INTEGER_VALUE,LITERAL_VALUE,COLNAME_VALUE};

class ColumnMapping
{
    map<string,int> colMap;
};

class Expressions
{
    private:
      EXPOPERATOR exp_operator;
      VALUETYPE valuetype;
      string value;
      Expressions *exp_left;
      Expressions *exp_right;

    public:
      static map<string, int> colMap; //map the column name to interger value.
      Expressions();
      Expressions(string value, VALUETYPE valuetype);
      Expressions(Expressions*& exp_left,string exp_opt,Expressions*& exp_right);
      FValue Compute();
      void operator delete(void *pointer);
      EXPOPERATOR get_exp_operator();
};
#endif
