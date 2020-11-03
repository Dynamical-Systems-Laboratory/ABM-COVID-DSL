#include "../../include/places/retirement_home.h"

/***************************************************** 
 * class: RetirementHome 
 * 
 * Defines and stores attributes of a single 
 * retirement home 
 * 
 *****************************************************/

//
// I/O
//

// Save information about a RetirementHome object
void RetirementHome::print_basic(std::ostream& where) const
{
	Place::print_basic(where);
	where << " " << beta_emp << " " << beta_ih << " " << psi_emp;	
}


