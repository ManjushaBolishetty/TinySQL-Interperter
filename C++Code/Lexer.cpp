// Programs analyses the input file by returned bythe filecontroller to tokenize the input
//  Lexer.cpp
//  DbProject
//
//  Created by Manjusha Bolishetty on 11/4/15.
//  Copyright © 2015 Manjusha Bolishetty. All rights reserved.
//
#include"Lexer.h"
MyKeyWords Lexer::keyword;

//Creating an instance of the Lexer class
Lexer::Lexer(){
    // cout<<"Initiating Lexical Analyser"<<endl;
}

//Initializing the Lexical analyzer with the input text to parse
Lexer::Lexer(char* inputtext){

    strcpy(inputToParse,inputtext);
    current_char = inputToParse;
    previous_char = current_char;
}


//Getting the next character for the lexer to parse
char Lexer::GetNextCharacter(){
    return *current_char;
}

//Get the next token
MyToken Lexer::GetNextToken(){
    previous_char = current_char;
    MyToken token;

    while(*current_char == '\n' || *current_char == '\t' || *current_char == ' ')
    {
        *current_char++;
    }

    if(*current_char == ',')
    {
        token = MyToken(COMMA, ",");
        current_char++;
    }
    else if(*current_char == '"')
    {

        int index = 0;
        current_char++;
        while(*current_char != '"')
        { // get the string within the double qoutes like A in "A"
            token.TokenValue[index] = *current_char;
            index++;
            current_char++;
        }
        token.TokenValue[index] = '\0'; //assign a null string to tokenvalue at index 0
        current_char++;
        if(*current_char != ' ' && *current_char != '\n' && *current_char!=',' && *current_char!= ')' && *current_char!='\0') {
            //Check if there is any character other than above mentioned the throw an error
            //Example "A&B" should throw Incorrect Literal error. It should be "A & B"
            throw string("Incorrect Literal");
        }
        token.TokenType = LITERAL;
    }
    else if(*current_char == '/')
    {
        token = MyToken(DIVISION,"/");
        current_char++;

    }
    else if(*current_char == '.')
    {
        token = MyToken(PERIOD,".");
        current_char++;

    }
    else if(*current_char == ')')
    {
        token = MyToken(ENCLOSINGPAREN,")");
        current_char++;

    }
    else if(*current_char == ']')
    {
        token = MyToken(ENCLOSINGSQPAREN,"]");
        current_char++;

    }
    else if(*current_char == '(')
    {
        token = MyToken(OPENINGPAREN,"(");
        current_char++;

    }
    else if(*current_char == '[')
    {
        token = MyToken(OPENINGSQPAREN,"[");
        current_char++;

    }
    else if(*current_char == '-')
    {
        token = MyToken(MINUS,"-");
        current_char++;

    }
    else if(*current_char == '+')
    {
        token = MyToken(ADDITION,"+");
        current_char++;

    }
    else if(*current_char == ';')
    {
        token = MyToken(SEMICOLON,";");
        current_char++;

    }
    else if(*current_char == '*')
    {
        token = MyToken(MULTIPLY,"*");
        current_char++;

    }
    else if(*current_char == '=' || *current_char == '>' || *current_char == '<')
    {
        token.TokenValue[0] = *current_char;
        token.TokenValue[1] = '\0';
        token.TokenType = COMPOPERATOR;
        current_char++;
    }
    else if(isalnum(*current_char))
    {// check if the input is an interger or a string.
        int index =0;
        bool all_digit = true;
        bool not_an_identifier = false;
        if(isdigit(*current_char))
            not_an_identifier = true; // not a table or a columnname
            while(isalnum(*current_char))
            {
                if(isalpha(*current_char)) {
                    all_digit = false; //input is not an integer
                }
                token.TokenValue[index] = *current_char;
                current_char++;
                index++;
            }
            token.TokenValue[index] = '\0';
            int type;
            if(all_digit)
            {
                token.TokenType = INTEGER; //mark tokentype as an integer
            }
            else if((type = keyword.KeywordToken(token.TokenValue))!=0) {
                token.TokenType = type;
            }
            else if(type == 0)
            {
                if(!not_an_identifier) {
                    token.TokenType = IDENTIFIER;
                }
                else {
                    token.TokenType = UNDEFINEDTOK;
                    strcat(token.TokenValue,"Error - Invalid Token");
                }
            }
        }
        else if(*current_char == '\0')
        {
            token = MyToken(EOF,"End of File");
        }
        else if(*current_char!= '\n' && *current_char!=' ')
        {
            token.TokenType = UNDEFINEDTOK;
            token.TokenValue[0] = *current_char;
            token.TokenValue[1] = '\0';
            strcat(token.TokenValue, "ERROR - Undefined Token");
            current_char++;
        }
    return token;
}

void Lexer::RestorePreviousToken()
{
    current_char = previous_char;
}

char* Lexer::getCurrentCharacter() {
    return current_char;
}

void Lexer::LexerReset(char* reset_character){
    current_char = reset_character;
    previous_char = current_char;
}
