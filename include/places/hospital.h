#ifndef HOSPITAL_H
#define HOSPITAL_H

#include "place.h"

class Place;

/***************************************************** 
 * class: Hospital 
 * 
 * Defines and stores attributes of a single hospital 
 * 
 *****************************************************/

class Hospital : public Place{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates a Hospital object with default attributes
	 */
	Hospital() = default;

	/**
	 * \brief Creates a Hospital object 
	 * \details Hospital with custom ID, location, and infection parameters
	 *
	 * @param hospital_ID - ID of the hospital 
	 * @param xi - x coordinate of the hospital 
	 * @param yi - y coordinate of the hospital
	 * @param severity_cor - severity correction for symptomatic
	 * @param betas - map of infection transmission rates, 1/time; name : value
	 */
	Hospital(const int hospital_ID, const double xi, const double yi,
			 const double severity_cor, const std::map<const std::string, const double> betas) : 
			Place(hospital_ID, xi, yi, severity_cor, 0.0)
	{ 
				beta_employee = betas.at("hospital employee"); 
				beta_non_covid_patient = betas.at("hospital non-COVID patient");
				beta_testee = betas.at("hospital testee");
				beta_hospitalized = betas.at("hospitalized");
				beta_hospitalized_ICU = betas.at("hospitalized ICU");
	}

	//
	// Infection related computations
	//
	
	/** 
	 *  \brief Include exposed employee contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_exposed(double inf_var) override 
		{ lambda_sum += inf_var*beta_employee; ++num_infected; }

	/** 
	 *  \brief Include exposed contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_exposed_patient(double inf_var) 
		{ lambda_sum += inf_var*beta_non_covid_patient; ++num_infected; }

	/** 
	 *  \brief Include symptomatic  contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_symptomatic_patient(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_non_covid_patient; ++num_infected; }

	/** 
	 *  \brief Include tested at hospital contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_hospital_tested(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_testee; ++num_infected; }

	/** 
	 *  \brief Include tested at hospital contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_exposed_hospital_tested(double inf_var) 
		{ lambda_sum += inf_var*beta_testee; ++num_infected; }

	/** 
	 *  \brief Include hospitalized contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_hospitalized(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_hospitalized; ++num_infected; }

	/** 
	 *  \brief Include hospitalized in ICU contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_hospitalized_ICU(double inf_var) 
		{ lambda_sum += inf_var*ck*beta_hospitalized_ICU; ++num_infected;}

	/// \brief Increase number of tested at that time step
	void increase_total_tested() { n_tested++; }

  	/// \brief Reset select variables of a place after transmission step
    void reset_contributions() override 
		{ lambda_tot = 0.0; lambda_sum = 0.0; num_infected = 0; n_tested = 0; }

	/// \brief Contribution takes into account agents tested at current step
	void compute_infected_contribution() override;

	/// \brief Returns number of Flu agents being tested in a hospital at that step
	int get_n_tested() const { return n_tested; }

	/// \brief Returns number of Flu agents being tested in a hospital at that step
	double get_lambda_sum() const { return lambda_sum; }

	//
 	// I/O
	//

	/**
	 * \brief Save information about a Hospital object
	 * \details Saves to a file, everything but detailed agent 
	 * 		information; order is ID | x | y | number of agents | 
	 * 		number of infected agents | ck | beta_1, 2, and 3 
	 * 		Delimiter is a space.
	 * 	@param where - output stream
	 */
	void print_basic(std::ostream& where) const override;

private:
	// Transmission rates that depend on the role in 
	// the hospital, all units are 1/time 
	double 	beta_employee = 0.0;
	double 	beta_non_covid_patient = 0.0;
	double 	beta_testee = 0.0;
	double 	beta_hospitalized = 0.0;
	double  beta_hospitalized_ICU = 0.0;

	// Number of agents being tested at a given 
	// time step (for infection probability)
	// this is both infected and not
	int n_tested = 0;
};
#endif
