#include "agent_tests.h"

/***************************************************** 
 *
 * Test suite for additional agent states 
 * in the Agent class
 *
 *****************************************************/

// Aliases for pointers to member functions used in these
// tests
using setter = void (Agent::*)(const bool);
using getter = bool (Agent::*)() const;

// Tests
bool test_states_on_off();

// Supporting functions
bool set_and_get(setter, getter, Agent);

int main()
{
	test_pass(test_states_on_off(), "Agent class states - getters and setters");
}

bool test_states_on_off()
{
	// Default agent object
	Agent agent;
	
	if (!set_and_get(&Agent::set_exposed, &Agent::exposed, agent))
		return false;
	if (!set_and_get(&Agent::set_recovering_exposed, &Agent::recovering_exposed, agent))
		return false;
	if (!set_and_get(&Agent::set_symptomatic, &Agent::symptomatic, agent))
		return false;
 	if (!set_and_get(&Agent::set_tested_covid_negative, &Agent::tested_covid_negative, agent))
		return false;
	if (!set_and_get(&Agent::set_tested_false_negative, &Agent::tested_false_negative, agent))
		return false;
	if (!set_and_get(&Agent::set_tested_false_positive, &Agent::tested_false_positive, agent))
		return false;
	if (!set_and_get(&Agent::set_tested, &Agent::tested, agent))
		return false;
	if (!set_and_get(&Agent::set_tested_in_car, &Agent::tested_in_car, agent))
		return false;
	if (!set_and_get(&Agent::set_tested_in_hospital, &Agent::tested_in_hospital, agent))
		return false;
	if (!set_and_get(&Agent::set_tested_awaiting_results, &Agent::tested_awaiting_results, agent))
		return false;
	if (!set_and_get(&Agent::set_tested_awaiting_test, &Agent::tested_awaiting_test, agent))
		return false;
	if (!set_and_get(&Agent::set_being_treated, &Agent::being_treated, agent))
		return false;
	if (!set_and_get(&Agent::set_home_isolated, &Agent::home_isolated, agent))
		return false;
	if (!set_and_get(&Agent::set_hospitalized, &Agent::hospitalized, agent))
		return false;
	if (!set_and_get(&Agent::set_hospitalized_ICU, &Agent::hospitalized_ICU, agent))
		return false;
	if (!set_and_get(&Agent::set_dying, &Agent::dying, agent))
		return false;
	if (!set_and_get(&Agent::set_recovering, &Agent::recovering, agent))
		return false;
	if (!set_and_get(&Agent::set_removed, &Agent::removed, agent))
		return false;
	return true;
}

// Set the value to true, than check if expected
bool set_and_get(setter set_val, getter get_val, Agent agent)
{
	(agent.*set_val)(true);
	if ((agent.*get_val)() != true)
		return false;
	return true;
}

