#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H

#include "place.h"
#include "../infection.h"

class Place;
class Infection;

/***************************************************** 
 * class: Household
 * 
 * Defines and stores attributes of a single household
 * 
 *****************************************************/

class Household : public Place{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates a Household object with default attributes
	 */
	Household() = default;

	/**
  	 * \brief Creates a Household object
	 * \details Household with custom ID, location, and infection parameters
	 *
  	 * @param house_ID - ID of the household
  	 * @param xi - x coordinate of the household
  	 * @param yi - y coordinate of the household
  	 * @param alpha_exp - household size correction
  	 * @param severity_cor - severity correction for symptomatic
  	 * @param beta - infection transmission rate, 1/time
	 * @param betaih - infection transmission rate for home isolated, 1/time
  	 */
	Household(int house_ID, double xi, double yi, const double alpha_exp, const double severity_cor, const double beta, const double betaih) : 
		alpha(alpha_exp), beta_ih(betaih), Place(house_ID, xi, yi, severity_cor, beta) { } 

	//
 	// I/O
	//

	/**
	 * \brief Save information about a Household object
	 * \details Saves to a file, everything but detailed agent 
	 * 		information; order is ID | x | y | number of agents | 
	 * 		number of infected agents | ck | beta_j | alpha
	 * 		Delimiter is a space.
	 *
	 * 	@param where - output stream
	 */
	void print_basic(std::ostream& where) const override;

	//
	// Infection related computations
	//

	/**
	 * \brief Calculates and stores probability contribution of infected agents if any 
	 *
	 * @param infection - infection object class instance
	 */
	void compute_infected_contribution() override;

	/** 
	 *  \brief Include contribution of a symptomatic, home isolated agent in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_symptomatic_home_isolated(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_ih; ++num_infected; }

	/** 
	 *  \brief Include contribution of an exposed , home isolated agent in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_exposed_home_isolated(double inf_var) 
		{ lambda_sum += inf_var*beta_ih; ++num_infected; }

private:
	// Household size scaling factor
	double alpha = 0.0;
	// Home isolated transmission rate
	double beta_ih = 0.0;
};

#endif
