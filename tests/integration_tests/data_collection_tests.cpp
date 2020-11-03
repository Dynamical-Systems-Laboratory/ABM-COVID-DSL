#include "integration_tests.h"

/***************************************************** 
 *
 * Test suite for checking the data collection 
 * functionality  
 *
 ******************************************************/

// Tests
bool proper_start_tests();

// Supporting functions
ABM setup_simulations(double, std::string, int);
bool check_length(const int, const std::vector<int>&);
bool check_end_values(const std::vector<int>&, const std::vector<int>&);

int main()
{
	test_pass(proper_start_tests(), "Data collection");	
}

bool proper_start_tests()
{
	// Model parameters
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 306;
	// Initially infected
	int infected_0 = 10;
	// Expected time to start collection
	int t_exp = 45;

	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");

	ABM abm = setup_simulations(dt, pfname, infected_0);

	// Output file names
	std::ofstream ftot_pos("output/total_tested_pos.txt");
	std::ofstream ftot_tested("output/total_tested.txt");
	std::ofstream ftot_inf("output/infected_with_time.txt");
	std::ofstream ftot_active("output/active_with_time.txt");

	// Data collection
	std::vector<int> tot_pos;
	std::vector<int> tot_tested;
	std::vector<int> tot_neg;
	std::vector<int> tot_false_pos;
	std::vector<int> tot_false_neg;
	std::vector<int> infected_count;
	std::vector<int> active_count;
    std::vector<int> total_dead;
	std::vector<int> tested_dead;
	std::vector<int> not_tested_dead;

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){
		abm.transmit_infection();
		// Collect data
		if (abm.get_time() >= t_exp){
			tot_tested.push_back(abm.get_total_tested());
			tot_pos.push_back(abm.get_total_tested_positive());
			tot_neg.push_back(abm.get_total_tested_negative());
			tot_false_pos.push_back(abm.get_total_tested_false_positive());
			tot_false_neg.push_back(abm.get_total_tested_false_negative());
			infected_count.push_back(abm.get_num_infected());	
			active_count.push_back(abm.get_num_active_cases());
			total_dead.push_back(abm.get_total_dead());
        	tested_dead.push_back(abm.get_tested_dead());
		    not_tested_dead.push_back(abm.get_not_tested_dead());
		} else {
			// All but real infected should be 0, 
			// testing doesn't start until then either
			if (abm.get_total_tested() != 0){
				std::cerr << "Testing tracked before data collection begins" << std::endl;
				return false;
			}
			if (abm.get_total_tested_positive() != 0){
				std::cerr << "Testing of positive tests tracked before data collection begins" << std::endl;
				return false;
			}
			if (abm.get_total_tested_negative() != 0){
				std::cerr << "Testing of negative tests tracked before data collection begins" << std::endl;
				return false;
			}
			if (abm.get_total_tested_false_positive() != 0){
				std::cerr << "Testing of false positive tests tracked before data collection begins" << std::endl;
				return false;
			}
			if (abm.get_total_tested_false_negative() != 0){
				std::cerr << "Testing of false negative tests tracked before data collection begins" << std::endl;
				return false;
			}
			if (abm.get_num_infected() == 0){
				std::cerr << "Testing of true infected should start at the begining" << std::endl;
				return false;
			}
			if (abm.get_num_active_cases() != 0){
				std::cerr << "Testing of detected active cases tracked before data collection begins" << std::endl;
				return false;
			}	
		}
	}

	// Runtime collected output (saved for reference)
	std::copy(tot_tested.begin(), tot_tested.end(), std::ostream_iterator<int>(ftot_tested, " "));
	std::copy(tot_pos.begin(), tot_pos.end(), std::ostream_iterator<int>(ftot_pos, " "));
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(ftot_inf, " "));
	std::copy(active_count.begin(), active_count.end(), std::ostream_iterator<int>(ftot_active, " "));

	// Lengths of the data collected by the program
	int exp_len = (abm.get_time() - t_exp )*4;
	if (!check_length(exp_len, abm.get_tested_positive_day())){
		std::cerr << "Tested positive - size of collected data mismatch" << std::endl;
		return false;
	}
	if (!check_length(exp_len, abm.get_tested_false_positive_day())){
		std::cerr << "Tested false positive - size of collected data mismatch" << std::endl;
		return false;
	}
	if (!check_length(exp_len, abm.get_tested_negative_day())){
		std::cerr << "Tested negative - size of collected data mismatch" << std::endl;
		return false;
	}
	if (!check_length(exp_len, abm.get_tested_false_negative_day())){
		std::cerr << "Tested false negative - size of collected data mismatch" << std::endl;
		return false;
	}
	if (!check_length(exp_len, abm.get_tested_day())){
		std::cerr << "Tested - size of collected data mismatch" << std::endl;
		return false;
	}
		
	// Sum of the second should equal last value in the first 
	if (!check_end_values(tot_tested, abm.get_tested_day())){
		std::cerr << "Total tested - collected values mismatch" << std::endl;
		return false;
	}
	if (!check_end_values(tot_pos, abm.get_tested_positive_day())){
		std::cerr << "Tested positive - collected values mismatch" << std::endl;
		return false;
	}
	if (!check_end_values(tot_false_pos, abm.get_tested_false_positive_day())){
		std::cerr << "Tested false positive - collected values mismatch" << std::endl;
		return false;
	}
	if (!check_end_values(tot_neg, abm.get_tested_negative_day())){
		std::cerr << "Tested negative - collected values mismatch" << std::endl;
		return false;
	}
	if (!check_end_values(tot_false_neg, abm.get_tested_false_negative_day())){
		std::cerr << "Tested false negative - collected values mismatch" << std::endl;
		return false;
	}
	if (tested_dead.back() + not_tested_dead.back() != total_dead.back()){
		std::cerr << "Mortality of tested and untested agents not equal total" << std::endl;
		return false;
	}
	return true;
}

// Initialize ABM simulations
ABM setup_simulations(double dt, std::string pfname, int inf_0)
{
	// Input files
	std::string fin("test_data/NR_agents.txt");
	std::string hfile("test_data/NR_households.txt");
	std::string sfile("test_data/NR_schools.txt");
	std::string wfile("test_data/NR_workplaces.txt");
	std::string hsp_file("test_data/NR_hospitals.txt");
	std::string rh_file("test_data/NR_retirement_homes.txt");

	// Files with age-dependent distributions
	std::string dexp_name("test_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distributions
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
	abm.create_hospitals(hsp_file);
	abm.create_retirement_homes(rh_file);

	// Then the agents
	abm.create_agents(fin, inf_0);

	return abm;
}

/// See if non-zzero vector length corresponds to expected
bool check_length(const int exp, const std::vector<int>& vec)
{
	const auto iter = std::find_if_not(vec.begin(), vec.end(), [](int x){ return x == 0; });
	if ((vec.end()-iter) > exp){
		return false;
	}
	return true;
}

/// Compare last value of v1 with sum of v2
bool check_end_values(const std::vector<int>& v1, const std::vector<int>& v2)
{
	int v2_sum = std::accumulate(v2.begin(), v2.end(), 0);
	if (v1.back() != v2_sum){
		return false;
	}
	return true;
}
