#include "abm_tests.h"

/***************************************************** 
 *
 * Test suite for creation of ABM objects
 *
 ******************************************************/

// Tests
bool create_households_test();
bool create_schools_test();
bool wrong_school_type_test();
bool create_workplaces_test();
bool create_hospitals_test();
bool create_retirement_homes_test();
bool create_agents_test();
bool create_agents_file_test();

// Supporting functions
bool compare_places_files(std::string fname_in, std::string fname_out, 
				const std::vector<double> infection_parameters, const bool has_type = false);
bool compare_agents_files(std::string fname_in, std::string fname_out);
bool correctly_registered(const ABM, const std::vector<std::vector<int>>, 
							const std::vector<std::vector<int>>, std::string, 
							const std::string, const std::string, const int);
bool check_initially_infected(const Agent& agent, const Flu& flu, int& n_exposed_never_sy,
								const std::map<std::string, double> infection_parameters);
bool check_fractions(int, int, double, std::string);

int main()
{
	test_pass(create_households_test(), "Household creation");
	test_pass(create_schools_test(), "School creation");
	test_pass(wrong_school_type_test(), "Wrong school type detection");
	test_pass(create_workplaces_test(), "Workplace creation");
	test_pass(create_hospitals_test(), "Hospitals creation");
	test_pass(create_retirement_homes_test(), "Retirement homes creation");
	test_pass(create_agents_test(), "Agent creation");
	test_pass(create_agents_file_test(), "Agent creation - file test");
}

// Checks household creation from file
bool create_households_test()
{
	// Create households and save the information
	std::string fin("test_data/NR_households.txt");
	std::string fout("test_data/houses_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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

	abm.create_households(fin);
	abm.print_households(fout);
	
	// Vector of infection parameters 
	// (order as in output file: ck, beta,  alpha, beta_ih)
	std::vector<double> infection_parameters = {2.0, 0.69, 0.80, 0.48}; 

	// Check if correct, hardcoded for places properties
	if (!compare_places_files(fin, fout, infection_parameters)){
		std::cerr << "Error in household creation" << std::endl;
		return false;
	}
	return true;
}

// Checks school creation from file
bool create_schools_test()
{
	// Create schools and save the information
	std::string fin("test_data/NR_schools.txt");
	std::string fout("test_data/schools_out.txt");

	double dt = 1.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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

	abm.create_schools(fin);
	abm.print_schools(fout);

	bool has_type = true;
	// Vector of infection parameters 
	// (order as in output file: ck, beta, beta for employee, psi for employee, 
	// psi for middle)
	std::vector<double> infection_parameters = {2.0, 1.33, 0.66, 0.2, 0.1}; 

	// Check if correct, hardcoded for places properties
	if (!compare_places_files(fin, fout, infection_parameters, has_type)){
		std::cerr << "Error in school creation" << std::endl;
		return false;
	}
	return true;
}

// Tests if exception is triggered upon entering a wrong school type
bool wrong_school_type_test()
{
	bool verbose = true;
	const std::invalid_argument invarg("Wrong type");

	std::string fin("test_data/schools_wrong_type.txt");
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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
	
	if (!exception_test(verbose, &invarg, &ABM::create_schools, abm, fin)){
		std::cerr << "Wrong school type not recognized as an error" << std::endl;
		return false;
	}
	return true;
}

// Checks workplace creation from file
bool create_workplaces_test()
{
	// Create workplaces and save the information
	std::string fin("test_data/NR_workplaces.txt");
	std::string fout("test_data/workplaces_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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

	abm.create_workplaces(fin);
	abm.print_workplaces(fout);
	
	// Vector of infection parameters 
	// (order as in output file: ck, beta, psi)
	std::vector<double> infection_parameters = {2.0, 0.66, 0.1}; 

	// Check if correct, hardcoded for places properties
	if (!compare_places_files(fin, fout, infection_parameters)){
		std::cerr << "Error in workplace creation" << std::endl;
		return false;
	}
	return true;	
}

// Checks hospital creation from file
bool create_hospitals_test()
{
	// Create workplaces and save the information
	std::string fin("test_data/NR_hospitals.txt");
	std::string fout("test_data/hospitals_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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

	abm.create_hospitals(fin);
	abm.print_hospitals(fout);
	
	// Vector of infection parameters 
	// (order as in output file: ck, betas for each category)
	std::vector<double> infection_parameters = {2.0, 1.28, 1.38, 1.75, 1.02, 1.34}; 

	// Check if correct, hardcoded for places properties
	if (!compare_places_files(fin, fout, infection_parameters)){
		std::cerr << "Error in hospital creation" << std::endl;
		return false;
	}
	return true;	
}

// Checks retirement home creation
bool create_retirement_homes_test()
{
	// Create workplaces and save the information
	std::string fin("test_data/NR_retirement_homes.txt");
	std::string fout("test_data/ret_homes_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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

	abm.create_retirement_homes(fin);
	abm.print_retirement_home(fout);

	// Vector of infection parameters 
	// (order as in output file: ck, beta resident, beta employee, beta
	// home isolated, psi employee)
	std::vector<double> infection_parameters = {2.0, 0.69, 0.66, 0.48, 0.0}; 

	// Check if correct, hardcoded for places properties
	if (!compare_places_files(fin, fout, infection_parameters)){
		std::cerr << "Error in retirement home creation" << std::endl;
		return false;
	}
	return true;
}

// Checks agents creation from file including proper 
// distribution into places 
bool create_agents_test()
{
	double dt = 0.25;
	int inf0 = 763, inf0_count = 0;
	int n_exposed_never_sy = 0;
	// Input files
	std::string fin("test_data/NR_agents.txt");
	std::string hfile("test_data/NR_households.txt");
	std::string sfile("test_data/NR_schools.txt");
	std::string wfile("test_data/NR_workplaces.txt");
	std::string hsp_file("test_data/NR_hospitals.txt");
	std::string rh_file("test_data/NR_retirement_homes.txt");

	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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
	abm.create_agents(fin, inf0);

	const std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
	const std::vector<Household>& households = abm.vector_of_households();
	const std::vector<School>& schools = abm.vector_of_schools();
	const std::vector<Workplace>& workplaces = abm.vector_of_workplaces();
	const std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	const std::vector<RetirementHome>& retirement_homes = abm.vector_of_retirement_homes();

	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 
	Flu& flu = abm.get_flu_object();

	// Check registration and count initially infected as created by ABM
	for (const auto& agent : agents){ 
		const int aID = agent.get_ID();
		if (agent.infected()){
			++inf0_count;
			if (!check_initially_infected(agent, flu, n_exposed_never_sy, infection_parameters)){
				std::cerr << "Error in initialization of infected agent" << std::endl;
				return false;	
			}	
		} 
		if (agent.student()){
			if (!find_in_place<School>(schools, aID, agent.get_school_ID())){
				std::cerr << "Agent not registered in a school" << std::endl;
				return false;
			}
		}
		if (agent.works()){
			if (agent.school_employee()){
				if (!find_in_place<School>(schools, aID, agent.get_work_ID())){
					std::cerr << "Agent not registered in a school as an employee" << std::endl;
					return false;
				}
			}else if (agent.retirement_home_employee()){
				if (!find_in_place<RetirementHome>(retirement_homes, aID, agent.get_work_ID())){
					std::cerr << "Agent not registered in a retirement home as an employee" << std::endl;
					return false;
				}		
			} else {
				if (!find_in_place<Workplace>(workplaces, aID, agent.get_work_ID())){
					std::cerr << "Agent not registered in a workplace" << std::endl;
					return false;
				}
			}
		}
		// Household or retirement home
		if (agent.retirement_home_resident()){
			if (!find_in_place<RetirementHome>(retirement_homes, aID, agent.get_household_ID())){
				std::cerr << "Agent not registered in a retirement home" << std::endl;
				return false;
			}	
		} else if (agent.hospital_non_covid_patient()){
			if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
				std::cerr << "Hospital patient without covid is not registered in a hospital" << std::endl;
				return false;
			}
		} else {
			if (!find_in_place<Household>(households, aID, agent.get_household_ID())){
				std::cerr << "Agent not registered in a household" << std::endl;
				return false;
			}
		}
		// Hospital employee
		if (agent.hospital_employee()){
			if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
				std::cerr << "Hospital employee is not registered in a hospital" << std::endl;
				return false;
			}
		}
	}
	if (inf0 != inf0_count){
		std::cerr << "Initially infected agent numbers don't match expectations" << std::endl;
		return false;
	}
	// Fraction that never develops symptoms
	if (!check_fractions(n_exposed_never_sy, inf0_count, infection_parameters.at("fraction exposed never symptomatic"), 
							"Wrong percentage of exposed agents never developing symptoms")){
		return false;
	}
	return true;
}

// Checks agents creation from file including proper 
// distribution into places - version with agents initialized as infected from file 
bool create_agents_file_test()
{
	double dt = 0.25;
	int inf0 = 1321, inf0_count = 0;
	int n_exposed_never_sy = 0;
	// Input files
	std::string fin("test_data/file_test_agents.txt");
	std::string hfile("test_data/NR_households.txt");
	std::string sfile("test_data/NR_schools.txt");
	std::string wfile("test_data/NR_workplaces.txt");
	std::string hsp_file("test_data/NR_hospitals.txt");
	std::string rh_file("test_data/NR_retirement_homes.txt");

	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
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
	abm.create_agents(fin);

	const std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
	const std::vector<Household>& households = abm.vector_of_households();
	const std::vector<School>& schools = abm.vector_of_schools();
	const std::vector<Workplace>& workplaces = abm.vector_of_workplaces();
	const std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	const std::vector<RetirementHome>& retirement_homes = abm.vector_of_retirement_homes();

	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 
	Flu& flu = abm.get_flu_object();

	// Load the agent file too and check correspondence of the entries
	std::ifstream input(fin);
	//std::ifstream input("test_data/break_file_test.txt");
	// Check registration and count initially infected as created by ABM
	for (const auto& agent : agents){ 
		const int aID = agent.get_ID();
		
		// Read and parse a line from file that should correspond to this agent
		std::vector<std::string> line(15);
		for (auto& entry : line){
			input >> entry;
		}
				
		if (agent.infected()){
			++inf0_count;
			if (line.at(14) != "1"){
				std::cerr << "This agent was not supposed to be infected" << std::endl;
				return false;
			}
			if (!check_initially_infected(agent, flu, n_exposed_never_sy, infection_parameters)){
				std::cerr << "Error in initialization of infected agent" << std::endl;
				return false;	
			}	
		}

		if (agent.student()){
			if (line.at(0) != "1"){
				std::cerr << "This agent was not supposed to be a student" << std::endl;
				return false;
			}
				if (agent.get_school_ID() != std::stoi(line.at(7))){
					std::cerr << "Loaded school and ID from file don't match" << std::endl;
					return false;
				}
			if (!find_in_place<School>(schools, aID, agent.get_school_ID())){
				std::cerr << "Agent not registered in a school" << std::endl;
				return false;
			}
		}

		if (agent.works()){
			if (agent.school_employee()){
				if (line.at(10) != "1"){
					std::cerr << "This agent was not supposed to work at a school" << std::endl;
					return false;
				}
				if (agent.get_work_ID() != std::stoi(line.at(11))){
					std::cerr << "Loaded school as workplace ID and ID from file don't match" << std::endl;
					return false;
				}
				if (!find_in_place<School>(schools, aID, agent.get_work_ID())){
					std::cerr << "Agent not registered in a school as an employee" << std::endl;
					return false;
				}
			}else if (agent.retirement_home_employee()){
				if (line.at(9) != "1"){
					std::cerr << "This agent was not supposed to work at a retirement home" << std::endl;
					return false;
				}
				if (agent.get_work_ID() != std::stoi(line.at(11))){
					std::cerr << "Loaded RH as a workplace ID and ID from file don't match" << std::endl;
					return false;
				}
				if (!find_in_place<RetirementHome>(retirement_homes, aID, agent.get_work_ID())){
					std::cerr << "Agent not registered in a retirement home as an employee" << std::endl;
					return false;
				}		
			} else {
				if (line.at(1) != "1"){
					std::cerr << "This agent was not supposed to work at a general workplace" << std::endl;
					return false;
				}
				if (agent.get_work_ID() != std::stoi(line.at(11))){
					std::cerr << "Loaded workplace ID and ID from file don't match" << std::endl;
					return false;
				}
				if (!find_in_place<Workplace>(workplaces, aID, agent.get_work_ID())){
					std::cerr << "Agent not registered in a workplace" << std::endl;
					return false;
				}
			}
		}

		// Household or retirement home
		if (agent.retirement_home_resident()){
			if (line.at(8) != "1"){
				std::cerr << "This agent was not supposed to live in a retirement home" << std::endl;
				return false;
			}
			if (agent.get_household_ID() != std::stoi(line.at(5))){
				std::cerr << "Loaded RH ID (as residence) and ID from file don't match" << std::endl;
				return false;
			}
			if (!find_in_place<RetirementHome>(retirement_homes, aID, agent.get_household_ID())){
				std::cerr << "Agent not registered in a retirement home" << std::endl;
				return false;
			}	
		} else if (agent.hospital_non_covid_patient()){
			if (line.at(6) != "1"){
				std::cerr << "This agent was not supposed to be a non-COVID hospital patient" << std::endl;
				return false;
			}
			if (agent.get_hospital_ID() != std::stoi(line.at(13))){
				std::cerr << "Loaded hospital ID and ID from file don't match" << std::endl;
				return false;
			}
			if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
				std::cerr << "Hospital patient without covid is not registered in a hospital" << std::endl;
				return false;
			}
		} else {
			if (agent.get_household_ID() != std::stoi(line.at(5))){
				std::cerr << "Loaded household ID and ID from file don't match" << std::endl;
				return false;
			}
			if (!find_in_place<Household>(households, aID, agent.get_household_ID())){
				std::cerr << "Agent not registered in a household" << std::endl;
				return false;
			}
		}

		// Hospital employee
		if (agent.hospital_employee()){
			if (line.at(12) != "1"){
				std::cerr << "This agent was not supposed to work at a hospital" << std::endl;
				return false;
			}
			if (agent.get_hospital_ID() != std::stoi(line.at(13))){
				std::cerr << "Hospital employee: loaded hospital ID and ID from file don't match" << std::endl;
				return false;
			}
			if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
				std::cerr << "Hospital employee is not registered in a hospital" << std::endl;
				return false;
			}
		}
	}

	if (inf0 != inf0_count){
		std::cerr << "Initially infected agent numbers don't match expectations" << std::endl;
		return false;
	}
	// Fraction that never develops symptoms
	if (!check_fractions(n_exposed_never_sy, inf0_count, infection_parameters.at("fraction exposed never symptomatic"), 
							"Wrong percentage of exposed agents never developing symptoms")){
		return false;
	}
	return true;
}

// Test suite for agents that are infected at intialization
bool check_initially_infected(const Agent& agent, const Flu& flu, int& n_exposed_never_sy,
								const std::map<std::string, double> infection_parameters)
{
	if (!agent.exposed() || agent.tested() || agent.home_isolated()){
		std::cerr << "Initially infected agent should be exposed and not tested" << std::endl;
		return false;
	}
	// Flu
	const int aID = agent.get_ID();
	const std::vector<int> flu_susceptible = flu.get_susceptible_IDs();
	for (const auto& afs : flu_susceptible){
		if (afs == aID){
			std::cerr << "Infected agent should not be part of future flu poll" << std::endl; 
			return false;
		}
	}
	const std::vector<int> flu_agents = flu.get_flu_IDs();
	for (const auto& afs : flu_agents){
		if (afs == aID){
			std::cerr << "Infected agent should not be part of flu group" << std::endl; 
			return false;
		}
	}
	if (agent.recovering_exposed()){
		++n_exposed_never_sy;
		// Recovery time 
		if (agent.get_latency_end_time() < infection_parameters.at("recovery time")){
			std::cerr << "Wrong latency of agent that will not develop symptoms" << std::endl; 
			return false;
		}
	}
	// Infectiousness variability
	if (agent.get_inf_variability_factor() < 0){
		std::cerr << "Agent's infectiousness variability out of range: " << agent.get_inf_variability_factor() << std::endl;
		return false;
	}
	// Latency and non-infectious period
	if ((agent.get_infectiousness_start_time() > agent.get_latency_end_time()) || 
			(agent.get_latency_end_time() < 0)){
		std::cerr << "Agent's non-infectious period or latency out of range" << std::endl;
		return false;
	}
	return true;
}

/// \brief Compare input places file with output from a Place object 
bool compare_places_files(std::string fname_in, std::string fname_out, 
				const std::vector<double> infection_parameters, const bool has_type)
{
	std::ifstream input(fname_in);
	std::ifstream output(fname_out);

	// Load the first file as vectors
	std::vector<int> in_IDs;
	std::vector<double> in_coords_x, in_coords_y;

	int ID = 0;
	double x = 0.0, y = 0.0;
	std::string type = {};
	if (has_type == false){
		while (input >> ID >> x >> y){
			in_IDs.push_back(ID);
			in_coords_x.push_back(x);
			in_coords_y.push_back(y);
		}
	}else{
		// For places that have types
		while (input >> ID >> x >> y >> type){
			in_IDs.push_back(ID);
			in_coords_x.push_back(x);
			in_coords_y.push_back(y);
		}
	}

	// Now load the second (output) and compare
	// Also check if total number of agents and infected agents is 0
	int num_ag = 0, num_inf = 0;
	int num_ag_exp = 0, num_inf_exp = 0;
	int ind = 0;
	double parameter;

	while (output >> ID >> x >> y >> num_ag >> num_inf){
		// Compare ID, location, agents
		if (ID != in_IDs.at(ind)){
			std::cerr << "Wrong place ID" << std::endl;
			return false;
		}
		if (!float_equality<double>(x, in_coords_x.at(ind), 1e-5)){
			std::cerr << "Wrong x coordinates" << std::endl;
			return false;
		}
		if (!float_equality<double>(y, in_coords_y.at(ind), 1e-5)){
			std::cerr << "Wrong y coordinatesD" << std::endl;
			return false;
		}
		if (num_ag != num_ag_exp){
			std::cerr << "Wrong number of agents" << std::endl;
			return false;
		}
		if (num_inf != num_inf_exp){
			std::cerr << "Wrong number of infected agents" << std::endl;
			return false;
		}
		// Compare infection parameters
		for (auto const& expected_parameter : infection_parameters){
			output >> parameter;
			if (!float_equality<double>(expected_parameter, parameter, 1e-5)){
				std::cerr << "Wrong infection transmission parameter" << std::endl;
				return false;
			}
		}
		++ind;
	}

	// In case file empty, shorter, or doesn't exist
	if (in_IDs.size() != ind){
		std::cerr << "Wrong number of locations" << std::endl;
		return false;
	}
	return true;	
}

/// \brief Compare input agent file with output from an Agent object 
bool compare_agents_files(std::string fname_in, std::string fname_out)
{
	std::ifstream input(fname_in);
	std::ifstream output(fname_out);

	// Load the first file as vectors and generate IDs
	std::vector<int> IDs;
	// student, works, hospital staff, hospital patient
	std::vector<std::vector<bool>> status;
	std::vector<bool> infected;
	std::vector<int> age;
	std::vector<std::vector<double>> position;
	// house ID, school ID, work ID, hospital ID
	std::vector<std::vector<int>> places;		 
	
	int ID = 0, yrs = 0, hID = 0, sID = 0, wID = 0, hspID = 0;
	bool studies = false, works = false, is_infected = false;
	bool works_at_hospital = false, non_covid_patient = false;
	double x = 0.0, y = 0.0;

	while (input >> studies >> works >> yrs 
			>> x >> y >> hID >> non_covid_patient >> sID >> wID 
			>> works_at_hospital >> hspID >> is_infected)
	{
		IDs.push_back(++ID);
		status.push_back({studies, works, works_at_hospital, non_covid_patient});
		infected.push_back(is_infected);
		age.push_back(yrs);
		position.push_back({x, y});
		places.push_back({hID, sID, wID, hspID});
	}

	// Now load the second (output) file and compare
	int ind = 0;
	while (output >> ID >> studies >> works >> yrs 
			>> x >> y >> hID >> non_covid_patient >> sID >> wID 
			>> works_at_hospital >> hspID >> is_infected)
	{
		if (ID != IDs.at(ind))
			return false;
		if (studies != status.at(ind).at(0) 
				|| works != status.at(ind).at(1)
				|| works_at_hospital != status.at(ind).at(2)
				|| non_covid_patient != status.at(ind).at(3))
			return false;
		if (is_infected != infected.at(ind))
			return false;
		if (yrs != age.at(ind))
			return false;
		if (!float_equality<double>(x, position.at(ind).at(0), 1e-5))
			return false;
		if (!float_equality<double>(y, position.at(ind).at(1), 1e-5))
			return false;
		if (hID != places.at(ind).at(0) 
				|| sID != places.at(ind).at(1) 
				|| wID != places.at(ind).at(2)
				|| hspID != places.at(ind).at(3))
			return false;
		++ind;
	}

	// In case file empty, shorter, or doesn't exist
	//if (IDs.size() != ind)
	//	return false;
	return true;	
}

/**
 * \brief Check if agents registered correctly in a given Place
 *
 * @param abm - an ABM object
 * @param place_info - vector of expected total number of agents in each place and total infected
 * @param place_agents - vector with IDs of agents in each place
 * @param place_type - type of place (house, school or work), case independent
 * @param info_file - name of the file to save basic info output to 
 * @param agent_file - name of the file to save agent IDs to
 * @param num_red_args - number of redundant arguments (i.e. infection paramters) in printed
 */ 
bool correctly_registered(const ABM abm, const std::vector<std::vector<int>> place_info, 
							const std::vector<std::vector<int>> place_agents, std::string place_type, 
							const std::string info_file, const std::string agent_file, const int num_red_args)
{

	// Save basic info and agent IDs
	place_type = str_to_lower(place_type);
	if (place_type == "house"){
		abm.print_households(info_file);
		abm.print_agents_in_households(agent_file);
	} else if (place_type == "school"){
		abm.print_schools(info_file);
		abm.print_agents_in_schools(agent_file);
	} else if (place_type == "workplace"){
		abm.print_workplaces(info_file);
		abm.print_agents_in_workplaces(agent_file);
	} else if (place_type == "hospital"){
		abm.print_hospitals(info_file);
		abm.print_agents_in_hospitals(agent_file);
	} else{
		std::cout << "Wrong place type" << std::endl;
		return false; 
	}
	
	// Check if total number of agents and infected agents are correct
	std::ifstream info_total(info_file);
	int ID = 0, num_agents = 0, num_inf = 0;
	double x = 0.0, y = 0.0, not_needed_arg = 0.0;
	int ind = 0;
	while (info_total >> ID >> x >> y >> num_agents >> num_inf){
		// Ignore remaining 
		for (int i=0; i<num_red_args; ++i)
			info_total >> not_needed_arg;

		if (num_agents != place_info.at(ind).at(0))
			return false;
		if (num_inf != place_info.at(ind).at(1))
			return false;		
		++ind;
	}

	// Check if correct agent IDs
	// Load ID file into a nested vector, one inner vector per place
	std::vector<std::vector<int>> saved_IDs;
	std::ifstream info_IDs(agent_file);
	std::string line;
	while (std::getline(info_IDs, line))
	{	
		std::istringstream iss(line);
		std::vector<int> place_IDs;
		while (iss >> ID)
			place_IDs.push_back(ID);
		saved_IDs.push_back(place_IDs);
	}

	// Compare the vectors
	return is_equal_exact<int>(place_agents, saved_IDs);
}

/// Check if num1/num2 is roughly equal to expected
bool check_fractions(int num1, int num2, double fr_expected, std::string msg)
{
	double fr_tested = static_cast<double>(num1)/static_cast<double>(num2);
	if (!float_equality<double>(fr_tested, fr_expected, 0.1)){
		std::cout << msg << std::endl;
		std::cout << "Computed: " << fr_tested << " Expected: " << fr_expected << std::endl; 
		return false;
	}
	return true;
}


