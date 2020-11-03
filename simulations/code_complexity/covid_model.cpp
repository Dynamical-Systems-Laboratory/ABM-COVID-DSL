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
	int tmax = 100;	
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

	// For time measurement
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (int ti = 0; ti<=tmax; ++ti){
		abm.transmit_infection();
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;

	// Print total values
	std::cout << "Total number of infected agents: " << abm.get_total_infected() << "\n"
			  << "Total number of casualities: " << abm.get_total_dead() << "\n"
			  << "Total number of recovered agents: " << abm.get_total_recovered() << "\n";
}
