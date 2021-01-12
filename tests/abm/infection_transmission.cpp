#include "abm_tests.h"

/***************************************************** 
 *
 * Test suite for infection related computations
 *
******************************************************/

// Tests
bool abm_contributions_test();
bool abm_events_test();
bool abm_time_dependent_testing();
bool abm_vaccination();

// Supporting functions
bool abm_vaccination_random();
bool abm_vaccination_group();
ABM create_abm(const double dt, int i0);

int main()
{
	test_pass(abm_events_test(), "Testing and lockdown events");
	test_pass(abm_time_dependent_testing(), "Time dependent testing");
	test_pass(abm_vaccination(), "Vaccination");
}

bool abm_events_test()
{
	// Agents 
	std::string fin("test_data/NR_agents.txt");
	// Places
	std::string hfile("test_data/NR_households.txt");
	std::string sfile("test_data/NR_schools.txt");
	std::string wfile("test_data/NR_workplaces.txt");
	std::string hspfile("test_data/NR_hospitals.txt");
	std::string rh_file("test_data/NR_retirement_homes.txt");
	
	// Model parameters
	double dt = 0.25;
	int tmax = 200;

	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dexp_name("test_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"exposed never symptomatic", dexp_name}, {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	
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

	// Contains event times and properties
	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 
	double tol = 1e-3;
	double time = 0.0;

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){
		// Propagate
		time = abm.get_time(); 
		abm.transmit_infection();

		// School closure - school rate is 0.0
		if (float_equality<double>(time, infection_parameters.at("school closure"), tol)){
			const std::vector<School>& schools =  abm.get_vector_of_schools();
			for (const auto& school : schools){
				if (!float_equality<double>(school.get_transmission_rate(), 0.0, tol)){
					std::cerr << "Error in school closure - student transmission rate not 0.0" << std::endl; 
					return false;	
				}
				if (!float_equality<double>(school.get_employee_transmission_rate(), 0.0, tol)){
					std::cerr << "Error in school closure - employee transmission rate not 0.0" << std::endl; 
					return false;	
				}
			}
		}

		// Non-essential workplace closure - reduced rate
		if (float_equality<double>(time, infection_parameters.at("lockdown"), tol)){
			double new_tr_rate = infection_parameters.at("workplace transmission rate")*
									infection_parameters.at("fraction of ld businesses");
			const std::vector<Workplace>& workplaces =  abm.get_vector_of_workplaces();
			for (const auto& workplace : workplaces){
				if (!float_equality<double>(workplace.get_absenteeism_correction(), infection_parameters.at("lockdown absenteeism"), tol)){
					std::cerr << "Error in lockdown - wrong workplace absenteeism correction" << std::endl; 
					return false;	
				}
				if (!float_equality<double>(workplace.get_transmission_rate(), new_tr_rate, tol)){
					std::cerr << "Error in lockdown - wrong workplace transmission rate" << std::endl; 
					return false;	
				}
			}
		}

		// Reopening phase 1 - more businesses open
		if (float_equality<double>(time, infection_parameters.at("reopening phase 1"), tol)){
			double new_tr_rate = infection_parameters.at("workplace transmission rate")*
									infection_parameters.at("fraction of phase 1 businesses");
			const std::vector<Workplace>& workplaces =  abm.get_vector_of_workplaces();
			for (const auto& workplace : workplaces){
				if (!float_equality<double>(workplace.get_absenteeism_correction(), infection_parameters.at("lockdown absenteeism"), tol)){
					std::cerr << "Error in reopening phase 1 - wrong workplace absenteeism correction" << std::endl; 
					return false;	
				}
				if (!float_equality<double>(workplace.get_transmission_rate(), new_tr_rate, tol)){
					std::cerr << "Error in reopening phase 1 - wrong workplace transmission rate" << std::endl; 
					return false;	
				}
			}
		}

		// Reopening phase 2 - even more businesses open
		if (float_equality<double>(time, infection_parameters.at("reopening phase 2"), tol)){
			double new_tr_rate = infection_parameters.at("workplace transmission rate")*
									infection_parameters.at("fraction of phase 2 businesses");
			const std::vector<Workplace>& workplaces =  abm.get_vector_of_workplaces();
			for (const auto& workplace : workplaces){
				if (!float_equality<double>(workplace.get_absenteeism_correction(), infection_parameters.at("lockdown absenteeism"), tol)){
					std::cerr << "Error in reopening phase 2 - wrong workplace absenteeism correction" << std::endl; 
					return false;	
				}
				if (!float_equality<double>(workplace.get_transmission_rate(), new_tr_rate, tol)){
					std::cerr << "Error in reopening phase 2 - wrong workplace transmission rate" << std::endl; 
					return false;	
				}
			}
		}

		// Reopening phase 3 - restaurants open
		if (float_equality<double>(time, infection_parameters.at("reopening phase 3"), tol)){
			double new_tr_rate = infection_parameters.at("workplace transmission rate")*
									infection_parameters.at("fraction of phase 3 businesses");
			const std::vector<Workplace>& workplaces =  abm.get_vector_of_workplaces();
			for (const auto& workplace : workplaces){
				if (!float_equality<double>(workplace.get_absenteeism_correction(), infection_parameters.at("lockdown absenteeism"), tol)){
					std::cerr << "Error in reopening phase 3 - wrong workplace absenteeism correction" << std::endl; 
					return false;	
				}
				if (!float_equality<double>(workplace.get_transmission_rate(), new_tr_rate, tol)){
					std::cerr << "Error in reopening phase 3 - wrong workplace transmission rate" << std::endl; 
					return false;	
				}
			}
		}

	}
	return true;
}

bool abm_time_dependent_testing()
{
	// Agents 
	std::string fin("test_data/NR_agents.txt");
	// Places
	std::string hfile("test_data/NR_households.txt");
	std::string sfile("test_data/NR_schools.txt");
	std::string wfile("test_data/NR_workplaces.txt");
	std::string hspfile("test_data/NR_hospitals.txt");
	std::string rh_file("test_data/NR_retirement_homes.txt");
	
	// Model parameters
	double dt = 0.25;
	int tmax = 300;

	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dexp_name("test_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"exposed never symptomatic", dexp_name}, {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	
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

	// Contains event times and properties
	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters();
   	const std::vector<std::vector<double>> exp_values = {{27.0, 0.1, 0.5}, {60.0, 1.0, 0.4}};	
	double tol = 1e-3;
	double time = 0.0;
	double flu_prob = 0.0;
	int exp_num_changes = exp_values.size();
	int change_count = 0;

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){
		// Propagate
		time = abm.get_time(); 
		abm.transmit_infection();

		// No testing, no flu - check for all agents 
		if (time < infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			for (const auto& agent : agents){
				if ((agent.tested() == true) || (agent.symptomatic_non_covid() == true)){
					std::cerr << "Agents tested before testing is supposed to start" << std::endl;
					return false;
				}
			}
		}
		// Testing starts with initial testing values
		if (float_equality<double>(time, infection_parameters.at("start testing"), tol)){
			const std::map<std::string, double> infection_parameters = abm.get_infection_parameters();
			Testing testing = abm.get_testing_object();
			flu_prob = (infection_parameters.at("fraction false positive") + infection_parameters.at("negative tests fraction"))
							*infection_parameters.at("fraction to get tested");
			if (!float_equality<double>(testing.get_sy_tested_prob(), 
									infection_parameters.at("fraction to get tested"), 1e-5)||
				!float_equality<double>(testing.get_exp_tested_prob(), 
									infection_parameters.at("exposed fraction to get tested"), 1e-5) || 
				!float_equality<double>(testing.get_prob_flu_tested(), flu_prob, 1e-5)){
				std::cerr << "Wrong initial testing values" << std::endl;
				return false;
			}	
		}
		// Now check each switch 
		for (const auto& tch : exp_values){
			if (float_equality<double>(time, tch.at(0), tol)){
				++change_count;
				const std::map<std::string, double> infection_parameters = abm.get_infection_parameters();
				Testing testing = abm.get_testing_object();
				flu_prob = (infection_parameters.at("fraction false positive") 
								+ infection_parameters.at("negative tests fraction"))*tch.at(2);
				if (!float_equality<double>(testing.get_sy_tested_prob(), tch.at(2), 1e-5)||
					!float_equality<double>(testing.get_exp_tested_prob(), tch.at(1), 1e-5) || 
					!float_equality<double>(testing.get_prob_flu_tested(), flu_prob, 1e-5)){
					std::cerr << "Wrong testing values at time " << time << std::endl;
					return false;
				}	
			}
		}
	}
	if (change_count != exp_num_changes){
		std::cerr << "Number of testing values switches not equal expected " << change_count << std::endl;
		return false;
	}	
	return true;
}

/// Test suite for vaccination procedures
bool abm_vaccination()
{
	if (!abm_vaccination_random()){
		std::cerr << "Error in random vaccination functionality" << std::endl;
		return false;
	}
	if (!abm_vaccination_group()){
		std::cerr << "Error in group vaccination functionality" << std::endl;
		return false;
	}
	return true;
}

/// Test for vaccination of random population
bool abm_vaccination_random()
{
	double tol = 1e-3;
	double dt = 0.25, time = 0.0;
	int tmax = 200;
	int initially_infected = 100;	
	int n_vaccinated = 5000;

	// Regular tests
	ABM abm = create_abm(dt, initially_infected);
	abm.set_random_vaccination(n_vaccinated);
	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters();

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){

		// No testing - no vaccination 
		if (time < infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			for (const auto& agent : agents){
				if (agent.vaccinated() == true){
					std::cerr << "Agents vaccinated before vaccination is supposed to start" << std::endl;
					return false;
				}
			}
		}

		// Start of vaccination, and the remaining part 
		if (float_equality<double>(time, infection_parameters.at("start testing"), tol)
						|| time > infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			int actual_vac = 0;
			std::vector<int> vac_IDs = {};
			for (const auto& agent : agents){
				if (agent.vaccinated()){
					// Collect 
					++actual_vac;
					vac_IDs.push_back(agent.get_ID());
					// Flag check
					if (agent.removed() || agent.infected() || agent.exposed()
							|| agent.symptomatic() || agent.symptomatic_non_covid()){
						std::cerr << "Agent in an invalid state"  << agent.removed() << agent.infected() << agent.exposed()
							<< agent.symptomatic() << agent.symptomatic_non_covid()<< std::endl;
						return false;
					}
				}
			}
			// Numbers should match
		if (actual_vac != n_vaccinated){			
				std::cerr << "Simulated number of vaccinated agents does not match the model" << std::endl;
				return false;
			}
			// IDs should be unique
			std::vector<int> orig_vac = vac_IDs;
			auto last = std::unique(vac_IDs.begin(), vac_IDs.end());
   			vac_IDs.erase(last, vac_IDs.end()); 
			if (vac_IDs.size() != orig_vac.size()){			
				std::cerr << "IDs of vaccinated agents are not unique" << std::endl;
				return false;
			}			
			// IDs shouldn't be consecutive - at least not all of them
			std::vector<int> id_diff(orig_vac.size()-1,0); 
			std::transform(std::next(orig_vac.begin()), orig_vac.end(), vac_IDs.begin(), id_diff.begin(), std::minus<int>());
			if (std::all_of(id_diff.begin(), id_diff.end(), [](int x){ return ((x == 1) || (x == -1)); })){
				std::cerr << "IDs of vaccinated agents are not random but sequential" << std::endl;
				return false;
			}
		}
		// Propagate
		time = abm.get_time(); 
		abm.transmit_infection();
	}

	// Exception test - number of vaccinated
	std::runtime_error arg_err("Wrong number to get vaccinated");
	// New instance and initialization
	ABM ex_abm = create_abm(dt, initially_infected);
	const std::vector<Agent>& agents = ex_abm.get_vector_of_agents();
	// This should be enough for "too many to vaccinate"; if it is not - there are no infected and no flu
	// i.e. something is wrong
	n_vaccinated = agents.size();
	ex_abm.set_random_vaccination(n_vaccinated);
	// For information from the exception wrapper
	bool verbose = false;
	for (int ti = 0; ti<=tmax; ++ti){
		time = ex_abm.get_time();
		if (!exception_test(verbose, &arg_err, &ABM::transmit_infection, ex_abm)
				&& float_equality<double>(time, infection_parameters.at("start testing"), tol)){
			std::cerr << "Failed to throw exception for too many vaccinated requested" << std::endl;
			return false;
		}
	}
	return true;
}

/// Test for vaccination of specific population groups
bool abm_vaccination_group()
{
	double tol = 1e-3;
	double dt = 0.25, time = 0.0;
	int tmax = 200;
	int initially_infected = 10000;	
	std::string v_group("hospital employees");
	bool v_verbose = true;

	//
	// Hospital employees - regular tests
	//
	
	ABM abm = create_abm(dt, initially_infected);
	abm.set_group_vaccination(v_group, v_verbose);
	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters();

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){

		// No testing - no vaccination 
		if (time < infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			for (const auto& agent : agents){
				if (agent.vaccinated() == true){
					std::cerr << "Agents vaccinated before vaccination is supposed to start" << std::endl;
					return false;
				}
			}
		}

		// Start of vaccination, and the remaining part 
		if (float_equality<double>(time, infection_parameters.at("start testing"), tol)
						|| time > infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			int actual_vac = 0;
			for (const auto& agent : agents){
				if (agent.vaccinated()){
					// Collect 
					++actual_vac;
					// Group check
					if (!agent.hospital_employee()){
						std::cerr << "Agent not in hospital employee group"  << std::endl;
						return false;
					}
					// Flag check - throught the run
					if (agent.removed() || agent.infected() || agent.exposed()
							|| agent.symptomatic() || agent.symptomatic_non_covid()){
						std::cerr << "Agent in an invalid state"  << agent.removed() << agent.infected() << agent.exposed()
							<< agent.symptomatic() << agent.symptomatic_non_covid()<< std::endl;
						return false;
					}
				}
			}
		}
		// Propagate
		time = abm.get_time(); 
		abm.transmit_infection();
	}

	//
	// School employees - regular tests
	// 

	v_group = "school employees";	
	abm = create_abm(dt, initially_infected);
	abm.set_group_vaccination(v_group, v_verbose);

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){

		// No testing - no vaccination 
		if (time < infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			for (const auto& agent : agents){
				if (agent.vaccinated() == true){
					std::cerr << "Agents vaccinated before vaccination is supposed to start" << std::endl;
					return false;
				}
			}
		}

		// Start of vaccination, and the remaining part 
		if (float_equality<double>(time, infection_parameters.at("start testing"), tol)
						|| time > infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			int actual_vac = 0;
			for (const auto& agent : agents){
				if (agent.vaccinated()){
					// Collect 
					++actual_vac;
					// Group check
					if (!agent.school_employee()){
						std::cerr << "Agent not a school employee"  << std::endl;
						return false;
					}
					// Flag check - throught the run
					if (agent.removed() || agent.infected() || agent.exposed()
							|| agent.symptomatic() || agent.symptomatic_non_covid()){
						std::cerr << "Agent in an invalid state"  << agent.removed() << agent.infected() << agent.exposed()
							<< agent.symptomatic() << agent.symptomatic_non_covid()<< std::endl;
						return false;
					}
				}
			}
		}
		// Propagate
		time = abm.get_time(); 
		abm.transmit_infection();
	}

	//
	// Retirement home employees - regular tests
	// 

	v_group = "retirement home employees";	
	abm = create_abm(dt, initially_infected);
	abm.set_group_vaccination(v_group, v_verbose);

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){

		// No testing - no vaccination 
		if (time < infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			for (const auto& agent : agents){
				if (agent.vaccinated() == true){
					std::cerr << "Agents vaccinated before vaccination is supposed to start" << std::endl;
					return false;
				}
			}
		}

		// Start of vaccination, and the remaining part 
		if (float_equality<double>(time, infection_parameters.at("start testing"), tol)
						|| time > infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			int actual_vac = 0;
			for (const auto& agent : agents){
				if (agent.vaccinated()){
					// Collect 
					++actual_vac;
					// Group check
					if (!agent.retirement_home_employee()){
						std::cerr << "Agent not a retirement home employee"  << std::endl;
						return false;
					}
					// Flag check - throught the run
					if (agent.removed() || agent.infected() || agent.exposed()
							|| agent.symptomatic() || agent.symptomatic_non_covid()){
						std::cerr << "Agent in an invalid state"  << agent.removed() << agent.infected() << agent.exposed()
							<< agent.symptomatic() << agent.symptomatic_non_covid()<< std::endl;
						return false;
					}
				}
			}
		}
		// Propagate
		time = abm.get_time(); 
		abm.transmit_infection();
	}

	//
	// Retirement home resident - regular tests
	// 

	v_group = "retirement home residents";	
	abm = create_abm(dt, initially_infected);
	abm.set_group_vaccination(v_group, v_verbose);

	// Simulation
	for (int ti = 0; ti<=tmax; ++ti){

		// No testing - no vaccination 
		if (time < infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			for (const auto& agent : agents){
				if (agent.vaccinated() == true){
					std::cerr << "Agents vaccinated before vaccination is supposed to start" << std::endl;
					return false;
				}
			}
		}

		// Start of vaccination, and the remaining part 
		if (float_equality<double>(time, infection_parameters.at("start testing"), tol)
						|| time > infection_parameters.at("start testing")){
			const std::vector<Agent>& agents = abm.get_vector_of_agents();
			int actual_vac = 0;
			for (const auto& agent : agents){
				if (agent.vaccinated()){
					// Collect 
					++actual_vac;
					// Group check
					if (!agent.retirement_home_resident()){
						std::cerr << "Agent not a retirement home resident"  << std::endl;
						return false;
					}
					// Flag check - throught the run
					if (agent.removed() || agent.infected() || agent.exposed()
							|| agent.symptomatic() || agent.symptomatic_non_covid()){
						std::cerr << "Agent in an invalid state"  << agent.removed() << agent.infected() << agent.exposed()
							<< agent.symptomatic() << agent.symptomatic_non_covid()<< std::endl;
						return false;
					}
				}
			}
		}
		// Propagate
		time = abm.get_time(); 
		abm.transmit_infection();
	}

	//
	// Exception test - wrong input 
	//
	
	std::invalid_argument arg_err("Wrong name of the group");
	std::string wrong_name("middle school employee");
	// New instance and initialization
	ABM ex_abm = create_abm(dt, initially_infected);
	// For information from the exception wrapper
	bool verbose = false;
	v_verbose = false;
	ex_abm.set_group_vaccination(wrong_name, v_verbose);
	for (int ti = 0; ti<=tmax; ++ti){
		time = ex_abm.get_time();
		if (!exception_test(verbose, &arg_err, &ABM::transmit_infection, ex_abm)
				&& float_equality<double>(time, infection_parameters.at("start testing"), tol)){
			std::cerr << "Failed to throw exception for wrong name of the group to be vaccinated" << std::endl;
			return false;
		}
	}
	return true;
}



// Common operations for creating the ABM interface
ABM create_abm(const double dt, int inf0)
{
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

	return abm;	
}
