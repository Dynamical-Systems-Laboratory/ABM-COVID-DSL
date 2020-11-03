#include "regular_transitions_tests.h"

/***************************************************** 
 *
 * Test suite for RegularTransitions class
 *
 ******************************************************/

// Tests
bool test_susceptible_transitions();
bool test_exposed_transitions();
bool test_symptomatic_transitions();

// Supporting functions
ABM create_abm(const double dt, int i0);
bool check_isolation(const Agent&, const std::vector<School>,
				 const std::vector<RetirementHome>, const std::vector<Workplace>,
				 bool check_in_isolation = true);
bool check_tests(const Agent& agent, std::vector<double>& time_of_results,
					int& n_tc, int& n_hsp, int& n_false_neg, int& n_ih, 
					const std::vector<Hospital> hospitals,
					const std::vector<School> schools, const std::vector<Workplace> workplaces);
bool check_fractions(int, int, double, std::string);
bool check_testing_transitions(const Agent& agent, const std::vector<Household>& households, 
				const std::vector<School>& schools, const std::vector<Hospital>& hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces, 
				std::vector<int>& state_changes, int& n_tested_exposed, int& n_tested_sy, int& n_waiting_for_res, 
				int& n_positive, int& n_false_neg, int& n_hsp, int& n_hsp_icu, int& n_ih, double time, double dt);
bool check_treatment_setup(const Agent& agent, const std::vector<Household> households,
				const std::vector<School>& schools, const std::vector<Hospital> hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces,
				int& n_hsp, int& n_hsp_icu, int& n_ih, double time, double dt);
bool check_hospitalized_isolation(const Agent& agent, const std::vector<Household> households,
				const std::vector<School>& schools, const std::vector<Hospital> hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces);
bool check_symptomatic_agent_removal(const Agent& agent, const std::vector<Household>& households,
				const std::vector<School>& schools, const std::vector<Hospital>& hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces, 
				std::vector<int>& state_changes, int& n_rh, int& n_rd, double time, double dt);

int main()
{
	test_pass(test_susceptible_transitions(), "Susceptible transitions");
	test_pass(test_exposed_transitions(), "Exposed transitions");
	test_pass(test_symptomatic_transitions(), "Symptomatic transitions");
}

/// Series of tests for transitions of a regular susceptible agent
bool test_susceptible_transitions()
{
	// Time in days
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 200;
	double time = 0.0, t_exp = 0.0; 
	int initially_infected = 10000;	

	ABM abm = create_abm(dt, initially_infected);

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
	std::vector<Household>& households = abm.vector_of_households();
    std::vector<School>& schools = abm.vector_of_schools();
    std::vector<Workplace>& workplaces = abm.vector_of_workplaces();
    std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	std::vector<RetirementHome>& retirement_homes = abm.vector_of_retirement_homes();
	Infection& infection = abm.get_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 
	Flu& flu = abm.get_flu_object();
	Testing testing = abm.get_testing_object();

	RegularTransitions regular;

	// Testing-related variables
	int got_infected = 0;
	int n_infected = 0, n_exposed = 0;
	int n_exposed_never_sy = 0, n_tested_exposed = 0;
	int n_hsp = 0, n_car = 0;
	// To initialize Flu agents
	testing.check_switch_time(0);
	abm.check_events(schools, workplaces);

	for (int ti = 0; ti<=tmax; ++ti){
		abm.compute_place_contributions();
	    					    				    
		for (auto& agent : agents){
			if (agent.hospital_employee() || 
					agent.hospital_non_covid_patient() || 
					agent.symptomatic_non_covid()){
				continue;
			}
			if (agent.infected() == false){
				got_infected = regular.susceptible_transitions(agent, time, infection,
					households, schools, workplaces, hospitals, 
					retirement_homes, infection_parameters, agents, flu, testing);
				if (got_infected == 0){
					continue;
				}
				// If infected - test and collect
				++n_infected;
				++n_exposed;
				// Flu - should not be in susceptible anymore
				int aID = agent.get_ID();
				const std::vector<int> flu_susceptible = flu.get_susceptible_IDs();
				for (const auto& afs : flu_susceptible){
					if (afs == aID){
						std::cerr << "Infected agent should not be part of future flu poll" << std::endl; 
						return false;
					}
				}
				// And shouldn't be in flu either
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
				if (agent.tested()){
					++n_tested_exposed;
					// Time to test
					t_exp = time + infection_parameters.at("time from decision to test");
					if (!float_equality<double>(t_exp, agent.get_time_of_test(), 1.1*dt)){
						std::cerr << "Wrong test time:\n"
								  << "\t\tComputed: " << agent.get_time_of_test() << " Expected: " << t_exp << std::endl; 
						return false;
					}
					// Isolation
					if (!agent.home_isolated()){
						std::cerr << "Exposed agent undergoing testing but not home isolated" << std::endl;
						return false;
					}
					if (!check_isolation(agent, schools, retirement_homes, workplaces)){
						std::cerr << "Exposed agent home isolated but still present in public places" << std::endl;
						return false;
					}
					// Treatment type
					if (agent.tested_in_hospital()){
						++n_hsp;
						// Hospital ID for testing
						if (agent.get_hospital_ID() <= 0){
							std::cerr << "Hospital tested agent without hospital ID" << std::endl;
							return false;
						}
					} else if (agent.tested_in_car()){
						++n_car;
					} else {
						std::cerr << "Exposed agents testing location not specified" << std::endl;
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
			}
		}
		abm.reset_contributions();
		abm.advance_in_time();
		time += dt;			
}

	// Verify the outcomes
	// Non-zero number of infected (some must have gotten infected)
	if (n_infected == 0){
		std::cerr << "No newly infected in the simulation" << std::endl;
		return false;
	}
	// Fraction that never develops symptoms
	if (!check_fractions(n_exposed_never_sy, n_exposed, infection_parameters.at("fraction exposed never symptomatic"), 
							"Wrong percentage of exposed agents never developing symptoms")){
		return false;
	}
	// Fraction tested
	if (!check_fractions(n_tested_exposed, n_exposed, infection_parameters.at("exposed fraction to get tested"), 
							"Wrong percentage of tested agents")){
		return false;
	}
	// Fraction tested in hospitals
	if (!check_fractions(n_hsp, n_tested_exposed, infection_parameters.at("fraction tested in hospitals"), 
							"Wrong percentage tested in hospitals")){
		return false;
	}

	return true;
}

/// Series of tests for transitions of a regular exposed agent
bool test_exposed_transitions()
{
	// Time in days
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 200;
	double time = 0.0, t_exp = 0.0; 
	int initially_infected = 700;	

	ABM abm = create_abm(dt, initially_infected);

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
	std::vector<Household>& households = abm.vector_of_households();
    std::vector<School>& schools = abm.vector_of_schools();
    std::vector<Workplace>& workplaces = abm.vector_of_workplaces();
    std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	std::vector<RetirementHome>& retirement_homes = abm.vector_of_retirement_homes();
	Infection& infection = abm.get_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 
	Testing testing = abm.get_testing_object();
	Flu flu = abm.get_flu_object();

	RegularTransitions regular;

	// Outcomes 
	int got_infected = 0;
	int n_infected = 0, n_exposed = 0;
	int n_tested_exposed = 0, n_waiting_for_res = 0;
	int n_tested_sy = 0, n_new_sy = 0;
	int n_positive = 0, n_false_neg = 0;
	int n_hsp = 0, n_car = 0;
	int n_sy_recovering = 0, n_sy_dying = 0;
	int n_tr_hsp = 0, n_hsp_icu = 0, n_ih = 0;
	// Recovered, dead (not applicable), tested, tested positive, tested false negative
    std::vector<int> state_changes(5,0);

	// To initialize Flu agents
	testing.check_switch_time(0);
	abm.check_events(schools, workplaces);
	for (int ti = 0; ti<=tmax; ++ti){
		abm.compute_place_contributions();
		for (auto& agent : agents){
			if (agent.hospital_employee() || 
					agent.hospital_non_covid_patient() || 
					agent.symptomatic_non_covid() || agent.removed()){
				continue;
			}
			if (agent.infected() == false){
				got_infected = regular.susceptible_transitions(agent, time, infection,
					households, schools, workplaces, hospitals, 
					retirement_homes, infection_parameters, agents, flu, testing);
			} else if (agent.exposed() == true){
				state_changes = regular.exposed_transitions(agent, infection, time, dt, 
					households, schools, workplaces, hospitals, retirement_homes, 
					infection_parameters, testing);
				// Verify each possible state
				if (agent.exposed()){
					if (!check_testing_transitions(agent, households, schools, 
							hospitals, retirement_homes, workplaces,
							state_changes, n_tested_exposed, n_tested_sy, n_waiting_for_res, n_positive, 
							n_false_neg, n_tr_hsp, n_hsp_icu, n_ih, time, dt)){
						std::cerr << "Exposed agent - testing error" << std::endl;
						return false;
					}	
				} else if (agent.symptomatic()){
					++n_new_sy;
					// Recovery or death
					if (agent.recovering()){
						++n_sy_recovering;
						if (agent.get_recovery_time() <= time){
							std::cerr << "Symptomatic agent with invalid recovery time" << std::endl;
							return false;
						}
					} else if (agent.dying()){
						++n_sy_dying;
						if (agent.get_time_of_death() <= time){
							std::cerr << "Symptomatic agent with invalid death time" << std::endl;
							return false;
						}
					} else {
						std::cerr << "Symptomatic agent does not have a valid removal state" << std::endl;
						return false;
					}
					// Testing 
					if (!check_testing_transitions(agent, households, schools, 
							hospitals, retirement_homes, workplaces,
							state_changes, n_tested_exposed, n_tested_sy, n_waiting_for_res, n_positive, 
							n_false_neg, n_tr_hsp, n_hsp_icu, n_ih, time, dt)){
						std::cerr << "New symptomatic agent - testing error" << std::endl;
						return false;
					}
					// Treatment
					if (agent.tested_covid_positive()){
						if (!check_treatment_setup(agent, households, schools, hospitals, retirement_homes, 
												workplaces, n_tr_hsp, n_hsp_icu, n_ih, time, dt)){
							return false;
						}
					}
				} else if (agent.removed()){
					// Correct flags
					if (state_changes.at(0) != 1){
						std::cerr << "Wrong return value/flag for recovery" << std::endl;
					}
					// Recovery time
					if (!float_equality<double>(time, agent.get_latency_end_time(), 1.1*dt)){
						std::cerr << "Wrong recovery time:\n"
								  << "\t\tCurrent time: " << time << " Expected: " << agent.get_latency_end_time() << std::endl; 
						return false;
					}
				   	// Checked if no home isolation
				   	if (agent.home_isolated()){
						std::cerr << "Exposed agent home isolated after recovery" << std::endl; 
						return false;
				  	}
					if (check_isolation(agent, schools, retirement_homes, workplaces, false)){
						std::cerr << "Exposed agent not home isolated but still not present in public places after recovery" << std::endl;
						return false;
					}
				}
			}
		}
		abm.reset_contributions();
		abm.advance_in_time();
		time += dt;
	}

	// Check values and ratios
	// All these have to be non-zero 
	if (n_tested_exposed == 0 || n_waiting_for_res == 0 || n_positive == 0 || 
					n_false_neg == 0){
		std::cerr << "Some of the testing quantities are 0" << std::endl;
		return false;
	}
	// Fraction false negative
	if (!check_fractions(n_false_neg, n_tested_exposed, infection_parameters.at("fraction false negative"), 
							"Wrong percentage of false negative agents")){
		return false;
	}
	// Treatment - all need to be non-zero
	if (n_tr_hsp == 0 || n_hsp_icu == 0 || n_ih == 0){
		std::cerr << "Some of the treatment types are 0" << std::endl;
		return false;
	}
	// Removal - all need to be non-zero
	if (n_sy_recovering == 0 || n_sy_dying == 0){
		std::cerr << "Some of the removal types are 0" << std::endl;
		return false;
	}
	return true;
}

/// Series of tests for transitions of a regular symptomatic agent
bool test_symptomatic_transitions()
{
	// Time in days
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 200;
	double time = 0.0, t_exp = 0.0; 
	int initially_infected = 20000;	

	ABM abm = create_abm(dt, initially_infected);

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
	std::vector<Household>& households = abm.vector_of_households();
    std::vector<School>& schools = abm.vector_of_schools();
    std::vector<Workplace>& workplaces = abm.vector_of_workplaces();
    std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	std::vector<RetirementHome>& retirement_homes = abm.vector_of_retirement_homes();
	Infection& infection = abm.get_infection_object();
	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 
	Flu& flu = abm.get_flu_object();
	Testing testing = abm.get_testing_object();

	RegularTransitions regular;

	// Outcomes 
	int got_infected = 0;
	int n_waiting_for_res = 0, n_tested_exposed = 0;
	int n_tested_sy = 0, n_new_sy = 0;
	int n_positive = 0, n_false_neg = 0;
	int n_hsp = 0, n_car = 0;
	int n_sy_recovering = 0, n_sy_dying = 0;
	int n_tr_hsp = 0, n_hsp_icu = 0, n_ih = 0;
	// Recovered, dead (not applicable), tested, tested positive, tested false negative
    std::vector<int> state_changes(5,0);

	// To initialize Flu agents
	testing.check_switch_time(0);
	abm.check_events(schools, workplaces);
	for (int ti = 0; ti<=tmax; ++ti){
		abm.compute_place_contributions();
		for (auto& agent : agents){
			if (agent.hospital_employee() || 
					agent.hospital_non_covid_patient() || 
					agent.symptomatic_non_covid() || agent.removed())
				continue;
			if (agent.infected() == false){
				got_infected = regular.susceptible_transitions(agent, time, infection,
					households, schools, workplaces, hospitals, 
					retirement_homes, infection_parameters, agents, flu, testing);
			}else if (agent.exposed() == true){
				state_changes = regular.exposed_transitions(agent, infection, time, dt, 
					households, schools, workplaces, hospitals, retirement_homes, 
					infection_parameters, testing);
			} else if (agent.symptomatic() == true){
				state_changes = regular.symptomatic_transitions(agent, time, dt, infection,
					households, schools, workplaces, hospitals, retirement_homes, 
					infection_parameters);
				if (agent.removed()){
					if (!check_symptomatic_agent_removal(agent, households, schools, hospitals, retirement_homes, 
									workplaces, state_changes, n_sy_recovering, n_sy_dying, time, dt)){
						return false;
					} 
				}
				if (!check_testing_transitions(agent, households, schools, 
							hospitals, retirement_homes, workplaces,
							state_changes, n_tested_exposed, n_tested_sy, n_waiting_for_res, n_positive, 
							n_false_neg, n_tr_hsp, n_hsp_icu, n_ih, time, dt)){
					std::cerr << "Symptomatic agent - testing error" << std::endl;
					return false;
				}
			}
		}
		abm.reset_contributions();
		abm.advance_in_time();
		time += dt;
	}

 	// Fraction symptomatic tested 
	if (!check_fractions(n_tested_sy, n_new_sy, infection_parameters.at("fraction to get tested"), 
							"Wrong percentage of symptomatic agents that should be tested")){
		return false;
	}
	// Treatment - all need to be non-zero
	if (n_tr_hsp == 0 || n_hsp_icu == 0 || n_ih == 0){
		std::cerr << "Some of the treatment types are 0" << std::endl;
		return false;
	}
	// Removal - all need to be non-zero
	if (n_sy_recovering == 0 || n_sy_dying == 0){
		std::cerr << "Some of the removal types are 0" << std::endl;
		return false;
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

/// Check if agent is checked out from all the public places
bool check_isolation(const Agent& agent, const std::vector<School> schools,
				const std::vector<RetirementHome> retirement_homes,
				const std::vector<Workplace> workplaces, bool check_in_isolation)
{
	const int aID = agent.get_ID();
   	bool not_present_school = true, not_present_work = true;	
	bool in_isolation = true;
	if (agent.student()){
		if (find_in_place<School>(schools, aID, agent.get_school_ID())){
			not_present_school = false;
		}
	}
	if (agent.works()){
		if (agent.school_employee()){
			if (find_in_place<School>(schools, aID, agent.get_work_ID())){
				not_present_work = false;
			}
		}else if (agent.retirement_home_employee()){
			if (find_in_place<RetirementHome>(retirement_homes, aID, agent.get_work_ID())){
				not_present_work = false;
			}		
		} else {
			if (find_in_place<Workplace>(workplaces, aID, agent.get_work_ID())){
				not_present_work = false;
			}
		}
	}
	if (check_in_isolation){
		in_isolation = not_present_school && not_present_work;
	} else {
		if (agent.student() && agent.works()){
			in_isolation = not_present_school && not_present_work;
		} else if (agent.student()){
			in_isolation =  not_present_school;
		} else if (agent.works()){
			in_isolation = not_present_work;
		} else if (!agent.student() && !agent.works()){
			in_isolation = false; 
		}	
	}
	return in_isolation;
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

/// Tests for exposed agent that is undergoing testing or gets the results
bool check_testing_transitions(const Agent& agent, const std::vector<Household>& households,
				const std::vector<School>& schools, const std::vector<Hospital>& hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces, 
				std::vector<int>& state_changes, int& n_tested_exposed, int& n_tested_sy, int& n_waiting_for_res, 
				int& n_positive, int& n_false_neg, int& n_hsp, int& n_hsp_icu, int& n_ih, double time, double dt)
{
	// Testing flags
	if (agent.tested()){
		// Check home isolation
		if (!agent.home_isolated()){
			std::cerr << "Agent undergoing testing but not home isolated" << std::endl;
			return false;
		}
		if (!check_isolation(agent, schools, retirement_homes, workplaces)){
			std::cerr << "Tested agent home isolated but still present in public places" << std::endl;
			return false;
		}
		// Just tested
		if (state_changes.at(2)){
			if (agent.exposed()){
				++n_tested_exposed;
			} else {
				++n_tested_sy;
			}
			// Should still be home isolated
			if (!agent.home_isolated()){
				std::cerr << "Agent waiting for test results not home isolated" << std::endl;
				return false;
			}
			if (!check_isolation(agent, schools, retirement_homes, workplaces)){
				std::cerr << "Agent waiting for test results home isolated but still present in public places" << std::endl;
				return false;
			}
			// Needs to be waiting for results
			if (!agent.tested_awaiting_results()){
				std::cerr << "Agent just tested is not waiting for test results" << std::endl;
				return false;
			}
			// Check testing time (can be approximate within a dt)
			if (!float_equality<double>(time, agent.get_time_of_test(), 1.1*dt)){
				  std::cerr << "Wrong testing time:\n"
				  			<< "\t\tCurrent time: " << time << " Expected: " << agent.get_time_of_test() << std::endl; 
				return false;
			}
			++n_waiting_for_res;	
		}
	}
	// Just got results 
	// Confirmed positive
	if (state_changes.at(3)){
		++n_positive;
		if (agent.exposed()){
			// Should still be home isolated 
			if (!agent.home_isolated()){
				std::cerr << "Agent confirmed positive not home isolated" << std::endl;
				return false;
			}
			if (!check_isolation(agent, schools, retirement_homes, workplaces)){
				std::cerr << "Agent confirmed positive and home isolated but still present in public places" << std::endl;
				return false;
			}

		} else {
			if (!check_treatment_setup(agent, households, schools, hospitals, retirement_homes, 
									workplaces, n_hsp, n_hsp_icu, n_ih, time, dt)){
				return false;
			}

		}
		// Correct flag
		if (!agent.tested_covid_positive()){
			std::cerr << "Agent confirmed positive doesn't have the flag set" << std::endl;
			return false;
		}
		// Check results time 
		if (!float_equality<double>(time, agent.get_time_of_results(), 1.1*dt)){
			  std::cerr << "Wrong test results time:\n"
			  			<< "\t\tCurrent time: " << time << " Expected: " << agent.get_time_of_results() << std::endl; 
			return false;
		}
	}
	// Symptomatic cannot be false negative right now, and the flag may be set before 
	// transitioning to symptomatic - i.e. at the same step
	if (state_changes.at(4) && agent.exposed()){
		++n_false_neg;
		// Should still be home isolated 
		if (agent.home_isolated()){
			std::cerr << "Agent false negative still home isolated" << std::endl;
			return false;
		}
		if (check_isolation(agent, schools, retirement_homes, workplaces, false)){
			std::cerr << "Agent false negative and still not back in public places" << std::endl;
			return false;
		}
		// Correct flag
		if (!agent.tested_false_negative()){
			std::cerr << "Agent false negative doesn't have the flag set" << std::endl;
			return false;
		}
		// Check results time 
		if (!float_equality<double>(time, agent.get_time_of_results(), 1.1*dt)){
			  std::cerr << "Wrong test results time:\n"
			  			<< "\t\tCurrent time: " << time << " Expected: " << agent.get_time_of_results() << std::endl; 
			return false;
		}
	}
	return true;
}

/// Verification of initial treatment
bool check_treatment_setup(const Agent& agent, const std::vector<Household> households,
				const std::vector<School>& schools, const std::vector<Hospital> hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces,
				int& n_hsp, int& n_hsp_icu, int& n_ih, double time, double dt)
{
	if (agent.hospitalized()){
		++n_hsp;
		// Isolation, hospital ID
		if (!check_hospitalized_isolation(agent, households, schools, hospitals, retirement_homes, workplaces)){
			std::cerr << "Hospitalized agent present/missing from assigned locations" << std::endl;
			return false;
		}
		if (agent.dying()){
			if (agent.get_time_hsp_to_icu() <= time){
				std::cerr << "Dying symptomatic agent with invalid hospital to ICU transition time" << std::endl;
				return false;
			}
		} else {
			if (agent.get_time_hsp_to_ih() <= time){
				std::cerr << "Recovering symptomatic agent with invalid hospital to home isolation transition time" << std::endl;
				return false;
			}
		}		
	} else if (agent.hospitalized_ICU()){
		++n_hsp_icu;
		// Isolation, hospital ID
		if (!check_hospitalized_isolation(agent, households, schools, hospitals, retirement_homes, workplaces)){
			std::cerr << "ICU hospitalized agent present/missing from assigned locations" << std::endl;
			return false;
		}
		if (agent.dying()){
			// Should not be leaving ICU
			if (!float_equality<double>(agent.get_time_icu_to_hsp(), 0.0, 1e-5) || 
				!float_equality<double>(agent.get_time_hsp_to_ih(), 0.0, 1e-5)){
				std::cerr << "Dying symptomatic agent in ICU has non-zero hospital or/and ih transition times" << std::endl;
				return false;
			}
		} else {
			// Transitioning to hospital, then to home isolation (if not recovered before that)
			if (agent.get_time_icu_to_hsp() <= time){
				std::cerr << "Recovering symptomatic agent with invalid ICU to hospital transition time" << std::endl;
				return false;
			}
			if (agent.get_time_hsp_to_ih() <= time){
				std::cerr << "Recovering symptomatic agent with invalid ICU (hospital) to home isolation transition time" << std::endl;
				return false;
			}
		}
	} else if (agent.home_isolated()){
		++n_ih;
		if (!check_isolation(agent, schools, retirement_homes, workplaces)){
			std::cerr << "Symptomatic agent home isolated but still present in public places" << std::endl;
			return false;
		}
		if (agent.dying()){
			if (agent.get_time_ih_to_icu() <= time){
				std::cerr << "Dying symptomatic agent with invalid home isolation to ICU transition time" << std::endl;
				return false;
			}
		} else {
			if (!(float_equality<double>(agent.get_time_ih_to_hsp(), 0.0, 1e-5) || 
					agent.get_time_ih_to_hsp() > time)){
				std::cerr << "Symptomatic agent home isolated with invalid time of optional transition to hospital" << std::endl;
				return false;
			}
		}
	} else {
		std::cerr << "Treated agent without a valid treatment type" << std::endl;
		return false;
	}
	if (!agent.being_treated()){	
		std::cerr << "Treated agent without a valid treatment flag" << std::endl;
		return false;
	}

	return true;
}

/// Verification of isolation and placement of a hospitalized agent 
bool check_hospitalized_isolation(const Agent& agent, const std::vector<Household> households,
				const std::vector<School>& schools, const std::vector<Hospital> hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces)
{
	const int aID = agent.get_ID();
	bool not_present = true;
	// Household or retirement home
	if (agent.retirement_home_resident()){
		if (find_in_place<RetirementHome>(retirement_homes, aID, agent.get_household_ID())){
			std::cerr << "Hospitalized agent still present in a retirement home" << std::endl;
			return false;
		}	
	} else {
		if (find_in_place<Household>(households, aID, agent.get_household_ID())){
			std::cerr << "Hospitalized agent still present in a household" << std::endl;
			return false;
		}
	}
	// Public places - schools and work
	if (!check_isolation(agent, schools, retirement_homes, workplaces)){
		std::cerr << "Hospitalized agent still present in public places" << std::endl;
		return false;
	}
	// Hospital
	if (agent.get_hospital_ID() <= 0){
		std::cerr << "Hospitalized agent without a valid hospital ID" << std::endl;
		return false;
	}
	if (!find_in_place<Hospital>(hospitals, aID, agent.get_hospital_ID())){
		std::cerr << "Hospitalized agent not registered in a hospital" << std::endl;
		return false;
	}
	return true;
}

/// Test for properties related to removal of a symptomatic agent
bool check_symptomatic_agent_removal(const Agent& agent, const std::vector<Household>& households,
				const std::vector<School>& schools, const std::vector<Hospital>& hospitals,
				const std::vector<RetirementHome>& retirement_homes, const std::vector<Workplace>& workplaces, 
				std::vector<int>& state_changes, int& n_rh, int& n_rd, double time, double dt)
{
	const int aID = agent.get_ID();
	if (state_changes.at(1)){
		++n_rd;
		// If the agent died - removed from all places
		if (agent.retirement_home_resident()){
			if (find_in_place<RetirementHome>(retirement_homes, aID, agent.get_household_ID())){
				std::cerr << "Agent that died still present in a retirement home" << std::endl;
				return false;
			}	
		} else {
			if (find_in_place<Household>(households, aID, agent.get_household_ID())){
				std::cerr << "Agent that died still present in a household" << std::endl;
				return false;
			}
		}
		if (agent.get_hospital_ID()>0){
			if (find_in_place<Hospital>(hospitals, aID, agent.get_hospital_ID())){
				std::cerr << "Agent that died still present in a hospital" << std::endl;
				return false;
			}
		}
		if (!check_isolation(agent, schools, retirement_homes, workplaces)){
			std::cerr << "Agent that died still present in public places" << std::endl;
			return false;
		}
		// Time of death 
		if (!float_equality<double>(time, agent.get_time_of_death(), 1.1*dt)){
			std::cerr << "Wrong time of death:\n"
					  << "\t\tCurrent time: " << time << " Expected: " << agent.get_time_of_death() << std::endl; 
			return false;
		}
		// Not tested/not treated/false negative
		if (!agent.tested_covid_positive() && state_changes.at(1) != 2){
			std::cerr << "Wrong flag for an untreated agent that died." << std::endl;
			return false;
		}
	} else if (state_changes.at(0)){
		++n_rh;
		// If recovering
		// Checked if no home isolation
		if (agent.home_isolated()){
			std::cerr << "Symptomatic agent home isolated after recovery" << std::endl; 
			return false;
		}
		if (check_isolation(agent, schools, retirement_homes, workplaces, false)){
			std::cerr << "Symptomatic agent not home isolated but still not present in public places after recovery" << std::endl;
			return false;
		}
		if (agent.retirement_home_resident()){
			if (!find_in_place<RetirementHome>(retirement_homes, aID, agent.get_household_ID())){
				std::cerr << "Agent recovered but not present in a retirement home" << std::endl;
				return false;
			}	
		} else {
			if (!find_in_place<Household>(households, aID, agent.get_household_ID())){
				std::cerr << "Agent recovered but not present in a household" << std::endl;
				return false;
			}
		}
		if (agent.get_hospital_ID()>0){
			if (find_in_place<Hospital>(hospitals, aID, agent.get_hospital_ID())){
				std::cerr << "Agent recovered but still present in a hospital" << std::endl;
				return false;
			}
		}
		// Recovery time
		if (!float_equality<double>(time, agent.get_recovery_time(), 1.1*dt)){
			std::cerr << "Wrong recovery time:\n"
					  << "\t\tCurrent time: " << time << " Expected: " << agent.get_recovery_time() << " " << dt<< std::endl; 
			return false;
		}
	} else {
		std::cerr << "Invalid removal flag" << std::endl;
		return false;
	}

	return true;
}
