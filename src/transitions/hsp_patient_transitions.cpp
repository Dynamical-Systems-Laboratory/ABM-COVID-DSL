#include "../../include/transitions/hsp_patient_transitions.h"

/***************************************************** 
 * class: HspPatientTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for agent that is 
 * a hospital patient and does not initially have
 * COVID 
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
int HspPatientTransitions::susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Hospital>& hospitals, const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, const Testing& testing)
{
	double lambda_tot = 0.0;
	int got_infected = 0;

	lambda_tot = compute_susceptible_lambda(agent, time, hospitals);

	if (infection.infected(lambda_tot) == true){
		got_infected = 1;
		agent.set_inf_variability_factor(infection.inf_variability());
		// Infectiousness, latency, and possibility of never developing 
		// symptoms 
		recovery_and_incubation(agent, infection, time, infection_parameters);
		// Determine if getting tested, how, and when
		// Remove agent from places if under home isolation
		if (testing.started(time)){
			set_testing_status(agent, infection, time, hospitals, infection_parameters, testing);
		}
	} 
	return got_infected;	
}

// Return total lambda of susceptible agent 
double HspPatientTransitions::compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Hospital>& hospitals)			
{
	double lambda_tot = 0.0;
	// Count only hospital
	const Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
	lambda_tot = hospital.get_infected_contribution();	
	return lambda_tot;
}

// Compte and set agent properties related to recovery without symptoms and incubation 
void HspPatientTransitions::recovery_and_incubation(Agent& agent, Infection& infection, const double time,
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
std::vector<int> HspPatientTransitions::exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<Hospital>& hospitals, 
										const std::map<std::string, double>& infection_parameters, const Testing& testing)
{
	std::vector<int> state_changes(5,0);
	// Modified mortality rate for hospital patients
	const bool is_hsp = true;

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
						hospitals, infection_parameters);
		if (agent.tested_covid_positive())
			state_changes.at(3) = 1;
		if (agent.tested_false_negative())
			state_changes.at(4) = 1;
	}

	// Check if latency time is over
	int agent_recovered = 0;
	if (agent.get_latency_end_time() <= time){
		// Reovering without symptoms - remove
		if (agent.recovering_exposed()){
			if (agent.home_isolated()){
				add_agent_to_all_places(agent, households, hospitals);
			}
			states_manager.set_exposed_never_symptomatic_to_removed(agent);
			agent_recovered = 1;
		} else {
			// Transition to symptomatic
			// Adjust flags - common, overwrite for specific
			states_manager.set_exposed_to_symptomatic(agent);
			// Removal settings		
			int agent_age = agent.get_age();
			if (infection.will_die_non_icu(agent_age, is_hsp)){
				states_manager.set_dying_symptomatic(agent);			
				agent.set_time_to_death(infection.time_to_death());
				agent.set_death_time(time);
			} else {
				states_manager.set_recovering_symptomatic(agent);			
				// This may change if treatment is ICU
				agent.set_recovery_duration(infection_parameters.at("recovery time"));
				agent.set_recovery_time(time);		
			}
			// Determine testing time and set home isolation - if not yet confirmed and IH
			if (agent.tested_covid_positive() == false){
				// If not waiting for test or results
				// otherwise don't change
				if ((agent.tested()) == false && (testing.started(time))){
					set_testing_status(agent, infection, time, hospitals, infection_parameters, testing);
				}
			}else{
				// If already confirmed positive and IH - determine treatment
				select_initial_treatment(agent, time, dt, infection, households, 
											hospitals, infection_parameters);
			}
		}
	}
	state_changes.at(0) = agent_recovered;
	return state_changes;
}

// Determine any testing related properties
void HspPatientTransitions::set_testing_status(Agent& agent, Infection& infection, const double time, 
										std::vector<Hospital>& hospitals, 
										const std::map<std::string, double>& infection_parameters,
										const Testing& testing)
{
	const int n_hospitals = hospitals.size();
	bool will_be_tested = false;

	// Different probability for exposed
	if (agent.exposed() == true){
 		will_be_tested = infection.will_be_tested(testing.get_exp_tested_prob());
		if (will_be_tested == true){
			agent.set_tested_exposed(true);
			// Assuming hospital patients test in their respective hospitals
			// Also - no home isolation until symptoms
			states_manager.set_exposed_waiting_for_test_in_hospital(agent);
			// Time to test
			agent.set_time_to_test(infection_parameters.at("time from decision to test"));
			agent.set_time_of_test(time);
		}
	} else if (agent.symptomatic()) {
		// Again - testing in the workplace - hospital
		// with home isolation set elsewhere
		states_manager.set_waiting_for_test_in_hospital(agent);
		// Will stay in the hospital
		agent.set_home_isolated(false);
		// Testing-related events - will be adjusted based on other time-dependent scenarios
		agent.set_time_to_test(infection_parameters.at("time from decision to test"));
		agent.set_time_of_test(time);
	}
}

// Transitions of a symptomatic agent 
std::vector<int> HspPatientTransitions::symptomatic_transitions(Agent& agent, const double time, 
				   	const double dt, Infection& infection,
					std::vector<Household>& households, std::vector<Hospital>& hospitals,
					const std::map<std::string, double>& infection_parameters)
{
	std::vector<int> state_changes(5,0);
	int tested_pos = 0;
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	removed = check_agent_removal(agent, time, households, hospitals);
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

	// If being tested
	if ((agent.tested()) && (agent.get_time_of_test() <= time)
			&& (agent.tested_awaiting_test() == true)){
		testing_transitions(agent, time, infection_parameters);
		state_changes.at(2) = 1;
		return state_changes;
	}

	// If getting test results (this may in principle happen in a
	// single step)
	if ((agent.tested()) && (agent.get_time_of_results() <= time)
			&& (agent.tested_awaiting_results() == true)){
		tested_pos = testing_results_transitions(agent, time, dt, infection, households, 
						hospitals, infection_parameters);
		state_changes.at(3) = tested_pos;
		return state_changes;
	}
	
	// Treatment transitions (also possible in a single step)
	if (agent.being_treated())
		treatment_transitions(agent, time, dt, infection, households, 
							hospitals, infection_parameters);
	return state_changes;
}

// Verify if agent is to be removed at this step
std::vector<int> HspPatientTransitions::check_agent_removal(Agent& agent, const double time,
					std::vector<Household>& households, 
					std::vector<Hospital>& hospitals)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	// If dying
	if (agent.dying() == true){
		if (agent.get_time_of_death() <= time){
			removed.at(1) = 1;
			remove_agent_from_all_places(agent, households, hospitals);
			states_manager.set_any_to_removed(agent);
		}
	}

	// If recovering
	if (agent.recovering() == true){
		if (agent.get_recovery_time() <= time){
			removed.at(0) = 1;
			add_agent_to_all_places(agent, households, hospitals);
			states_manager.set_any_to_removed(agent);
		}
	}
	return removed;
}

// Agent transitions related to testing time
void HspPatientTransitions::testing_transitions(Agent& agent, const double time,
										const std::map<std::string, double>& infection_parameters)
{
	// Determine the time agent gets results
	agent.set_time_until_results(infection_parameters.at("time from test to results"));
	agent.set_time_of_results(time);
	states_manager.set_tested_to_awaiting_results(agent);
}

// Agent transitions upon receiving test results 
int HspPatientTransitions::testing_results_transitions(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters)
{
	// If false negative, remove testing, put back to exposed
	// No false negative symptomatic
	double fneg_prob = infection_parameters.at("fraction false negative");
	int tested_pos = 0;
	if (infection.false_negative_test_result(fneg_prob) == true
			&& agent.exposed() == true){
		states_manager.set_tested_false_negative(agent);
	} else {
		// If confirmed positive
		tested_pos = 1;
		// Exposed - home isolation until symptomatic
		if (agent.exposed()){
			states_manager.set_home_isolation(agent);
			int hs_ID = infection.get_random_household_ID(households.size());
            agent.set_household_ID(hs_ID);
            // Register agent's ID
            int agent_ID = agent.get_ID();
            households.at(hs_ID-1).add_agent(agent_ID);
            // Remove agent from hospital
            hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
			agent.set_tested_covid_positive(true);
		} else {
			// Symptomatic - identify treatment
			if (agent.get_household_ID() == 0){
				int hs_ID = infection.get_random_household_ID(households.size());
            	agent.set_household_ID(hs_ID);
			}
			select_initial_treatment(agent, time, dt, infection, households, 
						hospitals, infection_parameters);
			agent.set_tested_covid_positive(true);
		}
	}
	return tested_pos;
}

// Determine type of intial treatement and its properties
void HspPatientTransitions::select_initial_treatment(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters)
{
	if (infection.agent_hospitalized(agent.get_age()) == true){
		// Remove agent from all places, then add to a random
		// hospital for treatment
		remove_agent_from_all_places(agent, households, hospitals);
		
		// But then add to a random hospital 
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
		if (agent.tested_covid_positive() == false){
			int hs_ID = infection.get_random_household_ID(households.size());
    	    agent.set_household_ID(hs_ID);
    	    // Register agent's ID
    	    int agent_ID = agent.get_ID();
    	    households.at(hs_ID-1).add_agent(agent_ID);
    	    // Remove agent from hospital
    	    hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
		}
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
			}
		}
	}
}

// Determine treatment changes 
void HspPatientTransitions::treatment_transitions(Agent& agent, const double time, 
			const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<Hospital>& hospitals,
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
				// Set up household ID
				int hs_ID = infection.get_random_household_ID(households.size());
		        agent.set_household_ID(hs_ID);
		        // Register agent's ID
		        int agent_ID = agent.get_ID();
		        households.at(hs_ID-1).add_agent(agent_ID);
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
				households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
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
				households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
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
void HspPatientTransitions::remove_agent_from_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<Hospital>& hospitals)
{
	int agent_ID = agent.get_ID();

	if (agent.home_isolated()){
		int hs_ID = agent.get_household_ID();
		if (hs_ID != 0){
			households.at(hs_ID-1).remove_agent(agent_ID);
		} else {
			throw std::runtime_error("Symptomatic home isolated agent does not have a valid household ID");
		}
	} else {
		hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
	}
}

// Add agent's ID back to the places where they are registered
// This is done to keep realistic numbers of people in different places
// which influences the probability
void HspPatientTransitions::add_agent_to_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<Hospital>& hospitals)
{
	int agent_ID = agent.get_ID();

	// ICU should always transition to hospital first
	if (agent.hospitalized_ICU())
		throw std::runtime_error("Attempting recovery of an agent directly from ICU");
	
	if (agent.home_isolated()){
		households.at(agent.get_household_ID()-1).remove_agent(agent_ID);
		hospitals.at(agent.get_hospital_ID()-1).add_agent(agent_ID);
	}
}


