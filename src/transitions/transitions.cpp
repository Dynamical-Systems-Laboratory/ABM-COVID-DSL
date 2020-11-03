#include "../../include/transitions/transitions.h"

/***************************************************** 
 * class: Transitions 
 *
 * Interface for computing of transitioning 
 * between different agents states 
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
std::vector<int> Transitions::susceptible_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				std::vector<RetirementHome>& retirement_homes, 
				const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, Flu& flu, const Testing& testing)
{
	// Ingected, tested, negative, false positive 
	std::vector<int> state_changes(4,0);
	int got_infected = 0;
	if (agent.symptomatic_non_covid()){
		// Currently only symptomatic non-COVID can reach all 4 states
		state_changes = flu_tr.susceptible_transitions(agent, time, infection,
				households, schools, workplaces, hospitals, retirement_homes, 
				infection_parameters, agents, flu, testing, dt);
	} else if (agent.hospital_employee()){
		got_infected = hsp_emp_tr.susceptible_transitions(agent, time, infection,
				households, schools, hospitals,	infection_parameters, agents, testing);
		state_changes.at(0) = got_infected;
	} else if (agent.hospital_non_covid_patient()){
		got_infected = hsp_pt_tr.susceptible_transitions(agent, time, infection,
				hospitals, infection_parameters, agents, testing);
		state_changes.at(0) = got_infected;
	} else {
		got_infected = regular_tr.susceptible_transitions(agent, time, infection,
				households, schools, workplaces, hospitals, retirement_homes,
				infection_parameters, agents, flu, testing);
		state_changes.at(0) = got_infected;
	}
	return state_changes;	
}

// Implement transitions relevant to exposed 
std::vector<int> Transitions::exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<School>& schools,
										std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
										std::vector<RetirementHome>& retirement_homes,
										const std::map<std::string, double>& infection_parameters, const Testing& testing)
{
	// Recovered, dead, tested, tested positive, tested false negative
	std::vector<int> state_changes(5,0);
	if (agent.hospital_employee()){
		state_changes = hsp_emp_tr.exposed_transitions(agent, infection, time, dt, 
					households, schools, hospitals,	infection_parameters, testing);
	} else if (agent.hospital_non_covid_patient()){
		state_changes = hsp_pt_tr.exposed_transitions(agent, infection, time, dt, 
					households, hospitals, infection_parameters, testing);
	} else {
		state_changes = regular_tr.exposed_transitions(agent, infection, time, dt, 
					households, schools, workplaces, hospitals, retirement_homes,
					infection_parameters, testing);
	}
	return state_changes;
}

// Transitions of a symptomatic agent 
std::vector<int> Transitions::symptomatic_transitions(Agent& agent, const double time, 
				   	const double dt, Infection& infection,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes,
					const std::map<std::string, double>& infection_parameters)
{
	// Recovered, dead, tested, tested positive, false negative
	std::vector<int> state_changes(5,0);
	if (agent.hospital_employee()){
		state_changes = hsp_emp_tr.symptomatic_transitions(agent, time, dt, infection,  
					households, schools, hospitals, infection_parameters);
	} else if (agent.hospital_non_covid_patient()){
		state_changes = hsp_pt_tr.symptomatic_transitions(agent, time, dt, 
					infection, households, hospitals, infection_parameters);
	} else {
		state_changes = regular_tr.symptomatic_transitions(agent, time, dt, infection, 
					households, schools, workplaces, hospitals, retirement_homes, 
					infection_parameters);
	}
	return state_changes; 
}


