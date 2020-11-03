#include "testing_class_tests.h"

/***************************************************** 
 *
 * Test suite for functionality of the Testing class
 *
 *****************************************************/

bool testing_time_dependence_test();

int main()
{
	test_pass(testing_time_dependence_test(), "Time dependence of testing");
}

bool testing_time_dependence_test()
{
	// Create agents 
	std::string fin("test_data/agents_test.txt");

	// Files with place info
	std::string hfile("test_data/houses_test.txt");
	std::string sfile("test_data/schools_test.txt");
	std::string wfile("test_data/workplaces_test.txt");
	std::string hspfile("test_data/hospitals_test.txt");
	std::string rh_file("test_data/rh_test.txt");

	// Model parameters
	// Time in days, space in km
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 400;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dexp_name("test_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"exposed never symptomatic", dexp_name}, {"hospitalization", dh_name}, 
		  {"ICU", dhicu_name}, {"mortality", dmort_name} };	
	// File with 	
	std::string tfname("test_data/tests_with_time.txt");
	
	ABM abm(dt, pfname, dfiles, tfname);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	abm.create_hospitals(hspfile);
	abm.create_retirement_homes(rh_file);
	// Then the agents
	abm.create_agents(fin);

	// Expected testing values
	std::vector<std::vector<double>> exp_testing = {
			{9, 0.8, 1.0, (0.05+0.89)*1.0},
			{15, 0.1, 0.5, (0.05+0.89)*0.5}, 
			{50, 0.7, 0.2, (0.05+0.89)*0.2}, 
			{70, 0.32, 0.25, (0.05+0.89)*0.25}};
	
	// Time and expected counter
	double time = 0.0;
	int ctn = 0;

	for (int ti = 0; ti<=tmax; ++ti){
		// Propagate 
		abm.transmit_infection();
		
		// Check the testing
		std::vector<double> test_values = exp_testing.at(ctn);
		Testing testing = abm.get_testing_object();

		// Check testing start
		if (!testing.started(time)){
			if (time >= exp_testing.at(ctn).at(0)){
				std::cerr << "Testing did not start when it should" << std::endl;
				return false;
			}
		}	

		// Check values
		if (testing.started(time)){
			int next = std::min(ctn+1, static_cast<int>(exp_testing.size()-1));
			if (float_equality<double>(time, exp_testing.at(next).at(0), 1e-5)){
				++ctn;
				test_values = exp_testing.at(ctn);
			}
			if (!float_equality<double>(test_values.at(1), testing.get_exp_tested_prob(), 1e-5)){
				std::cerr << "Wrong exposed testing probability" << std::endl;
				return false;
			}
			if (!float_equality<double>(test_values.at(2), testing.get_sy_tested_prob(), 1e-5)){
				std::cerr << "Wrong symptomatic testing probability" << std::endl;
				return false;
			}
			if (!float_equality<double>(test_values.at(3), testing.get_prob_flu_tested(), 1e-5)){
				std::cerr << "Wrong flu testing probability" << std::endl;
				return false;
			}
		}
		time += dt;
	}
	return true;
}
