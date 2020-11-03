#include "integration_tests.h"

/***************************************************** 
 *
 * Test suite for checking total number of infected
 * vs. tested and tested positive 
 *
 ******************************************************/

// Tests
bool balancing_all_tested();
bool balancing_exposed_tested();
bool balancing_symptomatic_tested();
bool balancing_all_false();
bool balancing_all_true();

// Supporting functions
bool complete_test(std::string, std::string dexp_name = "test_data/age_dist_exposed_never_sy.txt");
bool complete_test_false(std::string, std::string dexp_name = "test_data/age_dist_exposed_never_sy.txt");
bool complete_test_true(std::string, std::string dexp_name = "test_data/age_dist_exposed_never_sy.txt");
ABM setup_simulations(double, std::string, int, std::string dexp_name = "test_data/age_dist_exposed_never_sy.txt");
bool check_the_sums(int, std::vector<int>, std::vector<int>, 
				std::vector<int>, std::vector<int>, ABM&, int);

int main()
{
	test_pass(balancing_all_tested(), "All agents tested");	
	test_pass(balancing_exposed_tested(), "Exposed agents tested");
	test_pass(balancing_symptomatic_tested(), "Symptomatic agents tested");
	test_pass(balancing_all_false(), "All false test results");
	test_pass(balancing_all_true(), "All true test results");
}

bool balancing_all_tested()
{
	// File with infection parameters
	std::string pfname("test_data/balances/all_tested_no_expns.txt");
	std::string dexp_name("test_data/balances/age_dist_ens_all_tested_no_expns.txt");
	return complete_test(pfname, dexp_name);
}

bool balancing_exposed_tested()
{
	// File with infection parameters
	std::string pfname("test_data/balances/only_exp_tested.txt");
	std::string dexp_name("test_data/balances/age_dist_ens_only_exp_tested.txt");
	return complete_test(pfname, dexp_name);
}

bool balancing_symptomatic_tested()
{
	// File with infection parameters
	std::string pfname("test_data/balances/only_sy_tested.txt");
	std::string dexp_name("test_data/balances/age_dist_ens_only_sy_tested.txt");
	return complete_test(pfname, dexp_name);
}

bool balancing_all_false()
{
	// File with infection parameters
	std::string pfname("test_data/balances/all_false.txt");
	std::string dexp_name("test_data/balances/age_dist_ens_all_false.txt");
	return complete_test_false(pfname, dexp_name);
}

bool balancing_all_true()
{
	// File with infection parameters
	std::string pfname("test_data/balances/all_true.txt");
	std::string dexp_name("test_data/balances/age_dist_ens_all_true.txt");
	return complete_test_true(pfname, dexp_name);
}

bool complete_test(std::string pfname, std::string dexp_name)
{
	// Model parameters
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 576;
	// Initially infected
	int infected_0 = 100;

	ABM abm = setup_simulations(dt, pfname, infected_0, dexp_name);

	// Output file names
	std::ofstream ftot_pos("output/total_tested_pos.txt");
	std::ofstream ftot_tested("output/total_tested.txt");
	std::ofstream ftot_inf("output/infected_with_time.txt");
	std::ofstream ftot_active("output/active_with_time.txt");

	// Data collection
	std::vector<int> tot_pos(tmax+2);
	std::vector<int> tot_tested(tmax+2);
	std::vector<int> infected_count(tmax+2);
	std::vector<int> active_count(tmax+2);

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){
		// Collect data
		tot_tested.at(ti) = abm.get_total_tested();
		tot_pos.at(ti) = abm.get_total_tested_positive();
		infected_count.at(ti) = abm.get_num_infected();	
		active_count.at(ti) = abm.get_num_active_cases();
		// Propagate 
		abm.transmit_infection();
	}
	// Collect data from the last step
	tot_tested.at(tmax+1) = abm.get_total_tested();
	tot_pos.at(tmax+1) = abm.get_total_tested_positive();
	infected_count.at(tmax+1) = abm.get_num_infected();	
	active_count.at(tmax+1) = abm.get_num_active_cases();

	// Runtime collected output (saved for reference)
	std::copy(tot_tested.begin(), tot_tested.end(), std::ostream_iterator<int>(ftot_tested, " "));
	std::copy(tot_pos.begin(), tot_pos.end(), std::ostream_iterator<int>(ftot_pos, " "));
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(ftot_inf, " "));
	std::copy(active_count.begin(), active_count.end(), std::ostream_iterator<int>(ftot_active, " "));

	// Collect and check
	if (!check_the_sums(tmax, tot_tested, tot_pos, infected_count, active_count, abm, infected_0)){
		return false;
	}		

	return true;
}

bool complete_test_false(std::string pfname, std::string dexp_name)
{
	// Model parameters
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 400;
	// Initially infected
	int infected_0 = 100;

	ABM abm = setup_simulations(dt, pfname, infected_0, dexp_name);

	// Output file names
	std::ofstream ftot_pos("output/total_tested_pos.txt");
	std::ofstream ftot_neg("output/total_tested_neg.txt");
	std::ofstream ftot_fpos("output/total_tested_false_pos.txt");
	std::ofstream ftot_fneg("output/total_tested_false_neg.txt");
	std::ofstream ftot_tested("output/total_tested.txt");
	std::ofstream ftot_inf("output/infected_with_time.txt");
	std::ofstream ftot_active("output/active_with_time.txt");

	// Data collection
	std::vector<int> tot_pos(tmax+1);
	std::vector<int> tot_neg(tmax+1);
	std::vector<int> tot_tested(tmax+1);
	std::vector<int> tot_false_pos(tmax+1);
	std::vector<int> tot_false_neg(tmax+1);
	std::vector<int> infected_count(tmax+1);
	std::vector<int> active_count(tmax+1);

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){
		// Propagate 
		abm.transmit_infection();
		// Collect data
		tot_tested.at(ti) = abm.get_total_tested();
		tot_pos.at(ti) = abm.get_total_tested_positive();
		tot_neg.at(ti) = abm.get_total_tested_negative();
		tot_false_pos.at(ti) = abm.get_total_tested_false_positive();
		tot_false_neg.at(ti) = abm.get_total_tested_false_negative();
		infected_count.at(ti) = abm.get_num_infected();	
		active_count.at(ti) = abm.get_num_active_cases();
	}

	// Runtime collected output (saved for reference)
	std::copy(tot_tested.begin(), tot_tested.end(), std::ostream_iterator<int>(ftot_tested, " "));
	std::copy(tot_pos.begin(), tot_pos.end(), std::ostream_iterator<int>(ftot_pos, " "));
	std::copy(tot_neg.begin(), tot_neg.end(), std::ostream_iterator<int>(ftot_neg, " "));
	std::copy(tot_false_neg.begin(), tot_false_neg.end(), std::ostream_iterator<int>(ftot_fneg, " "));
	std::copy(tot_false_pos.begin(), tot_false_pos.end(), std::ostream_iterator<int>(ftot_fpos, " "));
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(ftot_inf, " "));
	std::copy(active_count.begin(), active_count.end(), std::ostream_iterator<int>(ftot_active, " "));

	// Daily data
	// Data collection
	std::vector<int> new_pos(tmax+1);
	std::vector<int> new_neg(tmax+1);
	std::vector<int> new_fpos(tmax+1);
	std::vector<int> new_fneg(tmax+1);
	std::vector<int> new_tested(tmax+1);
	std::vector<int> new_infected(tmax+1);

	// Collected by abm
	new_pos = abm.get_tested_positive_day();
	new_neg = abm.get_tested_negative_day();
	new_fneg = abm.get_tested_false_negative_day();
	new_fpos = abm.get_tested_false_positive_day();
	new_tested = abm.get_tested_day();
	new_infected = abm.get_infected_day();
	
	// Verifications
	// 1) All positive and negative tests should be zero
	int sum_pos = std::accumulate(new_pos.begin(), new_pos.end(), 0);
	int sum_neg = std::accumulate(new_neg.begin(), new_neg.end(), 0);
	if (!((tot_pos.back() == 0) && (tot_neg.back() == 0) && (sum_pos == 0) && (sum_neg == 0))){
		std::cerr << "All true tests counts should be zero" << std::endl; 
		return false;
	}
	
	// 2) For false results - non-zero 
	int sum_fpos = std::accumulate(new_fpos.begin(), new_fpos.end(), 0);
	int sum_fneg = std::accumulate(new_fneg.begin(), new_fneg.end(), 0);
	if ((tot_false_pos.back() == 0) || (tot_false_neg.back() == 0) || (sum_fpos == 0) || (sum_fneg == 0)){
		std::cerr << "All false tests counts should be larger than zero" << std::endl;
		return false;
	}

	// 3) Total equal to daily sum
	if ((tot_false_pos.back() != sum_fpos) || (tot_false_neg.back() != sum_fneg)){
		std::cerr << "Sum of all tests not equal to total detected" << std::endl;
		return false;
	}

	return true;
}

bool complete_test_true(std::string pfname, std::string dexp_name)
{
	// Model parameters
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 400;
	// Initially infected
	int infected_0 = 100;

	ABM abm = setup_simulations(dt, pfname, infected_0, dexp_name);

	// Output file names
	std::ofstream ftot_pos("output/total_tested_pos.txt");
	std::ofstream ftot_neg("output/total_tested_neg.txt");
	std::ofstream ftot_fpos("output/total_tested_false_pos.txt");
	std::ofstream ftot_fneg("output/total_tested_false_neg.txt");
	std::ofstream ftot_tested("output/total_tested.txt");
	std::ofstream ftot_inf("output/infected_with_time.txt");
	std::ofstream ftot_active("output/active_with_time.txt");

	// Data collection
	std::vector<int> tot_pos(tmax+1);
	std::vector<int> tot_neg(tmax+1);
	std::vector<int> tot_tested(tmax+1);
	std::vector<int> tot_false_pos(tmax+1);
	std::vector<int> tot_false_neg(tmax+1);
	std::vector<int> infected_count(tmax+1);
	std::vector<int> active_count(tmax+1);

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){
		// Propagate 
		abm.transmit_infection();
		// Collect data
		tot_tested.at(ti) = abm.get_total_tested();
		tot_pos.at(ti) = abm.get_total_tested_positive();
		tot_neg.at(ti) = abm.get_total_tested_negative();
		tot_false_pos.at(ti) = abm.get_total_tested_false_positive();
		tot_false_neg.at(ti) = abm.get_total_tested_false_negative();
		infected_count.at(ti) = abm.get_num_infected();	
		active_count.at(ti) = abm.get_num_active_cases();
	}

	// Runtime collected output (saved for reference)
	std::copy(tot_tested.begin(), tot_tested.end(), std::ostream_iterator<int>(ftot_tested, " "));
	std::copy(tot_pos.begin(), tot_pos.end(), std::ostream_iterator<int>(ftot_pos, " "));
	std::copy(tot_neg.begin(), tot_neg.end(), std::ostream_iterator<int>(ftot_neg, " "));
	std::copy(tot_false_neg.begin(), tot_false_neg.end(), std::ostream_iterator<int>(ftot_fneg, " "));
	std::copy(tot_false_pos.begin(), tot_false_pos.end(), std::ostream_iterator<int>(ftot_fpos, " "));
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(ftot_inf, " "));
	std::copy(active_count.begin(), active_count.end(), std::ostream_iterator<int>(ftot_active, " "));

	// Daily data
	// Data collection
	std::vector<int> new_pos(tmax+1);
	std::vector<int> new_neg(tmax+1);
	std::vector<int> new_fpos(tmax+1);
	std::vector<int> new_fneg(tmax+1);
	std::vector<int> new_tested(tmax+1);
	std::vector<int> new_infected(tmax+1);

	// Collected by abm
	new_pos = abm.get_tested_positive_day();
	new_neg = abm.get_tested_negative_day();
	new_fneg = abm.get_tested_false_negative_day();
	new_fpos = abm.get_tested_false_positive_day();
	new_tested = abm.get_tested_day();
	new_infected = abm.get_infected_day();
	
	// Verifications
	// 1) All positive and negative tests should be non-zero
	int sum_pos = std::accumulate(new_pos.begin(), new_pos.end(), 0);
	int sum_neg = std::accumulate(new_neg.begin(), new_neg.end(), 0);
	if (((tot_pos.back() == 0) && (tot_neg.back() == 0) && (sum_pos == 0) && (sum_neg == 0))){
		std::cerr << "All true tests counts should larger than zero" << std::endl;
		return false;
	}
	
	// 2) For false results - zero 
	int sum_fpos = std::accumulate(new_fpos.begin(), new_fpos.end(), 0);
	int sum_fneg = std::accumulate(new_fneg.begin(), new_fneg.end(), 0);
	if (!((tot_false_pos.back() == 0) || (tot_false_neg.back() == 0) || (sum_fpos == 0) || (sum_fneg == 0))){
		std::cerr << "All false tests counts should be zero" << std::endl;
		return false;
	}

	// 3) Total equal to daily sum
	if ((tot_pos.back() != sum_pos) || (tot_neg.back() != sum_neg)){
		std::cerr << "Sum of all tests not equal to total detected" << std::endl;
		return false;
	}

	return true;
}

// Initialize ABM simulations
ABM setup_simulations(double dt, std::string pfname, int inf0, std::string dexp_name)
{
	// Input files
	std::string fin("test_data/NR_agents.txt");
	std::string hfile("test_data/NR_households.txt");
	std::string sfile("test_data/NR_schools.txt");
	std::string wfile("test_data/NR_workplaces.txt");
	std::string hsp_file("test_data/NR_hospitals.txt");
	std::string rh_file("test_data/NR_retirement_homes.txt");

	// Files with age-dependent distributions
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
	abm.create_agents(fin, inf0);

	return abm;
}

bool check_the_sums(int tmax, std::vector<int> tot_tested, std::vector<int> tot_pos, 
				std::vector<int> infected_count, std::vector<int> active_count,
				ABM& abm, int infected_0)
{
	// Names are common
	std::ofstream fnew_pos("output/new_tested_pos_step.txt");
	std::ofstream fnew_tested("output/new_tested_step.txt");
	std::ofstream fnew_infected("output/new_infected_step.txt");

	// Data collection
	std::vector<int> new_pos(tmax+1);
	std::vector<int> new_tested(tmax+1);
	std::vector<int> new_infected(tmax+1);

	// Collected by abm
	new_pos = abm.get_tested_positive_day();
	new_tested = abm.get_tested_day();
	new_infected = abm.get_infected_day();

	// Output data (for reference)
	std::copy(new_tested.begin(), new_tested.end(), std::ostream_iterator<int>(fnew_tested, " "));
	std::copy(new_pos.begin(), new_pos.end(), std::ostream_iterator<int>(fnew_pos, " "));
	std::copy(new_infected.begin(), new_infected.end(), std::ostream_iterator<int>(fnew_infected, " "));

	// Tests
	// 1) Infected need to be equal to tested positive
	int sum_dpos = std::accumulate(new_pos.begin(), new_pos.end(), 0);
	int sum_dinf = std::accumulate(new_infected.begin(), new_infected.end(), 0);
	int sum_dtested = std::accumulate(new_tested.begin(), new_tested.end(), 0);	
	// In case of all exposed recovering, initially infected will not be tested and 
	// counted into sum_dpos so they should be excluded
	bool equal_pos_inf = ((sum_dpos == sum_dinf+infected_0) || (sum_dpos == sum_dinf)) 
			&& (sum_dpos == tot_pos.back());

	if (equal_pos_inf == false){
		std::cerr << "Total infected balance not equal expected " << std::endl;
		return false;
	}
	
	// 2) Number of tested equal or more than tested pos (or infected total)
	if ((sum_dtested < sum_dpos) || (tot_tested.back() < sum_dpos)){
		std::cerr << "Number of tested equal or more than tested pos (or infected total)" << std::endl;
		return false;
	}

	// 3) Total tested = sum of daily tested
	if (!(sum_dtested == tot_tested.back())){
		std::cerr << "Mismatch in tracked total tested" << std::endl;
		return false;
	}

	// 4) Infected and active counts
	int all_inf = std::accumulate(infected_count.begin(), infected_count.end(), 0);
	int all_act = std::accumulate(active_count.begin(), active_count.end(), 0);
	if ((sum_dinf + infected_0 > all_inf) || 
			(sum_dinf + infected_0 > all_act) || (all_act > all_inf)){
		std::cerr << "Mismatch in tracked infected and active" << std::endl;
		return false;
	}

	// 5) Recovered and dead equal to total infected
	if ((abm.get_total_recovered() + abm.get_total_dead()) != abm.get_total_infected())	{
		std::cerr << "Mismatch in recovered and dead balance" << std::endl;
		return false;
	}

	return true;
}


