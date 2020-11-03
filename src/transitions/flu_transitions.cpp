#include "../../include/transitions/flu_transitions.h"

/***************************************************** 
 * class: FluTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for an agent that
 * has a condition other than COVID and is symptomatic  
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
std::vector<int> FluTransitions::susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				std::vector<RetirementHome>& retirement_homes,
				const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, Flu& flu, const Testing& testing, const double dt)
{
	double lambda_tot = 0.0;
	// Infected, tested, negative, false positive
	std::vector<int> state_changes(4,0);

	lambda_tot = compute_susceptible_lambda(agent, time, households, schools, workplaces, hospitals, retirement_homes);
	if (infection.infected(lambda_tot) == true){
		state_changes.at(0) = 1;
		int new_flu = flu.swap_flu_agent(agent.get_ID());
		// If still available
		if (new_flu != -1){
			process_new_flu(agents.at(new_flu-1), hospitals.size(), time, schools, workplaces,
						    retirement_homes, infection, infection_parameters, flu, testing);
		}
		states_manager.set_former_flu(agent);
		agent.set_inf_variability_factor(infection.inf_variability());
		// Infectiousness, latency, and possibility of never developing 
		// symptoms 
		recovery_and_incubation(agent, infection, time, infection_parameters);
		// Determine if getting tested, how, and when
		// Remove agent from places if under home isolation
		set_testing_status(agent, infection, time, schools, 
							workplaces, hospitals, retirement_homes, 
							infection_parameters, testing);
	} else {
		// If entering pre-test home isolation		
		if ((agent.tested()) && (agent.get_time_for_flu_isolation() <= time)
				&& (agent.tested_awaiting_test() == true)
				&& (agent.home_isolated() == false)){
			agent.set_home_isolated(true);
		}
		// If being tested
		if ((agent.tested()) && (agent.get_time_of_test() <= time)
				&& (agent.tested_awaiting_test() == true)){
			testing_transitions_flu(agent, time, infection_parameters);
			state_changes.at(1) = 1;
		}
		// If getting test results (this may in principle happen in a
		// single step)
		if ((agent.tested()) && (agent.get_time_of_results() <= time)
			&& (agent.tested_awaiting_results() == true)){
			testing_results_transitions_flu(agent, agents, flu, time, infection, households, 
						schools, workplaces, hospitals, retirement_homes, infection_parameters, testing);
			if (agent.tested_covid_negative()){
				state_changes.at(2) = 1;
			}
			if (agent.tested_false_positive()){
				state_changes.at(3) = 1;
			}
		}
		// If recovering and leaving home isolation - reset all flags, swap with 
		// new flu and return back to regular susceptible
		if (agent.tested_false_positive() && agent.home_isolated() 
						&& agent.get_recovery_time() <= time){ 
			states_manager.reset_returning_flu(agent);
			add_to_all_workplaces_and_schools(agent, schools, workplaces, 
							retirement_homes);
			int new_flu = flu.swap_flu_agent(agent.get_ID());
			// If still available
			if (new_flu != -1){
				process_new_flu(agents.at(new_flu-1), hospitals.size(), time, schools, workplaces, 
							retirement_homes, infection, infection_parameters, flu, testing);
			}
		}
	}	
	return state_changes;	
}

// Set properties related to newly created agent with flu, including testing
void FluTransitions::process_new_flu(Agent& agent, const int n_hospitals, const double time, 
			   		std::vector<School>& schools, std::vector<Workplace>& workplaces,
					std::vector<RetirementHome>& retirement_homes,
					Infection& infection, const std::map<std::string, double>& infection_parameters, 
					Flu& flu, const Testing& testing)
{
	double test_time = 0.0;
	agent.set_symptomatic_non_covid(true);
	// Testing properties
	if (flu.getting_tested(testing)){
		// Home isolation for all tested
		remove_from_all_workplaces_and_schools(agent, schools, workplaces, retirement_homes);
		if (infection.tested_in_hospital(infection_parameters.at("fraction tested in hospitals"))){
			states_manager.set_waiting_for_test_in_hospital(agent);
			int hsp_ID = infection.get_random_hospital_ID(n_hospitals);
			// Registration will happen only upon testing time step
			agent.set_hospital_ID(hsp_ID);
		} else {
			states_manager.set_waiting_for_test_in_car(agent);
		}
		// Set testing times
		test_time = infection.wait_time_for_test(infection_parameters.at("flu testing duration"));
		agent.set_time_to_test(test_time);
		agent.set_time_of_test(time);
		// Delay home isolation until fixed number of days before test
		agent.set_home_isolated(false);
		// Time to start isolation 
		agent.set_flu_isolation(infection_parameters.at("time from decision to test"));
	}
}

// Return total lambda of susceptible agent 
double FluTransitions::compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Household>& households, const std::vector<School>& schools,
					const std::vector<Workplace>& workplaces, const std::vector<Hospital>& hospitals, 
					const std::vector<RetirementHome>& retirement_homes)			
{
	double lambda_tot = 0.0;

	// Regular susceptible agent
	
	if (agent.retirement_home_resident()){
		const RetirementHome& rh = retirement_homes.at(agent.get_household_ID()-1);
		lambda_tot = rh.get_infected_contribution();
		return lambda_tot;
	}

	const Household& house = households.at(agent.get_household_ID()-1);
	if (agent.student() && agent.works()){
		const School& school = schools.at(agent.get_school_ID()-1);
		if (agent.retirement_home_employee()){
			const RetirementHome& rh = retirement_homes.at(agent.get_work_ID()-1);
			lambda_tot = house.get_infected_contribution()+ 
						rh.get_infected_contribution()+
						school.get_infected_contribution();
		}else if (agent.school_employee()){
			const School& workSch = schools.at(agent.get_work_ID()-1);
			lambda_tot = house.get_infected_contribution()+ 
						workSch.get_infected_contribution()+
						school.get_infected_contribution();
		} else {
			const Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
			lambda_tot = house.get_infected_contribution()+ 
						workplace.get_infected_contribution()+
						school.get_infected_contribution();
		}
	} else if (agent.student()){
		const School& school = schools.at(agent.get_school_ID()-1);
		lambda_tot = house.get_infected_contribution()+ 
						school.get_infected_contribution();
	} else if (agent.works()){
		if (agent.retirement_home_employee()){
			const RetirementHome& rh = retirement_homes.at(agent.get_work_ID()-1);
			lambda_tot = house.get_infected_contribution()+ 
						rh.get_infected_contribution();
		} else if (agent.school_employee()){
			const School& school = schools.at(agent.get_work_ID()-1);
			lambda_tot = house.get_infected_contribution()+ 
						school.get_infected_contribution();
		} else {
			const Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
			lambda_tot = house.get_infected_contribution()+ 
						workplace.get_infected_contribution();
		}
	} else {
		lambda_tot = house.get_infected_contribution();
	}	
	// Overwrite if being tested or in home isolation
	if ((agent.tested()) && (agent.tested_in_hospital()) 
			&& (agent.get_time_of_test() <= time)
			&& (agent.tested_awaiting_test() == true)){
		const Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
		lambda_tot = hospital.get_infected_contribution();
	} else if (agent.home_isolated() == true){
		// Otherwise if waiting for test or results - home isolation
		lambda_tot =  house.get_infected_contribution();
	}
	return lambda_tot;
}

// Compte and set agent properties related to recovery without symptoms and incubation 
void FluTransitions::recovery_and_incubation(Agent& agent, Infection& infection, const double time,
				const std::map<std::string, double>& infection_parameters)
{
	// Determine if agent will recover without
	// becoming symptomatic and update corresponding states
	bool never_sy = infection.recovering_exposed(agent.get_age());

	// Total latency period
	double latency = infection.latency();
	// Portion of latency when the agent is not infectious
	double dt_ninf = std::min(infection_parameters.at("time from exposed to infectiousness"), latency);

	if (never_sy){
		states_manager.set_susceptible_to_exposed_never_symptomatic(agent);
		// Set to total latency + infectiousness duration
		double rec_time = infection_parameters.at("recovery time");
		agent.set_latency_duration(latency + rec_time);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}else{
		// If latency shorter, then  not infectious during the entire latency
		states_manager.set_susceptible_to_exposed(agent);
		agent.set_latency_duration(latency);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}
}

// Determine any testing related properties
void FluTransitions::set_testing_status(Agent& agent, Infection& infection, const double time, 
										std::vector<School>& schools, std::vector<Workplace>& workplaces, 
										std::vector<Hospital>& hospitals, std::vector<RetirementHome>& retirement_homes,
										const std::map<std::string, double>& infection_parameters,
										const Testing& testing)
{
	const int n_hospitals = hospitals.size();
	bool will_be_tested = false;

	// Different probability and settings for exposed
	if (agent.exposed() == true){
 		will_be_tested = infection.will_be_tested(testing.get_exp_tested_prob());
		if (will_be_tested == true){
			// Determine type of testing
			if (infection.tested_in_hospital(infection_parameters.at("fraction tested in hospitals"))){
				states_manager.set_exposed_waiting_for_test_in_hospital(agent);
				int hsp_ID = infection.get_random_hospital_ID(n_hospitals);
				// Registration will happen only upon testing time step
				agent.set_hospital_ID(hsp_ID);
			} else {
				states_manager.set_exposed_waiting_for_test_in_car(agent);
			}
			// Home isolation - removal from all public places 
			remove_from_all_workplaces_and_schools(agent, schools, workplaces, retirement_homes);
			// Time to test
			agent.set_time_to_test(infection_parameters.at("time from decision to test"));
			agent.set_time_of_test(time);
		}
	} 
}

// Non-covid symptomatic testing changes
void FluTransitions::testing_transitions_flu(Agent& agent, const double time,
										const std::map<std::string, double>& infection_parameters)
{
	// Determine the time agent gets results
	agent.set_time_until_results(infection_parameters.at("time from test to results"));
	agent.set_time_of_results(time);
	states_manager.set_tested_to_awaiting_results(agent);
}

// Non-covid symptomatic agent transitions upon receiving test results 
void FluTransitions::testing_results_transitions_flu(Agent& agent, 
			std::vector<Agent>& agents, Flu& flu,
			const double time, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			std::vector<RetirementHome>& retirement_homes,
			const std::map<std::string, double>& infection_parameters, const Testing& testing)
{
	// If false positive, put under home isolation 
	double fneg_prob = infection_parameters.at("fraction false positive");
	if (infection.false_positive_test_result(fneg_prob) == true){
		states_manager.set_tested_false_positive(agent);
		agent.set_recovery_duration(infection_parameters.at("recovery time"));
		agent.set_recovery_time(time);	
	} else { 		
		// If confirmed negative, release the isolation	
		// and remove from flu, add new flu agent
		states_manager.set_tested_negative(agent);
		add_to_all_workplaces_and_schools(agent, schools, workplaces, retirement_homes);
		int new_flu = flu.swap_flu_agent(agent.get_ID());
		// If still available
		if (new_flu != -1){
			process_new_flu(agents.at(new_flu-1), hospitals.size(), time, schools, workplaces, 
				retirement_homes, infection, infection_parameters, flu, testing);
		}
	}
}

// Remove agent's index from all workplaces and schools that have them registered
void FluTransitions::remove_from_all_workplaces_and_schools(Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces, std::vector<RetirementHome>& retirement_homes)
{
	// Else remove depending on status	
	int agent_ID = agent.get_ID();
	if (agent.student()){
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
	}
	if (agent.works()){
		if (agent.retirement_home_employee()){
			retirement_homes.at(agent.get_work_ID()-1).remove_agent(agent_ID);
		} else if (agent.school_employee()){
			schools.at(agent.get_work_ID()-1).remove_agent(agent_ID);
		} else {
			workplaces.at(agent.get_work_ID()-1).remove_agent(agent_ID);
		}
	}
}

// Add to all from places where they are registered
void FluTransitions::add_to_all_workplaces_and_schools(Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces, std::vector<RetirementHome>& retirement_homes)
{
	int agent_ID = agent.get_ID();
	if (agent.student()){
		schools.at(agent.get_school_ID()-1).add_agent(agent_ID);
	}
	if (agent.works()){
		if (agent.retirement_home_employee()){
			retirement_homes.at(agent.get_work_ID()-1).add_agent(agent_ID);
		} else if (agent.school_employee()){
			schools.at(agent.get_work_ID()-1).add_agent(agent_ID);
		} else {
			workplaces.at(agent.get_work_ID()-1).add_agent(agent_ID);
		}
	}
}


