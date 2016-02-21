//Handles all scenarios involving comparison operators
#ifndef _COMPARISON_H
#define _COMPARISON_H
#include <cstdlib>
#include <iostream> // For debugging
#include "Expressions.h"
enum Comparison_Operator {comp_less, comp_greater, comp_equal, comp_self};//< , > , ==

class Compare
{
private:
	bool myvalue;
	Comparison_Operator c_opt;
	Expressions* exp_left;
	Expressions* exp_right;
public:
	Compare(bool value);
	Compare(Expressions*& expLeft, string c_opt, Expressions*& expRight);
	bool Compute();
	void operator delete(void *pointer);
};
#endif
