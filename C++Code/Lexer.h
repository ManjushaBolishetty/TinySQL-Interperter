//Programs analyses the input file by returned bythe filecontroller to tokenize the input
//  Lexer.h
//  DbProject
//
//  Created by Manjusha Bolishetty on 11/4/15.
//  Copyright © 2015 Manjusha Bolishetty. All rights reserved.
//

#ifndef Lexer_h
#define Lexer_h
#include<fstream>
#include<iostream>
#include"MyToken.h"
#include"MyKeyWords.h"
using namespace std;

class Lexer
{
    private:
    //Lexer can parse a maximum of 200 characters.
        char inputToParse[500000];
        //Pointer to point to my current and previous token
        char* current_char;
        char* previous_char;
        //Static objects can be called without instantiating an object of that class.
        static MyKeyWords keyword;

    public:
        //Constructor
        Lexer();
        Lexer(char* inputtext);

        //calling the method GetNextToken() of the type class MyToken
        MyToken GetNextToken();

        //calling the method GetNexTCharacter() which returns a token of character type
        char GetNextCharacter();

        //call RestorePreviousToken() to access the previous token
        void RestorePreviousToken();

        //these 2 used only for condition Insert into table select * from table
        //basically go back once you have gone forward while parsing a piece of text
        char* getCurrentCharacter();//returns the current position in parsing text
        void LexerReset(char* reset_character); //resets the parsing pointer to the point specified

};
#endif /* Lexer_h */
