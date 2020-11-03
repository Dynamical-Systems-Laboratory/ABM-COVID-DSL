#ifndef FLU_TRANSITIONS_H
#define FLU_TRANSITIONS_H

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
// Other
//

#include "../common.h"
#include "../agent.h"
#include "../infection.h"
#include "../states_manager/regular_states_manager.h"
#include "../flu.h"
#include "../testing.h"

/***************************************************** 
 * class: FluTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for an agent that
 * has a condition other than COVID and is symptomatic  
 * 
 ******************************************************/

class FluTransitions{
public:

	//
	// Constructors
	//
	
	/// Creates a RegularTransitions object with default attributes
	FluTransitions() = default;

	/// \brief Implement transitions relevant to susceptible
	/// \details Returns 1 if the agent got infected 
	std::vector<int> susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			    std::vector<RetirementHome>& retirement_homes,	
				const std::map<std::string, double>& infection_parameters, 
				std::vector<Agent>& agents, Flu& flu, const Testing& testing, const double dt);

	/// \brief Determine any testing related properties
	void set_testing_status(Agent& agent, Infection& infection, const double time, 
				std::vector<School>& schools, std::vector<Workplace>& workplaces, 
				std::vector<Hospital>& hospitals, std::vector<RetirementHome>& retirement_homes,
				const std::map<std::string, double>& infection_parameters,
				const Testing& testing);

	/// \brief Agent transitions related to testing time
	void testing_transitions_flu(Agent& agent, const double time,
									const std::map<std::string, double>& infection_parameters);

	/// \brief Agent transitions upon receiving test results
	void testing_results_transitions_flu(Agent& agent, std::vector<Agent>& agents, Flu& flu,
			const double time, Infection& infection,
			std::vector<Household>& households, std::vector<School>& schools,
			std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
			std::vector<RetirementHome>& retirement_homes,
			const std::map<std::string, double>& infection_parameters, const Testing& testing);

	/// \brief Set properties related to newly created agent with flu, including testing
	void process_new_flu(Agent& agent, const int n_hospitals, const double time,  
					std::vector<School>& schools, std::vector<Workplace>& workplaces,
					std::vector<RetirementHome>& retirement_homes,
					Infection& infection, const std::map<std::string, double>& infection_parameters, 
					Flu& flu, const Testing& testing);
private:

	// For changing agent states
	RegularStatesManager states_manager;

	/// \brief Return total lambda of susceptible agent
	double compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Household>& households, const std::vector<School>& schools,
					const std::vector<Workplace>& workplaces, const std::vector<Hospital>& hospitals,
					const std::vector<RetirementHome>& retirement_homes);

	/// \brief Compte and set agent properties related to recovery without symptoms and incubation
	void recovery_and_incubation(Agent& agent, Infection& infection, const double time,
				                const std::map<std::string, double>& infection_parameters);

	/// \brief Remove agent's index from all workplaces and schools that have them registered
	void remove_from_all_workplaces_and_schools(Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces,
							std::vector<RetirementHome>& retirement_homes);

	/// \brief Add agent's index from all workplaces and schools that have them registered
	void add_to_all_workplaces_and_schools(Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces,
							std::vector<RetirementHome>& retirement_homes);
};

#endif
