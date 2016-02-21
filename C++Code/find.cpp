#include "find.h"

using namespace std;

Search::Search(Compare*& myvalue)
{
	find_operator = find_SELF;
	compare_predicate = myvalue;
	search_left  = NULL;
	search_right = NULL;
}

Search::Search(Search*& condition_left, string find_cond_operator, Search*& condition_right)
{
			if(find_cond_operator == "AND"){
				// cout << "Assigning find_operator as find_AND" << endl;
				find_operator = find_AND;
				// cout << find_operator << endl;
			}
			else if(find_cond_operator == "OR")
				find_operator = find_OR;
			else if(find_cond_operator == "NOT")
				find_operator = find_NOT;
			search_left = condition_left;
			search_right = condition_right;

}
bool Search::Compute()
{
	// cout << "Entering Search::Compute" << endl;
	// cout << find_operator << endl;
	bool result;
	switch(find_operator)
	{
			case find_AND:
					result = (search_left->Compute() && search_right->Compute());
					break;
			case find_OR:
					result = (search_left->Compute() || search_right->Compute());
					break;
			case find_NOT:
					result = (!search_right->Compute());
					break;
			case find_SELF:
					result = compare_predicate->Compute();
					break;
	}
	// cout << "Leaving Search::Compute" << endl;
	return result;
}

void Search::operator delete(void *pointer)
{
		if(pointer!=NULL)
		{
				if(((Search*)pointer)->compare_predicate != NULL)
					delete ((Search*)pointer)->compare_predicate;

				if(((Search*)pointer)->search_left != NULL)
					delete ((Search*)pointer)->search_left;

				if(((Search*)pointer)->search_right != NULL)
					delete ((Search*)pointer)->search_right;
				free(pointer);
		}
}
