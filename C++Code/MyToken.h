// Check if the .h file is defined using the “#ifndef” . If not define using “#define”
#ifndef _MYTOKEN_H
#define _MYTOKEN_H

//Include in the string library in the header when you want to define string variables
#include<string>
using namespace std;

class MyToken
{
public:
// declaration of variables to hold the token and its datatype
int TokenType;
char TokenValue[80];
MyToken();
MyToken(int tokentype, char value[]);
};

//End the if used to check if the header file exists
#endif
