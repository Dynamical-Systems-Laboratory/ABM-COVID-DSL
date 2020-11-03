#ifndef WORKPLACE_H
#define WORKPLACE_H

#include "place.h"

class Place;

/***************************************************** 
 * class: Workplace 
 * 
 * Defines and stores attributes of a single workplace
 * 
 *****************************************************/

class Workplace : public Place{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates a Workplace object with default attributes
	 */
	Workplace() = default;

	/**
	 * \brief Creates a Workplace object 
	 * \details Hospital with custom ID, location, and infection parameters
	 *
	 * @param work_ID - ID of the workplace
	 * @param xi - x coordinate of the workplace 
	 * @param yi - y coordinate of the workplace
	 * @param severity_cor - severity correction for symptomatic
	 * @param psi - absenteeism correction
	 * @param beta - infection transmission rate, 1/time
	 */
	Workplace(const int work_ID, const double xi, const double yi,
			 const double severity_cor, const double psi, const double beta) : 
			psi_j(psi), Place(work_ID, xi, yi, severity_cor, beta){ }

	//
	// Infection related computations
	//

	/** 
	 *  \brief Include symptomatic contribution in the sum
	 *	@param inf_var - agent infectiousness variability factor
	 */
	void add_symptomatic(double inf_var) override { lambda_sum += inf_var*ck*beta_j*psi_j; }


	//
	// Setters
	//
	
	void change_absenteeism_correction(const double val) { psi_j = val; }

	//
	// Getters
	//
	
	double get_absenteeism_correction() const { return psi_j; }

	//
 	// I/O
	//

	/**
	 * \brief Save information about a Workplace object
	 * \details Saves to a file, everything but detailed agent 
	 * 		information; order is ID | x | y | number of agents | 
	 * 		number of infected agents | ck | beta_j | psi_j 
	 * 		Delimiter is a space.
	 * 	@param where - output stream
	 */
	void print_basic(std::ostream& where) const override;

private:
	// Absenteeism correction
	double psi_j = 0.0;
};
#endif
