#include "../../include/states_manager/hsp_employee_states_manager.h"

/***************************************************** 
 * class: HspEmployeeStatesManager 
 *
 * Functionality for modifying agents states
 * for agents that are hospital employees  
 * 
 ******************************************************/

// Set all states for transition from susceptible to exposed
void HspEmployeeStatesManager::set_susceptible_to_exposed(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(true);
	agent.set_recovering_exposed(false);	
	agent.set_symptomatic(false);
	
	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);
	
	agent.set_dying(false);
	agent.set_recovering(false);	
	agent.set_removed(false);
}

// Set all states for transition from susceptible to exposed that will never become symptomatic
void HspEmployeeStatesManager::set_susceptible_to_exposed_never_symptomatic(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(true);
	agent.set_recovering_exposed(true);	
	agent.set_symptomatic(false);
	
	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);
	
	agent.set_dying(false);
	agent.set_recovering(true);	
	agent.set_removed(false);
}

// Set exposed that never developed symptoms to removed
void HspEmployeeStatesManager::set_exposed_never_symptomatic_to_removed(Agent& agent)
{
	set_any_to_removed(agent);
}

// Set all states for transition from exposed to general symptomatic
void HspEmployeeStatesManager::set_exposed_to_symptomatic(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(false);
	agent.set_recovering_exposed(false);	
	agent.set_symptomatic(true);
	agent.set_tested_false_negative(false);
	agent.set_dying(false);
	agent.set_recovering(false);	
	agent.set_removed(false);
}

// Set testing in hospital, initial state 
void HspEmployeeStatesManager::set_waiting_for_test_in_hospital(Agent& agent)
{
	agent.set_tested(true);

	agent.set_tested_in_car(false); 
	agent.set_tested_in_hospital(true); 
	agent.set_tested_awaiting_results(false);
	agent.set_tested_awaiting_test(true);

	// Home isolation for symptomatic
	agent.set_home_isolated(true);
}

// Set testing in hospital, initial state 
void HspEmployeeStatesManager::set_exposed_waiting_for_test_in_hospital(Agent& agent)
{
	agent.set_tested(true);
	agent.set_tested_exposed(true);

	agent.set_tested_in_car(false); 
	agent.set_tested_in_hospital(true); 
	agent.set_tested_awaiting_results(false);
	agent.set_tested_awaiting_test(true);

	// No home isolation for an exposed hospital employee 
	agent.set_home_isolated(false);
}

// Set all states relevant to agent that will die 
void HspEmployeeStatesManager::set_dying_symptomatic(Agent& agent)
{
	agent.set_dying(true);
	agent.set_recovering(false);	
}

// Set all states relevant to agent that will recover 
void HspEmployeeStatesManager::set_recovering_symptomatic(Agent& agent)
{
	agent.set_dying(false);
	agent.set_recovering(true);	
}

// Set all testing states to a single value 
void HspEmployeeStatesManager::set_all_testing_states(Agent& agent, const bool val)
{
	agent.set_tested(val);
	agent.set_tested_exposed(val);

	// Testing results
	agent.set_tested_covid_negative(val);
	agent.set_tested_false_negative(val);
	agent.set_tested_false_positive(val);

	// Testing phases and types
	agent.set_tested_in_car(val); 
	agent.set_tested_in_hospital(val); 
	agent.set_tested_awaiting_results(val);
	agent.set_tested_awaiting_test(val);
}

// Set all treatment states to a single value 
void HspEmployeeStatesManager::set_all_treatment_states(Agent& agent, const bool val)
{
	// Treatment types
	agent.set_being_treated(val);
	agent.set_home_isolated(val);
	agent.set_hospitalized(val);
	agent.set_hospitalized_ICU(val);
}

// Set all removed related states
void HspEmployeeStatesManager::set_any_to_removed(Agent& agent)
{
	agent.set_removed(true);

	agent.set_dying(false);
	agent.set_recovering(false);	

	agent.set_infected(false);
	agent.set_exposed(false);
	agent.set_recovering_exposed(false);	
	agent.set_symptomatic(false);
	
	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);
}

// Set all states for just tested 
void HspEmployeeStatesManager::set_tested_to_awaiting_results(Agent& agent)
{
	agent.set_tested_awaiting_results(true);
	agent.set_tested_in_car(false);
	agent.set_tested_in_hospital(false);
	agent.set_tested_awaiting_test(false);
}

// Set all states for transition from tested to false negative 
void HspEmployeeStatesManager::set_tested_false_negative(Agent& agent)
{
	set_all_treatment_states(agent, false);
	// First set all false, just change this one
	set_all_testing_states(agent, false);
	agent.set_tested_false_negative(true);
}

// States for hospitalized, ICU - dying
void HspEmployeeStatesManager::set_icu_dying(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);

	agent.set_being_treated(true);
	agent.set_home_isolated(false);
	agent.set_hospitalized(false);
	agent.set_hospitalized_ICU(true);

	agent.set_dying(true);
	agent.set_recovering(false);
}

// States for hospitalized, ICU - recovering 
void HspEmployeeStatesManager::set_icu_recovering(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);

	agent.set_being_treated(true);
	agent.set_home_isolated(false);
	agent.set_hospitalized(false);
	agent.set_hospitalized_ICU(true);

	agent.set_dying(false);
	agent.set_recovering(true);
}

// States for hospitalized 
void HspEmployeeStatesManager::set_hospitalized(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);

	agent.set_being_treated(true);
	agent.set_home_isolated(false);
	agent.set_hospitalized_ICU(false);
	agent.set_hospitalized(true);
}

// States for isolated at home
void HspEmployeeStatesManager::set_home_isolation(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);

	agent.set_being_treated(true);
	agent.set_hospitalized_ICU(false);
	agent.set_hospitalized(false);
	agent.set_home_isolated(true);
}

// States for negative
void HspEmployeeStatesManager::set_tested_negative(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);
	agent.set_tested_covid_positive(false);
	agent.set_tested_covid_negative(true);
}

