#ifndef FLU_H
#define FLU_H

#include "common.h"
#include "testing.h"
#include "rng.h"

/***************************************************** 
 * class: Flu 
 * 
 * Manages the susceptible agents that have symptoms
 * from an infection other than COVID  
 * 
 *****************************************************/

class Flu{
public:

	//
	// Construnctors
	//
	
	Flu() = default;	

	/// \brief Fraction of susceptible population with flu
	void set_fraction(const double flu_frac) { nc_sy_frac = flu_frac; }

	/// \brief Set portion to be tested false positive
	void set_fraction_tested_false_positive(const double flu_fp_tested) 
		{ frac_tested_fp = flu_fp_tested; }

	/// \brief Specifies the offset in days for the time Flu agents can be tested
	void set_testing_duration(const double dt) { testing_period = dt; }

	//
	//	Flu computations and agent management 
	//

	/**
	 * \brief Initial generation of flu population
	 * @return Vector of IDs of agents that have flu
	 */
	std::vector<int> generate_flu();	

	/**
	 * \brief Add a susceptible agent 
	 * @param index - agent ID (starts with 1)
	 */
	void add_susceptible_agent(const int index) 
		{ susceptible_agent_IDs.push_back(index); }

	/**
	 * \brief Remove a susceptible agent 
	 * @param index - agent ID (starts with 1)
	 */
	void remove_susceptible_agent(const int index);

	/**
	 * \brief Remove a flu agent 
	 * @param index - agent ID (starts with 1)
	 */
	void remove_flu_agent(const int index);

	/** 
	 * \brief Remove recovered from flu, add new chosen randomly
	 * @param index - agent to be removed from flu
	 * @return ID of the agent that now has flu
	 */
	int swap_flu_agent(const int index);

	/// \brief True if agent will get tested
	bool getting_tested(const Testing& testing)
		{ return rng.get_random(0,1) <= testing.get_prob_flu_tested(); }

	/// \brief True if agent will get tested false positive
	bool tested_false_positive()
		{ return rng.get_random(0,1) <= frac_tested_fp; }

	//
	// Getters
	//

	/// \brief Const reference to susceptible IDs vector
	const std::vector<int>& get_susceptible_IDs() const { return susceptible_agent_IDs; }
	/// \brief Const reference to IDs of agents with flu
	const std::vector<int>& get_flu_IDs() const { return flu_agent_IDs; }

private:
	// Fraction of the total susceptible population
	// that has an infection other than COVID with 
	// similar symptoms
	double nc_sy_frac = 0.0;
	// Fraction tested false positive
	double frac_tested_fp = 0.0;
	// Period during which an agent will be tested
	double testing_period = 0.0;

	// Random number generator class
	RNG rng;

	// Susceptible agents
	std::vector<int> susceptible_agent_IDs;
	// Susceptible with flu
	std::vector<int> flu_agent_IDs;
};

#endif
