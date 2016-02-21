#ifndef _GLOBALFILE_H
#define _GLOBALFILE_H
#include <fstream>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"



int myglobal;
//define the objects of Mainmemory , Disk classes.
MainMemory mem;
Disk disk;
SchemaManager schema_manager(&mem,&disk);
vector<string> VariablesFromTbl;

typedef pair<string,string>  TColPair;
typedef pair<FIELD_TYPE,Field> FValue;
map<TColPair, FValue> Column_Value_Mapping;

#endif
