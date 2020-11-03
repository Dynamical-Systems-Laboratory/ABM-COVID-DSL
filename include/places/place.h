#ifndef PLACE_H
#define PLACE_H

#include "../common.h"

/***************************************************** 
 * class: Place
 * 
 * Base class that defines a place  
 * 
 *****************************************************/

class Place{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates a Place object with default attributes
	 */
	Place() = default;

	/**
	 * \brief Creates a Place object 
	 * \details Place with custom ID, location, and infection parameters
	 * 			during creation of agents
	 * @param place_ID - ID of the place
	 * @param xi - x coordinate of the place
	 * @param yi - y coordinate of the place
	 * @param severity_cor - severity correction for symptomatic
	 * @param beta - infection transmission rate, 1/time
	 */
	Place(const int place_ID, const double xi, const double yi, const double severity_cor, const double beta) : 
		ID(place_ID), x(xi), y(yi), ck(severity_cor), beta_j(beta) { lambda_sum = 0.0; } 

	//
	// Infection related computations
	//
	
	/** 
	 *  \brief Include exposed contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	virtual void add_exposed(double inf_var) 
		{ lambda_sum += inf_var*beta_j; ++num_infected; }

	/** 
	 *  \brief Include symptomatic contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	virtual void add_symptomatic(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_j; ++num_infected; }

	/**
	 * \brief Calculates and stores fraction of infected agents if any  
	 */
	virtual void compute_infected_contribution();

	/**
	 *	\brief Reset the lambda sum of a place after transmission step
	 */
	virtual void reset_contributions() 
		{ lambda_sum = 0.0; lambda_tot = 0.0; num_infected = 0; }

	//
	// Setters
	//
	
	void change_transmission_rate(const double new_beta) { beta_j = new_beta; } 

	//
	// Getters
	//

	/// Return place ID
	int get_ID() const { return ID; }

	/// Return IDs of agents registered in this place	
	virtual std::vector<int> get_agent_IDs() const { return agent_IDs; }

	/// Return total number of infected agents
	int get_total_infected() const { return num_infected; }

	/// Return probability contribution of infected agents
	double get_infected_contribution() const { return lambda_tot; }

	/// Transmission rate
	double get_transmission_rate() const { return beta_j; }

	//
 	// I/O
	//

	/**
	 * \brief Save information about a Place object
	 * \details Saves to a file, everything but detailed agent 
	 * 		information; order is ID | x | y | number of agents | 
	 * 		number of infected agents
	 * 		Delimiter is a space.
	 * 	@param where - output stream
	 */
	virtual void print_basic(std::ostream& where) const;

	//
	// Initialization and update
	//

	/**
	 * \brief Assign agent to a given place indicating if infected
	 * @param agent_ID - global ID of the agent
	 * @param is_infected - true if agent is infected 
	 */
	void register_agent(const int agent_ID, const bool is_infected);

	/**
	 * \brief Add a new agent to this place
	 * @param index - agent ID (starts with 1)
	 */
	void add_agent(const int index) { agent_IDs.push_back(index); }

	/**
	 * \brief Remove an agent from this place
	 * @param index - agent ID (starts with 1)
	 */
	void remove_agent(const int index);

	// Virtual dtor - avoid UDB and memory leaks
	virtual ~Place() = default;

protected:
	// Place ID
	int ID = -1;
	// Location
	double x = 0.0, y = 0.0;
	// IDs of agents in this place
	std::vector<int> agent_IDs;
	// Total number of agents
	int num_tot = 0;
	// Total number of infected
	int num_infected = 0;

	// Sum of agents contributions
	double lambda_sum = 0.0;	
	// Total contribution to infection probability
	// from this place
	double lambda_tot = 0.0;

	// Severity correction for symptomatic
	double ck = 0.0;
	// Infection transmission rate, 1/time
	double beta_j = 0.0;

	// Ratio of infected agents
	double inf_ratio = 0.0;	
};

/// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Place& place);

#endif
