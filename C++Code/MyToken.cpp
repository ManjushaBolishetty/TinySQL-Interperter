// Include the MyToken.h header file
#include"MyToken.h"

//When the num of tokens is 0 initialize value with an empty string
MyToken::MyToken():TokenType(0)
{
  strcpy(TokenValue,"");
}

MyToken::MyToken(int tokentype, char value[])
{
    TokenType = tokentype;
    strcpy(TokenValue,value);
}
