#include "../../include/places/hospital.h"

/***************************************************** 
 * class: Hospital
 * 
 * Defines and stores attributes of a single hospital 
 * 
 *****************************************************/

//
// Infection related computations
//

// Calculates and stores probability contribution 
// from exposedi and symptoamtic agents if any 
void Hospital::compute_infected_contribution()
{
	num_tot = agent_IDs.size() + n_tested;
	if (num_tot == 0){
		lambda_tot = 0.0;
	}else{
		lambda_tot = lambda_sum/(static_cast<double>(num_tot));	
	}
}

//
// I/O
//

// Save information about a Hospital object
void Hospital::print_basic(std::ostream& where) const
{
	where << ID << " " << x << " " << y << " "
		  << num_tot << " " << num_infected << " "
		  << ck << " " << beta_employee << " "
		  << beta_non_covid_patient << " " 
		  << beta_testee << " " << beta_hospitalized 
		  << " " << beta_hospitalized_ICU;	
}


