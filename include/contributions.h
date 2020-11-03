#ifndef CONTRIBUTIONS_H
#define CONTRIBUTIONS_H

//
// Places
//

#include "places/place.h"
#include "places/household.h"
#include "places/retirement_home.h"
#include "places/school.h"
#include "places/workplace.h"
#include "places/hospital.h"

//
// Other
//

#include "common.h"
#include "agent.h"
#include "flu.h"

/***************************************************** 
 * class: Contributions
 *
 * Functionality for computing of infection probability
 * contributions from agents to places and eventually 
 * mobility 
 * 
 ******************************************************/

class Contributions{
public:

	//
	// Constructors
	//

	/// \brief Default constructor only
	Contributions() = default;
	
	//
	// Contributions of agents to places 
	//
	
	/** 
	 * \brief Count contributions of an exposed agent
	 * @param agent - reference to Agent object
	 * @param time - current time
	 * @param households... - references to vectors of places 
	 */
	void compute_exposed_contributions(const Agent& agent, const double time,	
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes);

	/** 
	 * \brief Count contributions of a symptomatic agent
	 * @param agent - reference to Agent object
	 * @param time -  current time
	 * @param households... - references to vectors of places
	 */
	void compute_symptomatic_contributions(const Agent& agent, const double time,	
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes);

	/// \brief Compute the total contribution to infection probability at every place
	void total_place_contributions(std::vector<Household>& households, 
					std::vector<School>& schools, std::vector<Workplace>& workplaces, 
					std::vector<Hospital>& hospitals, std::vector<RetirementHome>& retirement_homes);

	/// \brief Set contributions/sums from all agents in places to 0.0 
	void reset_sums(std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes);

private:

	//
	// Specific contribution types
	//
	
	/// \brief Count contributions of a untreated and not tested symptomatic agent
	void compute_regular_symptomatic_contributions(const Agent& agent,  
				const double inf_var, std::vector<Household>& households, 
				std::vector<School>& schools, std::vector<Workplace>& workplaces,
				std::vector<RetirementHome>& retirement_homes);

	/// \brief Count contributions of a symptomatic-hospital tested agent
	void compute_hospital_tested_contributions(const Agent& agent, 
				const double inf_var, std::vector<Hospital>& hospitals);   

	/// \brief Count contributions of a home-isolated agent 
	void compute_home_isolated_contributions(const Agent& agent, 
				const double inf_vari, std::vector<Household>& households,
				std::vector<RetirementHome>& retirement_homes);
	
	/// \brief Count contributions of a hospitalized agent
	void compute_hospitalized_contributions(const Agent& agent, 
				const double inf_var, std::vector<Hospital>& hospitals);   

	/// \brief Count contributions of an agent hospitalized in ICU
	void compute_hospitalized_ICU_contributions(const Agent& agent, 
				const double inf_var, std::vector<Hospital>& hospitals);   

};
#endif













