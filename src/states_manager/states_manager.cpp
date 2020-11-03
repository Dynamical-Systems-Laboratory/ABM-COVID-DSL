#include "../../include/states_manager/states_manager.h"

/***************************************************** 
 * class: StatesManager 
 *
 * Functionality for modifying agents states  
 * 
 ******************************************************/

// Set all states for transition from susceptible to exposed
void StatesManager::set_susceptible_to_exposed(Agent& agent)
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
void StatesManager::set_susceptible_to_exposed_never_symptomatic(Agent& agent)
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
void StatesManager::set_exposed_never_symptomatic_to_removed(Agent& agent)
{
	set_any_to_removed(agent);
}

// Set all states for transition from exposed to general symptomatic
void StatesManager::set_exposed_to_symptomatic(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(false);
	agent.set_recovering_exposed(false);	

	agent.set_symptomatic(true);

	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);
	
	agent.set_dying(false);
	agent.set_recovering(false);	
	agent.set_removed(false);
}

// Set testing in hospital, initial state 
void StatesManager::set_waiting_for_test_in_hospital(Agent& agent)
{
	agent.set_tested(true);

	agent.set_tested_in_car(false); 
	agent.set_tested_in_hospital(true); 
	agent.set_tested_awaiting_results(false);
	agent.set_tested_awaiting_test(true);

	// All waiting agents undergo home isolation
	// but not the official treatment yet
	agent.set_home_isolated(true);
}

// Set testing in a car, initial state 
void StatesManager::set_waiting_for_test_in_car(Agent& agent)
{
	agent.set_tested(true);

	agent.set_tested_in_car(true); 
	agent.set_tested_in_hospital(false); 
	agent.set_tested_awaiting_results(false);
	agent.set_tested_awaiting_test(true);

	// All waiting agents undergo home isolation
	// but not the official treatment yet
	agent.set_home_isolated(true);
}

// Set all states relevant to agent that will die 
void StatesManager::set_dying_symptomatic(Agent& agent)
{
	agent.set_dying(true);
	agent.set_recovering(false);	
}

// Set all states relevant to agent that will recover 
void StatesManager::set_recovering_symptomatic(Agent& agent)
{
	agent.set_dying(false);
	agent.set_recovering(true);	
}

// Set all testing states to a single value 
void StatesManager::set_all_testing_states(Agent& agent, const bool val)
{
	agent.set_tested(val);

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
void StatesManager::set_all_treatment_states(Agent& agent, const bool val)
{
	// Treatment types
	agent.set_being_treated(val);
	agent.set_home_isolated(val);
	agent.set_hospitalized(val);
	agent.set_hospitalized_ICU(val);
}

// Set all removed related states
void StatesManager::set_any_to_removed(Agent& agent)
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
void StatesManager::set_tested_to_awaiting_results(Agent& agent)
{
	agent.set_tested_awaiting_results(true);
	agent.set_tested_in_car(false);
	agent.set_tested_in_hospital(false);
	agent.set_tested_awaiting_test(false);
}

// Set all states for transition from tested to false negative 
void StatesManager::set_tested_false_negative(Agent& agent)
{
	set_all_treatment_states(agent, false);
	// First set all false, just change this one
	set_all_testing_states(agent, false);
	agent.set_tested_false_negative(true);
}

// States for hospitalized, ICU - dying
void StatesManager::set_icu_dying(Agent& agent)
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
void StatesManager::set_icu_recovering(Agent& agent)
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
void StatesManager::set_hospitalized(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);

	agent.set_being_treated(true);
	agent.set_home_isolated(false);
	agent.set_hospitalized_ICU(false);
	agent.set_hospitalized(true);
}

// States for isolated at home
void StatesManager::set_home_isolation(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);

	agent.set_being_treated(true);
	agent.set_hospitalized_ICU(false);
	agent.set_hospitalized(false);
	agent.set_home_isolated(true);
}

// Reset all non-covid symptomatic flags
void StatesManager::set_former_flu(Agent& agent)
{
	agent.set_symptomatic_non_covid(false);

	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);
}

// States for false positive, isolated at home
void StatesManager::set_tested_false_positive(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);

	agent.set_tested_false_positive(true);
	agent.set_home_isolated(true);
}

// States for negative
void StatesManager::set_tested_negative(Agent& agent)
{
	// Set all testing and treatment related flags to false
	set_all_testing_states(agent, false);
	set_all_treatment_states(agent, false);

	agent.set_tested_covid_negative(true);
}

// Reset flags for flu that is back to susceptible from IH
void StatesManager::reset_returning_flu(Agent& agent)
{
	set_all_treatment_states(agent, false);
	agent.set_tested_false_positive(false);
	agent.set_symptomatic_non_covid(false);
}
