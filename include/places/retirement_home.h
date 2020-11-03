#ifndef RETIREMENT_HOME_H
#define RETIREMENT_HOME_H

#include "place.h"

class Place;

/***************************************************** 
 * class: RetirementHome 
 * 
 * Defines and stores attributes of a single 
 * retirement home 
 * 
 *****************************************************/

class RetirementHome : public Place{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates a RetirementHome object with default attributes
	 */
	RetirementHome() = default;

	/**
	 * \brief Creates a RetirementHome object
	 * \details Retirement home with custom ID, location, and infection parameters
	 *
	 * @param rh_ID - ID of the home
	 * @param xi - x coordinate of the home
	 * @param yi - y coordinate of the home
	 * @param severity_cor - severity correction for symptomatic
	 * @param psi_e - employee absenteeism correction
	 * @param beta_e - employee infection transmission rate, 1/time
	 * @param beta_r - resident infection transmission rate, 1/time
	 * @param betaih - infection transmission rate for home isolated, 1/time
	 */
	RetirementHome(const int rh_ID, const double xi, const double yi, 
			const double severity_cor, const double psi_e, const double beta_e, 
			const double beta_r, const double betaih) : 
		psi_emp(psi_e), beta_emp(beta_e), beta_ih(betaih), Place(rh_ID, xi, yi, severity_cor, beta_r){ } 

	//
	// Infection related computations
	//

	/** 
	 *  \brief Include exposed employee contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_exposed_employee(double inf_var) 
		{ lambda_sum += inf_var*beta_emp; ++num_infected; }

	/** 
	 *  \brief Include symptomatic employee contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_symptomatic_employee(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_emp*psi_emp; ++num_infected; }

	/** 
	 *  \brief Include contribution of a symptomatic, home isolated agent in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_symptomatic_home_isolated(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_ih; ++num_infected;}

	/** 
	 *  \brief Include contribution of an exposed, home isolated agent in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_exposed_home_isolated(double inf_var) 
		{ lambda_sum += inf_var*beta_ih; ++num_infected; }

	//
 	// I/O
	//

	/**
	 * \brief Save information about a RetirementHome object
	 * \details Saves to a file, everything but detailed agent 
	 * 		information; order is ID | x | y | number of agents | 
	 * 		number of infected agents | ck | betas | psis 
	 * 		Delimiter is a space.
	 * @param where - output stream
	 */
	void print_basic(std::ostream& where) const override;

private:
	// Absenteeism correction - employee
	double psi_emp = 0.0;
	// Employee infection transmission rate, 1/time 
	double beta_emp = 0.0;
	// Home isolated infection transmission rate, 1/time
	double beta_ih = 0.0;
};

#endif
