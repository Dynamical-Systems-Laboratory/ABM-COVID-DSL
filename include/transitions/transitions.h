#ifndef TRANSITIONS_H
#define TRANSITIONS_H

//
// Places
//

#include "../places/place.h"
#include "../places/household.h"
#include "../places/school.h"
#include "../places/workplace.h"
#include "../places/hospital.h"
#include "../places/retirement_home.h"

//
// Transitions
//

#include "regular_transitions.h"
#include "hsp_employee_transitions.h"
#include "hsp_patient_transitions.h"
#include "flu_transitions.h"

//
// Other
//

#include "../common.h"
#include "../agent.h"
#include "../infection.h"
#include "../flu.h"
#include "../testing.h"

/***************************************************** 
 * class: Transitions 
 *
 * Interface for computing of transitioning 
 * between different agents states 
 * 
 ******************************************************/

class Transitions{
public:

	//
	// Constructors
	//
	
	/// Creates a Transitions object with default attributes
	Transitions() = default;

	//
	// Transitioning functionality
	//

	/// \brief Implement transitions relevant to susceptible
	/// \details Returns 1 if the agent got infected 
	std::vector<int> susceptible_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				std::vector<RetirementHome>& retirement_homes,
				const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, Flu& flu, const Testing& testing);

	/// \brief Implement transitions relevant to exposed
	/// \details Return 1 if recovered without symptoms 
	std::vector<int> exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				std::vector<RetirementHome>& retirement_homes,
				const std::map<std::string, double>& infection_parameters, const Testing& testing);

	/// \brief Transitions of a symptomatic agent 
	/// @return Vector where first entry is one if agent recovered, second if agent died
	std::vector<int> symptomatic_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection,
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				std::vector<RetirementHome>& retirement_homes,
				const std::map<std::string, double>& infection_parameters);
	
	/// \brief Set properties related to newly created agent with flu, including testing
	void process_new_flu(Agent& agent, const int n_hospitals, const double time,  
					std::vector<School>& schools, std::vector<Workplace>& workplaces, 
					std::vector<RetirementHome>& retirement_homes, Infection& infection, 
					const std::map<std::string, double>& infection_parameters, Flu& flu, const Testing& testing) 
	{ flu_tr.process_new_flu(agent, n_hospitals, time, schools, workplaces, retirement_homes, infection, infection_parameters, flu, testing); }

private:
	// Transition classes
	RegularTransitions regular_tr;
	HspEmployeeTransitions hsp_emp_tr;
	HspPatientTransitions hsp_pt_tr;
	FluTransitions flu_tr;
};

#endif
