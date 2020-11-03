#include "../../include/transitions/regular_transitions.h"

/***************************************************** 
 * class: RegularTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for a most general
 * case of agent 
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
int RegularTransitions::susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				std::vector<RetirementHome>& retirement_homes,
				const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, Flu& flu, const Testing& testing)
{
	double lambda_tot = 0.0;
	int got_infected = 0;
	lambda_tot = compute_susceptible_lambda(agent, time, households, schools, workplaces, retirement_homes);
	if (infection.infected(lambda_tot) == true){
		// Remove agent from potential flu population
		flu.remove_susceptible_agent(agent.get_ID());
		got_infected = 1;
		agent.set_inf_variability_factor(infection.inf_variability());
		// Infectiousness, latency, and possibility of never developing symptoms 
		recovery_and_incubation(agent, infection, time, infection_parameters);
		// Determine if getting tested, how, and when
		// Remove agent from places if under home isolation
		if (testing.started(time)){
			set_testing_status(agent, infection, time, schools, 
							workplaces, hospitals, retirement_homes, infection_parameters, testing);
		}
	}	
	return got_infected;	
}

// Return total lambda of susceptible agent 
double RegularTransitions::compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Household>& households, const std::vector<School>& schools,
					const std::vector<Workplace>& workplaces, 
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
		} else if (agent.school_employee()){
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

	return lambda_tot;
}

// Compte and set agent properties related to recovery without symptoms and incubation 
void RegularTransitions::recovery_and_incubation(Agent& agent, Infection& infection, const double time,
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

// Implement transitions relevant to exposed 
std::vector<int> RegularTransitions::exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<School>& schools,
										std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
										std::vector<RetirementHome>& retirement_homes,
										const std::map<std::string, double>& infection_parameters, const Testing& testing)
{
	// Recovered, dead (not applicable), tested, tested positive, tested false negative
	std::vector<int> state_changes(5,0);

	// First check for testing because that holds for transition changes too
	// If being tested
	if ((agent.tested()) && (agent.get_time_of_test() <= time)
			&& (agent.tested_awaiting_test() == true)){
		testing_transitions(agent, time, infection_parameters);
		state_changes.at(2) = 1;
	}
	// If getting test results (this may in principle happen in a
	// single step)
	if ((agent.tested()) && (agent.get_time_of_results() <= time)
			&& (agent.tested_awaiting_results() == true)){
		testing_results_transitions(agent, time, dt, infection, households, 
					schools, workplaces, hospitals, retirement_homes, infection_parameters);
		if (agent.tested_covid_positive()){
			state_changes.at(3) = 1;
		}
		if (agent.tested_false_negative()){
			state_changes.at(4) = 1;
		}
	}

	// Check if latency time is over
	int agent_recovered = 0;
	if (agent.get_latency_end_time() <= time){
		// Attempt retesting unless confirmed positive or not waiting for test/results
		bool check_for_test = (agent.tested() == false) && (agent.tested_covid_positive() == false);
		// Reovering without symptoms - remove
		if (agent.recovering_exposed()){
			if (agent.home_isolated()){
				add_to_all_workplaces_and_schools(agent, schools, workplaces, retirement_homes);
			}
			states_manager.set_exposed_never_symptomatic_to_removed(agent);
			agent_recovered = 1;
		} else {
			// Transition to symptomatic
			states_manager.set_exposed_to_symptomatic(agent);
			// Determine if agent will get tested
			if (check_for_test == true){
				untested_sy_setup(agent, infection, time, dt, households, 
					schools, workplaces, hospitals, retirement_homes, infection_parameters, testing);
			} else {
				recovery_status(agent, infection, time, infection_parameters);
				// If agent tested positive - select initial treatment for symptomatic
				// In that stage they are already IH
				if (agent.tested_covid_positive()){
					select_initial_treatment(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, retirement_homes, infection_parameters);
				} else {
					// This is to arange initial treatment differently
					agent.set_testing_since_exposed(true);
				}
			}
		}
	}
	state_changes.at(0) = agent_recovered;
	return state_changes;
}

// Determine testing status, treatment choices, and recovery 
void RegularTransitions::untested_sy_setup(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<School>& schools,
										std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
										std::vector<RetirementHome>& retirement_homes,
										const std::map<std::string, double>& infection_parameters,
										const Testing& testing)
{
		// Testing status
		bool will_be_tested = false;
		// Treatment requirements
		bool needs_hospitalization = false, needs_icu = false, needs_home_isolation = false;

		if (testing.started(time)){
			will_be_tested = set_testing_status(agent, infection, time, schools, 
							workplaces, hospitals, retirement_homes, infection_parameters, testing);
		}

		// Determine required treatment
		if (infection.agent_hospitalized(agent.get_age())){
			if (infection.agent_hospitalized_ICU(agent.get_age())){
				needs_icu = true;
			}else{
				needs_hospitalization = true;
			}
		}else{
			needs_home_isolation = true;
		}
		
		// Determine removal based on treatment and testing choices
		if (will_be_tested == false){
			// Dying if not getting tested and does not need ICU
			if (needs_icu){
				states_manager.set_dying_symptomatic(agent);			
				agent.set_time_to_death(infection.time_to_death());
				agent.set_death_time(time);
			} else {
				recovery_status(agent, infection, time, infection_parameters);
			}
		}else{
			// Set the treatment that will follow testing
			agent.to_be_hospitalized(needs_hospitalization);
			agent.to_be_in_ICU(needs_icu);
		   	agent.to_be_home_isolated(needs_home_isolation);
			// Determine if dying
			if (agent.get_will_be_hospitalized_ICU()){
				recovery_status_ICU(agent, infection, time, infection_parameters);
			}else{
				recovery_status(agent, infection, time, infection_parameters);
			}
		}
}

// Determine if the agent is recovering or dying 
void RegularTransitions::recovery_status(Agent& agent, Infection& infection, const double time,
			const std::map<std::string, double>& infection_parameters)
{
	int agent_age = agent.get_age();
	if (infection.will_die_non_icu(agent_age)){
		states_manager.set_dying_symptomatic(agent);			
		agent.set_time_to_death(infection.time_to_death());
		agent.set_death_time(time);
	} else {
		states_manager.set_recovering_symptomatic(agent);			
		agent.set_recovery_duration(infection_parameters.at("recovery time"));
		agent.set_recovery_time(time);		
	}
}

// Determine if the agent is recovering or dying in ICU 
void RegularTransitions::recovery_status_ICU(Agent& agent, Infection& infection, const double time,
			const std::map<std::string, double>& infection_parameters)
{
	if (infection.will_die_ICU()){
		states_manager.set_dying_symptomatic(agent);
		agent.set_time_to_death(infection.time_to_death());
		agent.set_death_time(time);
	} else {
		states_manager.set_recovering_symptomatic(agent);			
		agent.set_recovery_duration(infection_parameters.at("recovery time"));
		agent.set_recovery_time(time);		
	}
}

// Determine any testing related properties
bool RegularTransitions::set_testing_status(Agent& agent, Infection& infection, const double time, 
										std::vector<School>& schools, std::vector<Workplace>& workplaces, 
										std::vector<Hospital>& hospitals,
										std::vector<RetirementHome>& retirement_homes,
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
	} else if (agent.symptomatic()) {
 		will_be_tested = infection.will_be_tested(testing.get_sy_tested_prob());
		if (will_be_tested == true){
			// If agent is getting tested - determine type and properties of testing
			if (infection.tested_in_hospital(infection_parameters.at("fraction tested in hospitals"))){
				states_manager.set_waiting_for_test_in_hospital(agent);
				int hsp_ID = infection.get_random_hospital_ID(n_hospitals);
				// Registration will happen only upon testing time step
				agent.set_hospital_ID(hsp_ID);	
			} else {
				states_manager.set_waiting_for_test_in_car(agent);
			}
	
			// Testing-related events - will be adjusted based on other time-dependent scenarios
			agent.set_time_to_test(infection_parameters.at("time from decision to test"));
			agent.set_time_of_test(time);
	
			// Home isolation - removal from all public places except hospitals for former
			// patients and employees (removed previously because that holds for all of them)
			remove_from_all_workplaces_and_schools(agent, schools, workplaces, retirement_homes);
		}
	}
 	return will_be_tested;	
}

// Transitions of a symptomatic agent 
std::vector<int> RegularTransitions::symptomatic_transitions(Agent& agent, const double time, 
				   	const double dt, Infection& infection,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes,
					const std::map<std::string, double>& infection_parameters)
{
	std::vector<int> state_changes(5,0);
	int tested_pos = 0;
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	removed = check_agent_removal(agent, time, households, schools, workplaces, hospitals, retirement_homes);
	state_changes.at(0) = removed.at(0);
	state_changes.at(1) = removed.at(1);
	if (agent.removed() == true){
		if (state_changes.at(1) == 1){
			// Correct for not tested or false negative and not treated
			// The not treated is equal to not confirmed positive
			if (agent.tested_covid_positive() == false){
				state_changes.at(1) = 2;		
			}
		}	
		return state_changes;
	}
	if (agent.tested_false_negative() == true){
		return state_changes;
	}

	// If being tested at this time step
	if ((agent.tested()) && (agent.get_time_of_test() <= time)
			&& (agent.tested_awaiting_test() == true)){
		testing_transitions(agent, time, infection_parameters);
		state_changes.at(2) = 1;
		return state_changes;
	}

	// If getting test results 	
	if ((agent.tested()) && (agent.get_time_of_results() <= time)
			&& (agent.tested_awaiting_results() == true)){
		tested_pos = testing_results_transitions(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, retirement_homes, infection_parameters);
		state_changes.at(3) = tested_pos;
		return state_changes;
	}

	// Treatment transitions (also possible in a single step)
	if (agent.being_treated()){
		treatment_transitions(agent, time, dt, infection, households, 
							hospitals, retirement_homes, infection_parameters);
	}

	return state_changes;
}

// Verify if agent is to be removed at this step
std::vector<int> RegularTransitions::check_agent_removal(Agent& agent, const double time,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	// If dying
	if (agent.dying() == true){
		if (agent.get_time_of_death() <= time){
			removed.at(1) = 1;
			remove_agent_from_all_places(agent, households, schools, workplaces, hospitals, retirement_homes);
			states_manager.set_any_to_removed(agent);
		}
	}
	// If recovering
	if (agent.recovering() == true){
		if (agent.get_recovery_time() <= time){
			removed.at(0) = 1;
			// If any of these categories - return to regular public places/households
			if (agent.tested() || agent.being_treated()){
				add_agent_to_all_places(agent, households, schools, workplaces, hospitals, retirement_homes);
			}
			states_manager.set_any_to_removed(agent);
		}
	}
	return removed;
}

// Agent transitions related to testing time
void RegularTransitions::testing_transitions(Agent& agent, const double time,
										const std::map<std::string, double>& infection_parameters)
{
	// Determine the time agent gets results
	agent.set_time_until_results(infection_parameters.at("time from test to results"));
	agent.set_time_of_results(time);
	states_manager.set_tested_to_awaiting_results(agent);
}

// Agent transitions upon receiving test results 
int RegularTransitions::testing_results_transitions(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			std::vector<RetirementHome>& retirement_homes,
			const std::map<std::string, double>& infection_parameters)
{
	// If false negative, remove testing, put back to exposed
	// No false negative symptomatic 
	double fneg_prob = infection_parameters.at("fraction false negative");
	int tested_pos = 0;	
	if (infection.false_negative_test_result(fneg_prob) == true
		 && agent.exposed() == true){
		states_manager.set_tested_false_negative(agent);
		add_to_all_workplaces_and_schools(agent, schools, workplaces, retirement_homes);
	} else {
		// If confirmed positive
		tested_pos = 1;
		// Exposed - keep in home isolation until symptomatic
		if (agent.exposed()){
			states_manager.set_home_isolation(agent);
			agent.set_tested_covid_positive(true);		
		} else {
			// Symptomatic - identify treatment
			// For ones that were already setup for testing as exposed
			if (agent.get_testing_since_exposed()){
				select_initial_treatment(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, retirement_homes, infection_parameters);
			} else {
				// Others - treatment already chosen
				setup_initial_treatment(agent, time, dt, infection, households, 
						schools, workplaces, hospitals, retirement_homes,
						infection_parameters);
			}
			agent.set_tested_covid_positive(true);
		}
	}
	return tested_pos;	
}

// Determine type of intial treatement and its properties
void RegularTransitions::select_initial_treatment(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			std::vector<RetirementHome>& retirement_homes,
			const std::map<std::string, double>& infection_parameters)
{
	if (infection.agent_hospitalized(agent.get_age()) == true){
		
		// Remove agent from all places, then add to a random
		// hospital for treatment
		remove_agent_from_all_places(agent, households, schools, workplaces, 
						hospitals, retirement_homes);
		
		// Set hospital ID and add
		int hID = infection.get_random_hospital_ID(hospitals.size());
		agent.set_hospital_ID(hID);
		hospitals.at(hID-1).add_agent(agent.get_ID());
		// ICU
		if (infection.agent_hospitalized_ICU(agent.get_age()) == true){
			// Retest for dying
			if (infection.will_die_ICU()){
				states_manager.set_icu_dying(agent);
				agent.set_time_to_death(infection.time_to_death());
				agent.set_death_time(time);
			}else{
				// If recovering - set times and transitions
				states_manager.set_icu_recovering(agent);
				// Reset the recovery time to > ICU + hospitalization
				double t_icu = infection_parameters.at("time in ICU");
				double t_hsp_icu = infection_parameters.at("time in hospital after ICU");
				agent.set_time_icu_to_hsp(time + t_icu);
			   	agent.set_time_hsp_to_ih(time + t_icu + t_hsp_icu);	
				agent.set_recovery_duration(t_icu + t_hsp_icu);
				agent.set_recovery_time(time);
			}
		}else{
			// Hospitalized
			states_manager.set_hospitalized(agent);
			// If dying, set transition to ICU
			if (agent.dying() == true){
				double dt_icu = infection_parameters.at("time before death to ICU");
				double t_icu = std::max(agent.get_time_of_death() - dt_icu, time + dt_icu);
				agent.set_time_hsp_to_icu(t_icu);	
			}else{
				// If recovering, set transition to home
				double t_rh = agent.get_recovery_time();
				double del_t_hsp = infection_parameters.at("time in hospital");
				double t_hsp = time + del_t_hsp; 
				if (t_rh > t_hsp){
					agent.set_time_hsp_to_ih(t_hsp);
				}else{
					agent.set_time_hsp_to_ih(t_hsp);
					agent.set_recovery_duration(del_t_hsp);
					agent.set_recovery_time(time);
				}
			}
		}
	}else{
		// If home isolated
		states_manager.set_home_isolation(agent);
		// If dying, set transition to ICU
		if (agent.dying() == true){
			double dt_icu = infection_parameters.at("time before death to ICU");
			double t_icu = std::max(agent.get_time_of_death() - dt_icu, time + dt_icu);
			agent.set_time_ih_to_icu(t_icu);	
		}else{
			// If recovering, determine possible transition to hospital
			double t_rh = agent.get_recovery_time();
			double t_hsp = time + infection.get_onset_to_hospitalization();
			// Only if transition is later than a step away and earlier than
			// the recovery time
			if ((t_rh > t_hsp) && (t_hsp > time + dt)){
				agent.set_time_ih_to_hsp(t_hsp);
			}else{
				// Set to past recovery
				agent.set_time_ih_to_hsp(2.0*t_rh);
			}
		}
	}
}

// Setup intial treatement properties
void RegularTransitions::setup_initial_treatment(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			std::vector<RetirementHome>& retirement_homes,
			const std::map<std::string, double>& infection_parameters)
{
	if (agent.get_will_be_hospitalized() || agent.get_will_be_hospitalized_ICU()){
		// Remove agent from all places, then add to a random
		// hospital for treatment
		remove_agent_from_all_places(agent, households, schools, workplaces, 
						hospitals, retirement_homes);
		
		// Set hospital ID and add
		int hID = infection.get_random_hospital_ID(hospitals.size());
		agent.set_hospital_ID(hID);
		hospitals.at(hID-1).add_agent(agent.get_ID());
		if (agent.get_will_be_hospitalized_ICU()){
			if (agent.dying()){
				states_manager.set_icu_dying(agent);
			}else{
				// If recovering - set times and transitions
				states_manager.set_icu_recovering(agent);
				// Reset the recovery time to > ICU + hospitalization
				double t_icu = infection_parameters.at("time in ICU");
				double t_hsp_icu = infection_parameters.at("time in hospital after ICU");
				agent.set_time_icu_to_hsp(time + t_icu);
				agent.set_time_hsp_to_ih(time + t_icu + t_hsp_icu);	
				agent.set_recovery_duration(t_icu + t_hsp_icu);
				agent.set_recovery_time(time);
			}
		} else {
			// Hospitalized
			states_manager.set_hospitalized(agent);
			// If dying, set transition to ICU
			if (agent.dying() == true){
				double dt_icu = infection_parameters.at("time before death to ICU");
				double t_icu = std::max(agent.get_time_of_death() - dt_icu, time + dt_icu);
				agent.set_time_hsp_to_icu(t_icu);	
			}else{
				// If recovering, set transition to home
				double t_rh = agent.get_recovery_time();
				double del_t_hsp = infection_parameters.at("time in hospital");
				double t_hsp = time + del_t_hsp; 
				if (t_rh > t_hsp){
					agent.set_time_hsp_to_ih(t_hsp);
				}else{
					agent.set_time_hsp_to_ih(t_hsp);
					agent.set_recovery_duration(del_t_hsp);
					agent.set_recovery_time(time);
				}
			}
		}
	} else if (agent.get_will_be_home_isolated()){
		// If home isolated
		states_manager.set_home_isolation(agent);
		// If dying, set transition to ICU
		if (agent.dying() == true){
			double dt_icu = infection_parameters.at("time before death to ICU");
			double t_icu = std::max(agent.get_time_of_death() - dt_icu, time + dt_icu);
			agent.set_time_ih_to_icu(t_icu);	
		}else{
			// If recovering, determine possible transition to hospital
			double t_rh = agent.get_recovery_time();
			double t_hsp = time + infection.get_onset_to_hospitalization();
			// Only if transition is later than a step away and earlier than
			// the recovery time
			if ((t_rh > t_hsp) && (t_hsp > time + dt)){
				agent.set_time_ih_to_hsp(t_hsp);
			}else{
				// Set to past recovery
				agent.set_time_ih_to_hsp(2.0*t_rh);
			}
		}
	} else {
		throw std::runtime_error("Regular symptomatic agent that was tested does not have treatment status.");
	}		
}

// Determine treatment changes 
void RegularTransitions::treatment_transitions(Agent& agent, const double time, 
			const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<Hospital>& hospitals,
			std::vector<RetirementHome>& retirement_homes,
			const std::map<std::string, double>& infection_parameters)
{
	// ICU - can only transition to hospitalization
	// if not dying
	if (agent.recovering() && agent.hospitalized_ICU()){
		if (agent.get_time_icu_to_hsp() <= time){
			agent.set_hospitalized_ICU(false);
			agent.set_hospitalized(true);
		}
	}else if (agent.hospitalized()){
		if (agent.dying()){
			if (agent.get_time_hsp_to_icu() <= time){
				// Transition to ICU
				agent.set_hospitalized(false);
				agent.set_hospitalized_ICU(true);
			}	
		}else{		
			if (agent.get_time_hsp_to_ih() <= time){
				// Transition to home isolation
				agent.set_hospitalized(false);
				agent.set_home_isolated(true);
				// Remove from hospital and add to household
				int agent_ID = agent.get_ID();
				if (agent.retirement_home_resident()){
					retirement_homes.at(agent.get_household_ID()-1).add_agent(agent_ID);
				} else {
			        households.at(agent.get_household_ID()-1).add_agent(agent_ID);
				}
		        // Remove agent from hospital
		        hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
			}
		}
	}else if (agent.home_isolated()){
		if (agent.dying()){
			// Will end up in ICU
			if (agent.get_time_ih_to_icu() <= time){
				// Set hospital ID, add to hospital 
				int hID = infection.get_random_hospital_ID(hospitals.size());
				agent.set_hospital_ID(hID);
				hospitals.at(hID-1).add_agent(agent.get_ID());
				// Remove from home
				if (agent.retirement_home_resident()){
					retirement_homes.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
				} else {
			        households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
				}
				agent.set_home_isolated(false);
				agent.set_hospitalized(false);
				agent.set_hospitalized_ICU(true);	
			}
		} else {
			double t_hsp = agent.get_time_ih_to_hsp();
			// Comparing to dt too, it is currently minimum 
			// for hospitalization; for now assuming
			// no ICU for recovering - hospitalized
			if ( (t_hsp >= dt) && (t_hsp <= time) ){
				agent.set_home_isolated(false);
				agent.set_hospitalized(true);
				// Set hospital ID 
				int hID = infection.get_random_hospital_ID(hospitals.size());
				agent.set_hospital_ID(hID);
				hospitals.at(hID-1).add_agent(agent.get_ID());
				// Remove from home
				if (agent.retirement_home_resident()){
					retirement_homes.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
				} else {
			        households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
				}
				// Set transition back
				double t_rh = agent.get_recovery_time();
				double del_t_hsp = infection_parameters.at("time in hospital");
				double t_hsp = time + del_t_hsp; 
				if (t_rh > t_hsp){
					agent.set_time_hsp_to_ih(t_hsp);
				}else{
					agent.set_time_hsp_to_ih(t_hsp);
					agent.set_recovery_duration(del_t_hsp);
					agent.set_recovery_time(time);
				}
			}
		}		
	}	
}

// Remove agent's ID from places where they are registered
void RegularTransitions::remove_agent_from_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes)
{
	// No error if already removed due to hospitalization
	int agent_ID = agent.get_ID();

	int hs_ID = agent.get_household_ID();
	if (hs_ID > 0){
		if (agent.retirement_home_resident()){
			retirement_homes.at(agent.get_household_ID()-1).remove_agent(agent_ID);
		} else {
		    households.at(agent.get_household_ID()-1).remove_agent(agent_ID);
		}
	} else {
		throw std::runtime_error("Regular symptomatic agent does not have a valid household ID");
	}
	
	if (agent.hospitalized() || agent.hospitalized_ICU()){
		hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
	}

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
void RegularTransitions::add_to_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces,
							std::vector<RetirementHome>& retirement_homes)
{
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).add_agent(agent_ID);
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

// Remove agent's index from all workplaces and schools that have them registered
void RegularTransitions::remove_from_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces,
							std::vector<RetirementHome>& retirement_homes)
{
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
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

// Add agent's ID back to the places where they are registered
// This is done to keep realistic numbers of people in different places
// which influences the probability
void RegularTransitions::add_agent_to_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes)
{
	int agent_ID = agent.get_ID();
	// ICU should always transition to hospital first
	if (agent.hospitalized_ICU())
		throw std::runtime_error("Attempting recovery of an agent directly from ICU");
	// Remove from all not to count twice
	if (agent.hospitalized()){
		hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
	}
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
	// Hospitalized was the only possibility where infected agent was not
	// associated with a household
	if (agent.hospitalized()){
		if (agent.retirement_home_resident()){
			retirement_homes.at(agent.get_household_ID()-1).add_agent(agent_ID);
		} else {
		    households.at(agent.get_household_ID()-1).add_agent(agent_ID);
		}
	}
}


