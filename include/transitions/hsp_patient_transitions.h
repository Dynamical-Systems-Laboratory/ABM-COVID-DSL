#ifndef HSP_PATIENT_TRANSITIONS_H
#define HSP_PATIENT_TRANSITIONS_H

//
// Places
//

#include "../places/place.h"
#include "../places/household.h"
#include "../places/school.h"
#include "../places/workplace.h"
#include "../places/hospital.h"

//
// Other
//

#include "../common.h"
#include "../agent.h"
#include "../infection.h"
#include "../states_manager/hsp_employee_states_manager.h"
#include "../flu.h"

/***************************************************** 
 * class: HspPatientTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for agent that is 
 * a hospital patient and does not initially have
 * COVID 
 * 
 ******************************************************/

class HspPatientTransitions{
public:

	//
	// Constructors
	//
	
	/// Creates a HspEmployeeTransitions object with default attributes
	HspPatientTransitions() = default;

	/// \brief Implement transitions relevant to susceptible
	/// \details Returns 1 if the agent got infected 
	int susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Hospital>& hospitals, const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, const Testing& testing);

	/// \brief Implement transitions relevant to exposed
	/// \details Return 1 if recovered without symptoms 
	std::vector<int> exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, std::vector<Household>& households,
				std::vector<Hospital>& hospitals, const std::map<std::string, double>& infection_parameters, const Testing& testing);

	/// \brief Determine any testing related properties
	void set_testing_status(Agent& agent, Infection& infection, const double time, 
				std::vector<Hospital>& hospitals, 
				const std::map<std::string, double>& infection_parameters, const Testing& testing);

	/// \brief Transitions of a symptomatic agent 
	/// @return Vector where first entry is one if agent recovered, second if agent died
	std::vector<int> symptomatic_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection, std::vector<Household>& households,
				std::vector<Hospital>& hospitals, const std::map<std::string, double>& infection_parameters);

	/// \brief Agent transitions related to testing time
	void testing_transitions(Agent& agent, const double time,
										const std::map<std::string, double>& infection_parameters);

	/// \brief Agent transitions upon receiving test results
	int testing_results_transitions(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households,	std::vector<Hospital>& hospitals, 
			const std::map<std::string, double>& infection_parameters);

	/// \brief Determine treatment changes 
	void treatment_transitions(Agent& agent, const double time, 
			const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters);

private:

	// For changing agent states
	HspEmployeeStatesManager states_manager;

	/// \brief Return total lambda of susceptible agent
	double compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Hospital>& hospitals);

	/// \brief Compte and set agent properties related to recovery without symptoms and incubation
	void recovery_and_incubation(Agent& agent, Infection& infection, const double time,
				                const std::map<std::string, double>& infection_parameters);
	
	/// Determine type of intial treatement and its properties
	void select_initial_treatment(Agent& agent, 
			const double time, const double dt, Infection& infection,
			std::vector<Household>& households, std::vector<Hospital>& hospitals,
			const std::map<std::string, double>& infection_parameters);

	/// \brief Verifies and manages removal of an agent from the model
	/// @return Vector where first entry is one if agent recovered, second if agent died
	std::vector<int> check_agent_removal(Agent& agent, const double time,
					std::vector<Household>& households, std::vector<Hospital>& hospitals);

	/// \brief Remove agent's ID from places where they are registered
	void remove_agent_from_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<Hospital>& hospitals);

	/// \brief Add agent's ID back to the places where they are registered
	/// \details This is done to keep realistic numbers of people in different places
	/// 	which influences the probability
	void add_agent_to_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<Hospital>& hospitals);

};

#endif
