//Handles all scenarios involving comparison operators
#include "Comparison.h"
typedef pair<string, string> TColPair;
typedef pair<FIELD_TYPE, Field> FValue;

Compare::Compare(bool value)
{
	myvalue = value;
	c_opt = comp_self;
	exp_left = NULL;
	exp_right = NULL;
}

Compare::Compare(Expressions*& expLeft, string copt, Expressions*& expRight)
{
	if(copt == "=")
		c_opt = comp_equal;
	else if(copt == "<")
		c_opt = comp_less;
	else
		c_opt = comp_greater;
	 exp_left = expLeft;
	 exp_right = expRight;
}

bool Compare::Compute()
{
	// cout << "Entering Compare::Compute" << endl;
	FIELD_TYPE field_type_left;
	FIELD_TYPE field_type_right;
	FValue value_left;
	FValue value_right;
	bool result;
	bool excption = false;
	// cout << c_opt << endl;
	switch(c_opt)
	{
	case comp_less:
					value_left = exp_left->Compute();
					value_right = exp_right->Compute();
					if(value_left.first == INT && value_right.first == INT)
						result = (value_left.second.integer < value_right.second.integer);
					else if(value_left.first == STR20 && value_right.first == STR20)
					{
						if( (*(value_left.second.str)).compare(*(value_right.second.str)) <0)
							result = true;
						else
							result = false;
					}
					else
						excption = true;
					break;
	case comp_greater:
					value_left = exp_left->Compute();
					value_right = exp_right->Compute();
					if(value_left.first == INT && value_right.first == INT)
								result = (value_left.second.integer > value_right.second.integer);
					else if(value_left.first == STR20 && value_right.first == STR20)
					{
								if( (*(value_left.second.str)).compare(*(value_right.second.str)) >0)
									result = true;
								else
									result = false;
					}
					else
						excption = true;
					break;
	case comp_equal:
					// cout << exp_left->get_exp_operator() << endl;
					// cout << exp_right->get_exp_operator() << endl;
					value_left = exp_left->Compute();
					value_right = exp_right->Compute();
					if(value_left.first == INT && value_right.first == INT) {
						// cout << "In Compare::Compute, int, int" << endl;
							result = (value_left.second.integer == value_right.second.integer);
					}
					else if(value_left.first == STR20 && value_right.first == STR20)
					{
							// cout << "In Compare::Compute, Before compare" << endl;
							if( (*(value_left.second.str)).compare(*(value_right.second.str)) == 0)
								result = true;
							else
								result = false;
							// cout << "In Compare::Compute, After compare" << endl;
					}
		else
					excption = true;
					break;
	case comp_self:
					result = myvalue;
					break;
	}
	if(excption)
			throw string("Comparison between different data types, Error!!");
	// cout << "In Compare::Compute, Before return" << endl;
	// cout << result << endl;
	return result;
}

void Compare::operator delete(void *pointer)
{
		if(pointer!=NULL)
		{
					if(((Compare*)pointer)->exp_right != NULL)
					{
						delete ((Compare*)pointer)->exp_right;
						((Compare*)pointer)->exp_right = NULL;
					}
					if(((Compare*)pointer)->exp_left != NULL)
					{
						delete ((Compare*)pointer)->exp_left;
						((Compare*)pointer)->exp_left = NULL;
					}
					free(pointer);
					pointer = NULL;
		}
}
