#ifndef HSP_EMPLOYEE_STATESMANAGER_H
#define HSP_EMPLOYEE_STATESMANAGER_H

//
// Other
//

#include "../common.h"
#include "../io_operations/abm_io.h"
#include "../agent.h"

/***************************************************** 
 * class: HspEmployeeStatesManager 
 *
 * Functionality for modifying agents states 
 * for agents that are hospital employees 
 * 
 ******************************************************/

class HspEmployeeStatesManager{
public:

	//
	// Constructors
	//

	HspEmployeeStatesManager() = default;

	/// Set all states for transition from susceptible to exposed
	void set_susceptible_to_exposed(Agent& agent);
	/// Set all states for transition from susceptible to exposed that will never become symptomatic
	void set_susceptible_to_exposed_never_symptomatic(Agent& agent);
	/// Set exposed that never developed symptoms to removed
	void set_exposed_never_symptomatic_to_removed(Agent& agent);

	/// Set all states for transition from exposed to general symptomatic
	void set_exposed_to_symptomatic(Agent& agent);
	/// Set all states relevant to agent that will die 
	void set_dying_symptomatic(Agent& agent);
	/// Set all states relevant to agent that will recover 
	void set_recovering_symptomatic(Agent& agent);
	
	/// Set testing in hospital, initial state 
	void set_waiting_for_test_in_hospital(Agent& agent);
	/// Set testing in hospital for exposed, initial state 
	void set_exposed_waiting_for_test_in_hospital(Agent& agent);
	/// Set all states for just tested 
	void set_tested_to_awaiting_results(Agent& agent);
	/// Set all states for transition from tested to false negative 
	void set_tested_false_negative(Agent& agent);

	/// States for hospitalized, ICU - dying
	void set_icu_dying(Agent& agent);
	/// States for hospitalized, ICU - recovering 
	void set_icu_recovering(Agent& agent);
	/// States for hospitalized 
	void set_hospitalized(Agent& agent);
	/// States for isolated at home
	void set_home_isolation(Agent& agent);
	
	/// Set all removed related states
	void set_any_to_removed(Agent& agent);

	/// States for negative 
	void set_tested_negative(Agent& agent);
	/// Reset flags for flu that is back to susceptible from IH
	void reset_returning_flu(Agent& agent);

private:

	/// Set all testing states to a single value 
	void set_all_testing_states(Agent& agent, const bool val);

	/// Set all treatment states to a single value 
	void set_all_treatment_states(Agent& agent, const bool val);

};

#endif
