// Check if the .h file is defined using the “#ifndef” . If not define using “#define”
//
#ifndef _KEYWORDS_H
#define _KEYWORDS_H
#include<string>
#include"MyTokenType.h"

class MyKeyWords{
public:
//Constructor for class MyKeyWords
  MyKeyWords()
  {
      fillKeywordMap();// maps the keyword to a value;
  }

  int KeywordToken(string value)
  {
      return mappedKeyword[value]; //return the mapped the keyword to a value
  }

  private:
  //create a map to store the key value pairs of the token
  map<string,int> mappedKeyword;

  void fillKeywordMap()
  {
      //maps the key associated with the value of the Token from the header file MyTokenType.h
      mappedKeyword["SELECT"]=SELECT;
      mappedKeyword["DISTINCT"] = DISTINCT;
      mappedKeyword["FROM"] = FROM;
      mappedKeyword["WHERE"]=WHERE;
      mappedKeyword["AND"] = AND;
      mappedKeyword["NOT"]=NOT;
      mappedKeyword["OR"]=OR;
      mappedKeyword["ORDER"]=ORDER;
      mappedKeyword["BY"] = BY;
      mappedKeyword["CREATE"] = CREATE;
      mappedKeyword["TABLE"]=TABLE;
      mappedKeyword["INSERT"]=INSERT;
      mappedKeyword["INTO"]=INTO;
      mappedKeyword["VALUES"]=VALUES;
      mappedKeyword["DELETE"] = DELETE;
      mappedKeyword["DROP"] = DROP;
      mappedKeyword["INT"]=INT_T;
      mappedKeyword["NULL"] = NULL_T;
      mappedKeyword["STR20"]=STR20_T;

  }
};

#endif
