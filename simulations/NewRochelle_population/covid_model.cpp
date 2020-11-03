#include "../../include/abm.h"
#include <chrono>

/***************************************************** 
 *
 * ABM run of COVID-19 SEIR in New Rochelle, NY 
 *
 ******************************************************/

int main()
{
	// Time in days, space in km
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 600;	
	// Print agent info this many steps
	int dt_out_agents = 100; 
	// Number of initially infected
	int inf0 = 22;

	// Input files
	std::string fin("input_data/NR_agents.txt");
	std::string hfile("input_data/NR_households.txt");
	std::string sfile("input_data/NR_schools.txt");
	std::string wfile("input_data/NR_workplaces.txt");
	std::string hsp_file("input_data/NR_hospitals.txt");
	std::string rh_file("input_data/NR_retirement_homes.txt");

	// File with infection parameters
	std::string pfname("input_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dexp_name("input_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("input_data/age_dist_hospitalization.txt");
	std::string dhicu_name("input_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("input_data/age_dist_mortality.txt");
	// Map for abm loading of distributions
	std::map<std::string, std::string> dfiles = 
		{ {"exposed never symptomatic", dexp_name}, {"hospitalization", dh_name}, 
		  {"ICU", dhicu_name}, {"mortality", dmort_name} };
	// File with 	
	std::string tfname("input_data/tests_with_time.txt");

	// Output file names
	// Daily new
	std::ofstream fnew_pos("output/new_tested_pos_step.txt");
	std::ofstream fnew_neg("output/new_tested_neg_step.txt");
	std::ofstream fnew_fpos("output/new_tested_false_pos_step.txt");
	std::ofstream fnew_fneg("output/new_tested_false_neg_step.txt");
	std::ofstream fnew_tested("output/new_tested_step.txt");
	std::ofstream fnew_infected("output/new_infected_step.txt");
	// Daily total 
	std::ofstream ftot_expnsy("output/total_exp_never_sy.txt");
	std::ofstream ftot_pos("output/total_tested_pos.txt");
	std::ofstream ftot_neg("output/total_tested_neg.txt");
	std::ofstream ftot_fpos("output/total_tested_false_pos.txt");
	std::ofstream ftot_fneg("output/total_tested_false_neg.txt");
	std::ofstream ftot_tested("output/total_tested.txt");
	std::ofstream ftot_inf("output/infected_with_time.txt");
	std::ofstream ftot_active("output/active_with_time.txt");
	std::ofstream ftot_dead("output/dead_with_time.txt");
	std::ofstream ftot_nt_dead("output/not_tested_dead_with_time.txt");
	std::ofstream ftot_t_dead("output/tested_dead_with_time.txt");
	std::ofstream ftot_ih("output/home_isolated_with_time.txt");
	std::ofstream ftot_hn("output/hospitalized_with_time.txt");
	std::ofstream ftot_hicu("output/icu_with_time.txt");

	ABM abm(dt, pfname, dfiles, tfname);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	abm.create_hospitals(hsp_file);
	abm.create_retirement_homes(rh_file);

	// Then the agents
	abm.create_agents(fin, inf0);

	// Simulation
	// Data collection
	std::vector<int> new_pos(tmax+1);
	std::vector<int> new_tested(tmax+1);
	std::vector<int> new_infected(tmax+1);
	std::vector<int> new_false_pos(tmax+1);
	std::vector<int> new_false_neg(tmax+1);
	std::vector<int> new_neg(tmax+1);
	// Totals
	std::vector<int> tot_expnsy(tmax+1);
	std::vector<int> tot_pos(tmax+1);
	std::vector<int> tot_neg(tmax+1);
	std::vector<int> tot_false_pos(tmax+1);
	std::vector<int> tot_false_neg(tmax+1);
	std::vector<int> tot_tested(tmax+1);
	std::vector<int> infected_count(tmax+1);
	std::vector<int> active_count(tmax+1);
	std::vector<int> total_dead(tmax+1);
	std::vector<int> tested_dead(tmax+1);
	std::vector<int> not_tested_dead(tmax+1);
	// Current treatment totals
	std::vector<int> ih_count(tmax+1);
	std::vector<int> hn_count(tmax+1);
	std::vector<int> icu_count(tmax+1);
	std::vector<int> treatment_temp(3,0);

	// For time measurement
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (int ti = 0; ti<=tmax; ++ti){
		// Save agent information
		if (ti%dt_out_agents == 0){
			std::string fname = "output/agents_t_" + std::to_string(ti) + ".txt";
			abm.print_agents(fname);
		}
		// Collect data
		tot_tested.at(ti) = abm.get_tot_recovering_exposed();
		tot_tested.at(ti) = abm.get_total_tested();
		tot_pos.at(ti) = abm.get_total_tested_positive();
		tot_neg.at(ti) = abm.get_total_tested_negative();
		tot_false_pos.at(ti) = abm.get_total_tested_false_positive();
		tot_false_neg.at(ti) = abm.get_total_tested_false_negative();
		infected_count.at(ti) = abm.get_num_infected();
		active_count.at(ti) = abm.get_num_active_cases();
		total_dead.at(ti) = abm.get_total_dead();
		tested_dead.at(ti) = abm.get_tested_dead();
		not_tested_dead.at(ti) = abm.get_not_tested_dead();
		// Treatment
		treatment_temp = abm.get_treatment_data();
		ih_count.at(ti) = treatment_temp.at(0);	
		hn_count.at(ti) = treatment_temp.at(1);
		icu_count.at(ti) = treatment_temp.at(2);
		// Propagate 
		abm.transmit_infection();
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;

	// Collected by abm
	new_pos = abm.get_tested_positive_day();
	new_neg = abm.get_tested_negative_day();
	new_false_pos = abm.get_tested_false_positive_day();
	new_false_neg = abm.get_tested_false_negative_day();
	new_tested = abm.get_tested_day();
	new_infected = abm.get_infected_day();

	// Output data 
	// Daily new
	std::copy(new_tested.begin(), new_tested.end(), std::ostream_iterator<int>(fnew_tested, " "));
	std::copy(new_pos.begin(), new_pos.end(), std::ostream_iterator<int>(fnew_pos, " "));
	std::copy(new_neg.begin(), new_neg.end(), std::ostream_iterator<int>(fnew_neg, " "));
	std::copy(new_false_pos.begin(), new_false_pos.end(), std::ostream_iterator<int>(fnew_fpos, " "));
	std::copy(new_false_neg.begin(), new_false_neg.end(), std::ostream_iterator<int>(fnew_fneg, " "));
	std::copy(new_infected.begin(), new_infected.end(), std::ostream_iterator<int>(fnew_infected, " "));
	// Totals
	std::copy(tot_expnsy.begin(), tot_expnsy.end(), std::ostream_iterator<int>(ftot_expnsy, " "));
	std::copy(tot_tested.begin(), tot_tested.end(), std::ostream_iterator<int>(ftot_tested, " "));
	std::copy(tot_pos.begin(), tot_pos.end(), std::ostream_iterator<int>(ftot_pos, " "));
	std::copy(tot_neg.begin(), tot_neg.end(), std::ostream_iterator<int>(ftot_neg, " "));
	std::copy(tot_false_pos.begin(), tot_false_pos.end(), std::ostream_iterator<int>(ftot_fpos, " "));
	std::copy(tot_false_neg.begin(), tot_false_neg.end(), std::ostream_iterator<int>(ftot_fneg, " "));
	// Infection
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(ftot_inf, " "));
	std::copy(active_count.begin(), active_count.end(), std::ostream_iterator<int>(ftot_active, " "));
	std::copy(total_dead.begin(), total_dead.end(), std::ostream_iterator<int>(ftot_dead, " "));
	std::copy(not_tested_dead.begin(), not_tested_dead.end(), std::ostream_iterator<int>(ftot_nt_dead, " "));
	std::copy(tested_dead.begin(), tested_dead.end(), std::ostream_iterator<int>(ftot_t_dead, " "));
	// Treatment
	std::copy(ih_count.begin(), ih_count.end(), std::ostream_iterator<int>(ftot_ih, " "));
	std::copy(hn_count.begin(), hn_count.end(), std::ostream_iterator<int>(ftot_hn, " "));
	std::copy(icu_count.begin(), icu_count.end(), std::ostream_iterator<int>(ftot_hicu, " "));

	// Print total values
	std::cout << "Total number of infected agents: " << abm.get_total_infected() << "\n"
			  << "Total number of casualities: " << abm.get_total_dead() << "\n"
			  << "Total number of recovered agents: " << abm.get_total_recovered() << "\n";
}
