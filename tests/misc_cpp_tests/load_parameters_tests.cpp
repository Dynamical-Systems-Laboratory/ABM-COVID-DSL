#include "../common/test_utils.h"
#include <string>
#include "../../include/io_operations/load_parameters.h"

/*************************************************************** 
 * Suite for testing LoadParameters class 
 **************************************************************/

// Tests
bool read_parameters_test();
bool read_age_dependent_distribution_test();

// Supporting functions
bool equal_maps(std::map<std::string, double>, std::map<std::string, double>);

int main()
{
	test_pass(read_parameters_test(), "Load infection parameters");
	test_pass(read_age_dependent_distribution_test(), "Load age-dependent distributions");
}

/// Test for loading infection parameters
bool read_parameters_test()
{
	std::map<std::string, double> expected = 
			{{"days in December", 31.0}, {"feet in meters", 3.280}, 
			 {"hours", 1.5}};
	std::map<std::string, double> loaded = {};

	LoadParameters ldp;
	loaded = ldp.load_parameter_map("test_data/ldp_input.txt");

	return equal_maps(expected, loaded);
}

/// Test for loading infection parameters
bool read_age_dependent_distribution_test()
{
	std::map<std::string, double> expected = 
			{{"0-9", 0.001}, {"10-19", 0.003}, 
			 {"20-29", 0.012}, {"30-39", 0.032}};
	std::map<std::string, double> loaded = {};

	LoadParameters ldp;
	loaded = ldp.load_age_dependent("test_data/age_dependent_dist.txt");

	return equal_maps(expected, loaded);
}

/// \brief Test two maps for equality
bool equal_maps(std::map<std::string, double> expected, std::map<std::string, double> loaded)
{
	if (expected.size() != loaded.size())
		return false;

	for (const auto& entry : loaded){
		const auto iter = expected.find(entry.first);
		if (iter == expected.end()){
			return false;
		} else {
			if (!float_equality<double>(iter->second, entry.second, 1e-5))
				return false;
		}
	}

	return true;		
}
