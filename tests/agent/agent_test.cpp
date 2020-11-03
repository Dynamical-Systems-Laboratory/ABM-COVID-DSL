#include "agent_tests.h"

/***************************************************** 
 *
 * Test suite for functionality of the Agent class
 *
 *****************************************************/

// Tests
bool agent_constructor_getters_test();
bool agent_events_test();
bool agent_out_test();

int main()
{
	test_pass(agent_constructor_getters_test(), "Agent class constructor and getters");
	test_pass(agent_events_test(), "Agent class event scheduling and handling functionality");
	test_pass(agent_out_test(), "Agent class ostream operator");
}

/// Tests Agent class constructor and most of existing getters 
// This checks only one agent type, more robust type checking 
// i.e. health state, works/studies combinations are in the
// ABM test suite  
bool agent_constructor_getters_test()
{
	bool student = true, works = false, infected = true;
	bool is_hospital_patient = false, works_at_hospital = true;
	bool res_rh = false, works_rh = false, works_school = true;
	int age = 25, hID = 3, sID = 305, wID = 0, hspID = 1;
	double xi = 7.009, yi = 100.5;
	int aID = 1;
	double inf_var = 0.2009;
	double cur_time = 4.0;

	Agent agent(student, works, age, xi, yi, hID, is_hospital_patient, sID, res_rh, works_rh, works_school, wID, works_at_hospital, hspID, infected);
	agent.set_ID(aID);
	agent.set_inf_variability_factor(inf_var);
	
	// IDs
	if (aID != agent.get_ID() || hID != agent.get_household_ID() 
			|| sID != agent.get_school_ID() || wID != agent.get_work_ID() 
			|| hspID != agent.get_hospital_ID())
		return false;

	// Age
	if (age != agent.get_age())
		return false;

	// Location
	if (!float_equality<double>(xi, agent.get_x_location(), 1e-5)
			|| !float_equality<double>(yi, agent.get_y_location(), 1e-5))
		return false;

	// State
	if (student != agent.student() || works != agent.works() 
			|| infected != agent.infected())
		return false;	

	// Hospital patient or healthcare professional status
	if (is_hospital_patient != agent.hospital_non_covid_patient() || 
			works_at_hospital != agent.hospital_employee())
		return false;	
		
	// Hospital patient or healthcare professional status
	if (res_rh != agent.retirement_home_resident() || 
			works_rh != agent.retirement_home_employee() ||
			works_school != agent.school_employee())
		return false;
	
	// Infectiousness variability
	if (!float_equality<double>(inf_var, agent.get_inf_variability_factor(), 1e-5))
		return false;

	return true;
}

/// Check the correctness of the time-depending event handling
bool agent_events_test()
{
	bool student = true, works = false, infected = true;
	bool is_hospital_patient = false, works_at_hospital = true;
	bool res_rh = false, works_rh = false, works_school = true;
	int age = 25, hID = 3, sID = 305, wID = 0, hspID = 1;
	double xi = 7.009, yi = 100.5;
	int aID = 1;
	double inf_var = 0.2009;

	double cur_time = 4.0;
	double latency = 3.5, lat_end_time = cur_time + latency;
	double dt_inert = 5.0, inert = cur_time + dt_inert; 
	double otd = 1.2, time_of_death = cur_time + otd;
	double recovery = 10.0, time_of_recovery = cur_time + recovery; 
	double test_time = 0.5, time_of_test = cur_time + test_time;
	double test_res_time = 3.5, time_of_test_res = cur_time + test_res_time;
	double t_hsp_icu = cur_time + 2, t_hsp_ih = cur_time + 0.3, t_icu_hsp = cur_time + 11;
	double t_ih_icu = cur_time + 1, t_ih_hsp = cur_time + 3.1;  

	Agent agent(student, works, age, xi, yi, hID, is_hospital_patient, sID, res_rh, works_rh, works_school, wID, works_at_hospital, hspID, infected);
	agent.set_ID(aID);
	agent.set_inf_variability_factor(inf_var);
	
	agent.set_latency_duration(latency);
	agent.set_latency_end_time(cur_time);
	agent.set_infectiousness_start_time(cur_time, dt_inert);

	agent.set_time_to_death(otd);
	agent.set_death_time(cur_time);

	agent.set_recovery_duration(recovery);
	agent.set_recovery_time(cur_time);

  	agent.set_time_to_test(test_time);
  	agent.set_time_of_test(cur_time);

    agent.set_time_until_results(test_res_time); 
  	agent.set_time_of_results(cur_time);
   	
   	agent.set_time_hsp_to_icu(t_hsp_icu); 
 	agent.set_time_hsp_to_ih(t_hsp_ih); 
  	agent.set_time_icu_to_hsp(t_icu_hsp);
  	agent.set_time_ih_to_icu(t_ih_icu); 
	agent.set_time_ih_to_hsp(t_ih_hsp); 

	// Latency
	if (!float_equality<double>(lat_end_time, agent.get_latency_end_time(), 1e-5))
		return false;

	// Infectiousness 
	if (!float_equality<double>(inert, agent.get_infectiousness_start_time(), 1e-5))
		return false;

	// Death
	if (!float_equality<double>(time_of_death, agent.get_time_of_death(), 1e-5))
		return false;
	
	// Recovery
	if (!float_equality<double>(time_of_recovery, agent.get_recovery_time(), 1e-5))
		return false;
		
	// Testing
	if (!float_equality<double>(time_of_test, agent.get_time_of_test(), 1e-5))
		return false;
	if (!float_equality<double>(time_of_test_res, agent.get_time_of_results(), 1e-5))
		return false;
	
	// Treatment
	if (!float_equality<double>(t_hsp_icu, agent.get_time_hsp_to_icu(), 1e-5))
		return false;
	if (!float_equality<double>(t_hsp_ih, agent.get_time_hsp_to_ih(), 1e-5))
		return false;
	if (!float_equality<double>(t_icu_hsp, agent.get_time_icu_to_hsp(), 1e-5))
		return false;
	if (!float_equality<double>(t_ih_icu, agent.get_time_ih_to_icu(), 1e-5))
		return false;
	if (!float_equality<double>(t_ih_hsp, agent.get_time_ih_to_hsp(), 1e-5))
		return false;

	return true;
}


/// Tests Agent ostream operator overload/print capabilities
bool agent_out_test()
{
	bool student = true, works = false, infected = true;
	bool is_hospital_patient = false, works_at_hospital = true;
	bool res_rh = true, works_rh = false, works_school = true;
	int age = 25, hID = 3, sID = 305, wID = 0, hspID = 1;
	double xi = 7.009, yi = 100.5;
	int aID = 1;
	double inf_var = 0.2009;

	Agent agent(student, works, age, xi, yi, hID, is_hospital_patient, sID, res_rh, works_rh, works_school, wID, works_at_hospital, hspID, infected);
	agent.set_ID(aID);

	// Get directly from the stream and compare
	std::stringstream agent_buff;
	agent_buff << agent;
	std::istringstream res(agent_buff.str());

	bool test_student = false, test_works = false, test_infected = false; 
	bool test_works_hospital = false, test_patient = false;
	bool test_rh_res = false, test_works_rh = false, test_works_school = false;
	int test_age = 0, test_hID = 0, test_sID = 0, test_wID = 0, test_hspID = 0;
	double test_xi = 0.0, test_yi = 0.0;
	int test_aID = 0;

	res >> test_aID >> test_student >> test_works >> test_age
		>> test_xi >> test_yi >> test_hID >> test_patient >> test_sID >> test_wID
		>> test_works_hospital >> test_hspID >> works_rh >> works_school >> test_rh_res >> test_infected;

	// IDs
	if (aID != test_aID || hID != test_hID || sID != test_sID || wID != test_wID || hspID != test_hspID)
		return false;
	
	// Location
	if (!float_equality<double>(xi, test_xi, 1e-5)
			|| !float_equality<double>(yi, test_yi, 1e-5))
		return false;

	// State
	if (student != test_student || works != test_works || infected != test_infected 
					|| works_at_hospital != test_works_hospital || is_hospital_patient != test_patient)
		return false;	

	return true;
}


