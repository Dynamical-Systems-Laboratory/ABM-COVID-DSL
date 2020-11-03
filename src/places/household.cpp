#include "../../include/places/household.h"

/***************************************************** 
 * class: Household
 * 
 * Defines and stores attributes of a single household
 * 
 *****************************************************/

//
// I/O
//

// Save information about a Household object
void Household::print_basic(std::ostream& where) const
{
	Place::print_basic(where);
	where << " " << alpha << " " << beta_ih;	
}

// Calculates and stores fraction of infected agents if any 
void Household::compute_infected_contribution()
{
	num_tot = agent_IDs.size();
	
	if (num_tot == 0)
		lambda_tot = 0.0;
	else
		lambda_tot = lambda_sum/std::pow(static_cast<double>(num_tot), alpha);
}


