#include "../common/test_utils.h"
#include "../../include/utils.h"

/*************************************************************** 
 * Suite for testing utils.h, common utility functions
 **************************************************************/

// Tests
bool str_to_lower_test();

int main()
{
	test_pass(str_to_lower_test(), "Convert string to all lowercase");	
}

bool str_to_lower_test()
{
	std::vector<std::string> test_strings = 
		{"String", "STRING", "stRINg", "string", "strinG"};
	std::string exact_string("string");

	for (const auto& str : test_strings){
		if (exact_string != str_to_lower(str))
			return false;
	}
	
	return true;
}
