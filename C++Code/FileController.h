//FileController.cpp reads the user input
#ifndef _FILECONTROLLER_H
#define _FILECONTROLLER_H

#include<iostream>
#include<string>
#include<fstream>
using namespace std;

class FileController
{
private:
    char inputfile[100];
public:
    char* GetFile(char* inputfile);
    string GetFileString(char* inputfile);
};
#endif
