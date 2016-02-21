// Check if the .h file is defined using the “#ifndef” . If not define using “#define”
#ifndef _MYTOKENTYPE_H
#define _MYTOKENTYPE_H

//Include in the string library in the header when you want to define string variables
#include<string>

//include the method map to store key-value pairs
#include<map>

//Include the std class in the headers
using namespace std;

//defining constant values for each of the token types using enumerator or enum
enum
{
      AND = 1, //starting the sequence from 1
      BY = 2,      //BY is assigned with 2
      COMPOPERATOR = 3, //comparison or relational operators =|<|>
      COMMA = 4,//,
      CREATE = 5,
      DELETE = 6,
      DISTINCT = 7,
      DIVISION = 8,// / Division sign - added
      PERIOD = 9,// .
      DROP = 10,
      ENCLOSINGSQPAREN = 11, //] Enclosing SQuare parenthesis
      ENCLOSINGPAREN = 12,//) Enclosing curly parenthesis
      FROM = 13,
      IDENTIFIER = 14,//tablename or columnname
      INSERT = 15,
      INT_T = 16,
      INTO = 17,
      LITERAL = 18,//a string of characters
      MINUS = 19,//-
      NOT = 20,
      NULL_T =21,
      INTEGER = 22,
      OPENINGSQPAREN = 23, //[ opening square parenthesis
      OPENINGPAREN = 24,//( opening parenthesis
      OR = 25,
      ORDER = 26,
      ADDITION = 27,//+
      SELECT = 28,
      SEMICOLON = 29,//;
      STR20_T = 30,
      MULTIPLY = 31, //*
      TABLE = 32,
      UNDEFINEDTOK = 33,// UNDEFINED Token
      VALUES = 34,
      WHERE = 35
};
#endif
