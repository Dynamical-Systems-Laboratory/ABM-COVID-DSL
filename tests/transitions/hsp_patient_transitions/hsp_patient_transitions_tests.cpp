#include "hsp_patient_transitions_tests.h"

/***************************************************** 
 *
 * Test suite for HspPatientTransitions class
 *
 ******************************************************/

// Tests
bool test_susceptible_transitions();
bool test_exposed_transitions();
bool test_symptomatic_transitions();

// Supporting functions
ABM create_abm(const double dt, int i0);
bool check_values(std::vector<double>);
bool check_fractions(int, int, double, std::string);
bool only_hospitals(const Agent&, const std::vector<Hospital>&, const std::vector<Household>&,
				const std::vector<School>&, const std::vector<RetirementHome>& retirement_homes,
				const std::vector<Workplace>&);
bool check_testing_transitions(const Agent& agent, const std::vector<Household>& households, 
				const std::vector<Hospital>& hospitals,	std::vector<int>& state_changes, 
				int& n_tested_exposed, int& n_tested_sy, int& n_waiting_for_res, 
				int& n_positive, int& n_false_neg, int& n_hsp, int& n_hsp_icu, int& n_ih, double time, double dt);
bool check_treatment_setup(const Agent& agent, const std::vector<Household>& households,
			const std::vector<Hospital>& hospitals, int& n_hsp, int& n_hsp_icu, int& n_ih, double time, double dt);
bool check_symptomatic_agent_removal(const Agent& agent, const std::vector<Household>& households,
				const std::vector<Hospital>& hospitals, std::vector<int>& state_changes, 
				int& n_rh, int& n_rd, double time, double dt);

int main()
{
	test_pass(test_susceptible_transitions(), "Susceptible transitions");
	test_pass(test_exposed_transitions(), "Exposed transitions");
	test_pass(test_symptomatic_transitions(), "Symptomatic transitions");
}

/// Series of tests for transitions of a susceptible non-COVID hospital patient
bool test_susceptible_transitions()
{
	// Time in days
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 20;
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
	Testing testing = abm.get_testing_object();

	HspPatientTransitions hsp_pt;

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
			if (agent.hospital_non_covid_patient() == false){
				continue;
			}
			if (!only_hospitals(agent, hospitals, households, schools, retirement_homes, workplaces)){
				std::cerr << "Location registration errors" << std::endl;
				return false;
			}
			if (agent.infected() == false){
				got_infected = hsp_pt.susceptible_transitions(agent, time, infection,
					hospitals, infection_parameters, agents, testing);
				if (got_infected == 0){
					continue;
				}
			    // If infected - test and collect
				++n_infected;
				++n_exposed;
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
					if (agent.home_isolated()){
						std::cerr << "Exposed hospital patient undergoing testing should not be home isolated" << std::endl;
						return false;
					}
					if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
						std::cerr << "Exposed hospital patient undergoing testing should still be in the hospital" << std::endl;
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
	// All should be tested in hospitals
	if (!check_fractions(n_hsp, n_tested_exposed, 1.0, "Wrong percentage tested in hospitals")){
		return false;
	}
	// None should be tested in a car
	if (!check_fractions(n_car, n_tested_exposed, 0.0, "Wrong percentage tested in car")){
		return false;
	}
	return true;
}

/// Series of tests for transitions of an exposed non-COVID hospital patient
bool test_exposed_transitions()
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
	Testing testing = abm.get_testing_object(); 

	HspPatientTransitions hsp_pt;

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
			if (agent.hospital_non_covid_patient() == false
						|| agent.removed()){
				continue;
			}
			if (agent.infected() == false){
				got_infected = hsp_pt.susceptible_transitions(agent, time, infection,
					hospitals, infection_parameters, agents, testing);
			}else if (agent.exposed() == true){
				state_changes = hsp_pt.exposed_transitions(agent, infection, time, dt, 
					households, hospitals, infection_parameters, testing);
				// Verify each possible state
				if (agent.exposed()){
					if (!check_testing_transitions(agent, households,  
								hospitals, state_changes, n_tested_exposed, n_tested_sy, 
								n_waiting_for_res, n_positive, 
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
					if (!check_testing_transitions(agent, households, hospitals, state_changes, 
							n_tested_exposed, n_tested_sy, n_waiting_for_res, n_positive, 
							n_false_neg, n_tr_hsp, n_hsp_icu, n_ih, time, dt)){
						std::cerr << "New symptomatic agent - testing error" << std::endl;
						return false;
					}
					// Treatment
					if (agent.tested_covid_positive()){
						if (!check_treatment_setup(agent, households, hospitals, n_tr_hsp, n_hsp_icu, n_ih, time, dt)){
							return false;
						}
					}
				} else if (agent.removed()){
					// Correct flags
					if (state_changes.at(0) != 1){
						std::cerr << "Wrong return value/flag for recovery " << std::endl;
						return false;
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
					if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
						std::cerr << "Recovered hospital patient undergoing testing should be back in the hospital" << std::endl;
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

/// Series of tests for transitions of a symptomatic non-COVID hospital patient
bool test_symptomatic_transitions()
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
	Testing testing = abm.get_testing_object();

	HspPatientTransitions hsp_pt;

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
			if (agent.hospital_non_covid_patient() == false || agent.removed()){
				continue;
			}
			if (agent.infected() == false){
				got_infected = hsp_pt.susceptible_transitions(agent, time, infection,
					hospitals, infection_parameters, agents, testing);
			}else if (agent.exposed() == true){
				state_changes = hsp_pt.exposed_transitions(agent, infection, time, dt, 
					households, hospitals, infection_parameters, testing);
			} else if (agent.symptomatic() == true){
				state_changes = hsp_pt.symptomatic_transitions(agent, time, dt, infection,  
					households, hospitals, infection_parameters);
				if (agent.removed()){
					if (!check_symptomatic_agent_removal(agent, households, hospitals, 
							state_changes, n_sy_recovering, n_sy_dying, time, dt)){
						return false;
					} 
				}
				if (!check_testing_transitions(agent, households,  
							hospitals, state_changes, n_tested_exposed, n_tested_sy, 
							n_waiting_for_res, n_positive, 
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

/// Check if values in the vector meet logical criteria
bool check_values(std::vector<double> values)
{
	auto is_negative = [](double val){ return val <= 0; };
	if (std::find_if(values.begin(), values.end(), is_negative) != values.end())
		return false;
	return true;
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

/// Check if an agent is only registered in a hospital
bool only_hospitals(const Agent& agent, const std::vector<Hospital>& hospitals, 
				const std::vector<Household>& households, const std::vector<School>& schools, 
				const std::vector<RetirementHome>& retirement_homes, 
				const std::vector<Workplace>& workplaces)
{
	if (agent.get_household_ID() > 0){
		if (find_in_place<Household>(households, agent.get_ID(), agent.get_household_ID())){
			std::cerr << "Hospital patient should not be member of a household" << std::endl;
			return false;
		}
		if ((agent.retirement_home_resident()) || (agent.get_household_ID() <= retirement_homes.size() && 
			find_in_place<RetirementHome>(retirement_homes, agent.get_ID(), agent.get_household_ID()))){
			std::cerr << "Hospital patient should not be member of a retirement home" << std::endl;
			return false;
		}
	}
	if (agent.get_school_ID() > 0 || agent.get_work_ID() > 0){
		std::cerr << "Hospital patient should not be member of a school or a workplace" << std::endl;
		return false;
	}
	if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
		std::cerr << "Hospital patient should be member of a hospital" << std::endl;
		return false;
	}
	return true;
}

/// Tests for exposed agent that is undergoing testing or gets the results
bool check_testing_transitions(const Agent& agent, const std::vector<Household>& households,
				const std::vector<Hospital>& hospitals, std::vector<int>& state_changes, 
				int& n_tested_exposed, int& n_tested_sy, int& n_waiting_for_res, 
				int& n_positive, int& n_false_neg, int& n_hsp, int& n_hsp_icu, int& n_ih, double time, double dt)
{
	// Testing flags
	if (agent.symptomatic() && !(agent.tested_covid_positive() || agent.tested())){
		std::cerr << "Symptomatic hospital non-covid patient should be tested" << std::endl;
		return false;
	}
	if (agent.tested()){
		if (agent.exposed()){
			if (agent.home_isolated()){
				std::cerr << "Exposed hospital non-covid patient undergoing testing should not be home isolated" << std::endl;
				return false;
			}
			if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
				std::cerr << "Exposed hospital patient undergoing testing should still be in the hospital" << std::endl;
				return false;
			}
		} else if (agent.symptomatic()){
			// Check home isolation
			if (agent.home_isolated()){
				std::cerr << "Symptomatic agent waiting for a test should not be home isolated" << std::endl;
				return false;
			}
			if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
				std::cerr << "Symptomatic hospital patient undergoing testing should still be in the hospital" << std::endl;
				return false;
			}
		}
		// Just tested
		if (state_changes.at(2)){
			if (agent.exposed()){
				++n_tested_exposed;
			} else {
				++n_tested_sy;
			}
			if (agent.exposed()){
				if (agent.home_isolated()){
					std::cerr << "Exposed hospital patient undergoing testing should not be home isolated at the time of test" << std::endl;
					return false;
				}
				if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
					std::cerr << "Exposed hospital patient undergoing testing should still be in the hospital at the time of test" << std::endl;
					return false;
				}
			} else if (agent.symptomatic()){
				// Check home isolation
				if (agent.home_isolated()){
					std::cerr << "Symptomatic hospital patient undergoing testing should not be home isolated at the time of test" << std::endl;
					return false;
				}
				if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
					std::cerr << "Exposed hospital patient undergoing testing should still be in the hospital at the time of test" << std::endl;
					return false;
				}
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
			// Should be home isolated 
			if (!agent.home_isolated()){
				std::cerr << "Agent confirmed positive not home isolated" << std::endl;
				return false;
			}
			if (find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
				std::cerr << "Exposed hospital patient should be home isolated after confirming covid positive" << std::endl;
				return false;
			}
			if (agent.get_household_ID() <= 0){
				std::cerr << "Exposed hospital patient in home isolation without a valid household ID" << std::endl;
				return false;
			}
			if (!find_in_place<Household>(households, agent.get_ID(), agent.get_household_ID())){
				std::cerr << "Exposed hospital patient in home isolation not registered in the household" << std::endl;
				return false;
			}
		} else {
			if (!check_treatment_setup(agent, households, hospitals, n_hsp, n_hsp_icu, n_ih, time, dt)){
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
		// Should not be home isolated 
		if (agent.home_isolated()){
			std::cerr << "Agent false negative still home isolated" << std::endl;
			return false;
		}
		if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
			std::cerr << "False negative exposed hospital patient should be in the hospital" << std::endl;
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
bool check_treatment_setup(const Agent& agent, const std::vector<Household>& households,
				const std::vector<Hospital>& hospitals, int& n_hsp, int& n_hsp_icu, 
				int& n_ih, double time, double dt)
{
	if (agent.hospitalized()){
		++n_hsp;
		// Isolation, hospital ID
		if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
			std::cerr << "Hospital patient hospitalized but not registered in a hospital" << std::endl;
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
		if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
			std::cerr << "Hospital patient hospitalized but not registered in a hospital" << std::endl;
			return false;
		}
		if (agent.dying()){
			// Should not be leaving ICU
			if (!float_equality<double>(agent.get_time_icu_to_hsp(), 0.0, 1e-5) || 
				!float_equality<double>(agent.get_time_hsp_to_ih(), 0.0, 1e-5)){
				std::cerr << "Dying symptomatic agent in ICU has non-zero hospital or/and ih transition times:\n" 
						  << "ICU->hsp: " << agent.get_time_icu_to_hsp() 
						  << " hsp->ih: " << agent.get_time_hsp_to_ih() << std::endl;
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
		if (agent.get_household_ID() <= 0){
			std::cerr << "Hospital patient in home isolation without a valid household ID" << std::endl;
			return false;
		}
		if (!find_in_place<Household>(households, agent.get_ID(), agent.get_household_ID())){
			std::cerr << "Hospital patient in home isolation still registered in the household" << std::endl;
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
				std::cerr << "Symptomatic agent home isolated with invalid time of optional to transition to hospital" << std::endl;
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

/// Test for properties related to removal of a symptomatic agent
bool check_symptomatic_agent_removal(const Agent& agent, const std::vector<Household>& households,
				const std::vector<Hospital>& hospitals, std::vector<int>& state_changes, 
				int& n_rh, int& n_rd, double time, double dt)
{
	const int aID = agent.get_ID();
	if (state_changes.at(1)){
		++n_rd;
		// If the agent died - removed from all places
		if (agent.retirement_home_resident()){
			std::cerr << "Hospital non-COVID patient should not be a retirement home resident" << std::endl;
			return false;	
		} else {
			if (agent.get_household_ID() > 0 && 
					find_in_place<Household>(households, aID, agent.get_household_ID())){
				std::cerr << "Agent that died still present in a household" << std::endl;
				return false;
			}
		}
		if (find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
			std::cerr << "Hospital patient that died still registered in a hospital" << std::endl;
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
		if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID())){
			std::cerr << "Hospital patient that recovered should be registered in a hospital" << std::endl;
			return false;
		}
		if (agent.retirement_home_resident()){
			std::cerr << "Hospital non-COVID patient should not be a retirement home resident" << std::endl;
			return false;	
		} else {
			if (agent.get_household_ID() > 0 && 
					find_in_place<Household>(households, aID, agent.get_household_ID())){
				std::cerr << "Agent that recovered still present in a household" << std::endl;
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
