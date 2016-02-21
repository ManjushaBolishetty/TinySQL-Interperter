//FileController.cpp reads the file line by line and reads the completefile to the function which
//called the FileController.cpp
#include <fstream>
#include "FileController.h"

char* FileController::GetFile(char* inputfile)
{
  //Ask for inputfile
  ifstream in(inputfile);
  if(!in)
  {
      cout<<"Cannot access the inputfile " << inputfile<<endl;
      return NULL;
  }
  char sentencelength[255]; //holds a single statement line
  char completefile[500000]= " "; //holds a set of statements
  //While the length of the input line is 255 or less append the line to the input file
  while(in.getline(sentencelength,255))
  {
    //   cout<<"sentencelength"<<endl;
      strcat(completefile,sentencelength);
      strcat(completefile,"\n");
  }
  return completefile;
}


string FileController::GetFileString(char* inputfile)
{
  ifstream in(inputfile);
  if(!in)
  {
      cout<<"Cannot access the String inputfile " << inputfile<<endl;
      string s = " ";
      return s;
  }

  char sentencelength[255]; //holds a single statement line
  string completefileString= " "; //holds a set of statements
  //While the length of the input line is
  //255 or less append the line to the input file
  while(in.getline(sentencelength,255))
  {
      completefileString.append(sentencelength);
      completefileString.append("\n");
  }
  return completefileString;
}
