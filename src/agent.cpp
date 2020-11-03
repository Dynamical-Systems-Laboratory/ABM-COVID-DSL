#include "../include/agent.h"

/***************************************************** 
 * class: Agent
 * 
 * Defines and stores attributes of a single agent
 * 
 *****************************************************/

//
// I/O
//

// Print Agent information 
void Agent::print_basic(std::ostream& where) const
{
	where << ID << " " << is_student << " " << is_working  
		  << " " << age << " " << x << " " << y << " "
		  << house_ID << " " << is_non_covid_patient << " " << school_ID 
		  << " " << work_ID << " " << works_at_hospital 
		  << " " << hospital_ID << " " << worksRH 
		  << " " << worksSch << " " << livesRH << " "<< is_infected;	
}

//
// Infection related computations
//

//
// Supporting functions
//

// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Agent& agent) 
{
	agent.print_basic(out);
	return out;
}


