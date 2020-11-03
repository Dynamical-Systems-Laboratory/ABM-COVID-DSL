#include "../../include/places/workplace.h"

/***************************************************** 
 * class: Workplace
 * 
 * Defines and stores attributes of a single workplace 
 * 
 *****************************************************/

//
// I/O
//

// Save information about a Workplace object
void Workplace::print_basic(std::ostream& where) const
{
	Place::print_basic(where);
	where << " " << psi_j;	
}


