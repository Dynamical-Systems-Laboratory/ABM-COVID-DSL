#include "../include/contributions.h"

/***************************************************** 
 * class: Contributions
 *
 * Functionality for computing of infection probability
 * contributions from agents to places and eventually 
 * mobility 
 * 
 ******************************************************/

// Count contributions of an exposed agent
void Contributions::compute_exposed_contributions(const Agent& agent, const double time,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				std::vector<RetirementHome>& retirement_homes)
{
	// Skip if not yet infectious
	if (time < agent.get_infectiousness_start_time()){
		return;
	}
	
	// Agent's infection variability
	double inf_var = 0.0;
	inf_var = agent.get_inf_variability_factor();

	// If main state "tested"
	if (agent.tested()){
		if (agent.get_time_of_test() <= time && agent.tested_awaiting_test() == true){
			// If being tested at this step 			
			if (agent.tested_in_hospital() == true){
				compute_hospital_tested_contributions(agent, inf_var, hospitals);
			}else if (agent.tested_in_car() == true){
				return;
			}else{
				throw std::runtime_error("Agents testing site not specified");
			}
		} else if (agent.tested_awaiting_results() || agent.tested_awaiting_test()){
			// Assuming home isolation except for hospital employees and non-covid
			if (agent.hospital_non_covid_patient() == false && 
					agent.hospital_employee() == false){
				compute_home_isolated_contributions(agent, inf_var, households, retirement_homes);
			} else if (agent.hospital_non_covid_patient()){
				Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
				hospital.add_exposed_patient(inf_var);
			} else if (agent.hospital_employee()){
				Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
				hospital.add_exposed(inf_var);
				// Household
				Household& household = households.at(agent.get_household_ID()-1);
				household.add_exposed(inf_var);
				// Other places
				if (agent.student() == true){
					School& school = schools.at(agent.get_school_ID()-1);
					school.add_exposed(inf_var);	
				}
			}
		}
	}else{
		// If hospitalized with a different condition, 
		// and exposed (infectious), only hospital contribution
		if (agent.hospital_non_covid_patient() == true &&
				agent.tested_covid_positive() == false){
			Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
			hospital.add_exposed_patient(inf_var);
			return;
		}
	
		// Exposed confirmed COVID in home isolation
		if (agent.tested_covid_positive()){
			compute_home_isolated_contributions(agent, inf_var, households, retirement_homes);
			return;
		}

		// Household or retirement home
		if (agent.retirement_home_resident()){
			RetirementHome& rh = retirement_homes.at(agent.get_household_ID()-1);
			rh.add_exposed(inf_var);
		} else {
			Household& household = households.at(agent.get_household_ID()-1);
			household.add_exposed(inf_var);
		}

		// Other places
		if (agent.student() == true){
			School& school = schools.at(agent.get_school_ID()-1);
			school.add_exposed(inf_var);	
		}
		if (agent.works() == true){
			if (agent.retirement_home_employee()){
				RetirementHome& rh = retirement_homes.at(agent.get_work_ID()-1);
				rh.add_exposed_employee(inf_var);
			} else if (agent.school_employee()){
				School& sch = schools.at(agent.get_work_ID()-1);
				sch.add_exposed_employee(inf_var);
			} else {
				Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
				workplace.add_exposed(inf_var);
			}
		}
		if (agent.hospital_employee() == true){
			Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
			hospital.add_exposed(inf_var);
		}
	}
}

// Count contributions of a symptomatic agent
void Contributions::compute_symptomatic_contributions(const Agent& agent, const double time,	
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					std::vector<RetirementHome>& retirement_homes)
{
	// Agent's infection variability
	double inf_var = 0.0;
	inf_var = agent.get_inf_variability_factor();

	// If main state "tested"
	if (agent.tested()){
		// If awaiting results 
		if (agent.get_time_of_test() <= time && agent.tested_awaiting_test() == true){
			// If being tested at this step
			if (agent.tested_in_hospital() == true){
				compute_hospital_tested_contributions(agent, inf_var, hospitals);
			}else if (agent.tested_in_car() == true){
				return;
			}else{
				throw std::runtime_error("Agents testing site not specified");
			}
		} else if (agent.tested_awaiting_results() || agent.tested_awaiting_test()){
			// Assuming home isolation except for hospital patients formerly
			// non-COVID
			if (agent.hospital_non_covid_patient() == false){
				compute_home_isolated_contributions(agent, inf_var, households, retirement_homes);
			}else{
				compute_hospitalized_contributions(agent, inf_var, hospitals);
			}		
		}
	} else if (agent.being_treated()){ 
		// If getting treatment
		if (agent.home_isolated() == true){
			compute_home_isolated_contributions(agent, inf_var, households, retirement_homes);
		}else if (agent.hospitalized() == true){
			compute_hospitalized_contributions(agent, inf_var, hospitals);
		}else if (agent.hospitalized_ICU() == true){
			compute_hospitalized_ICU_contributions(agent, inf_var, hospitals);
		}
	} else {
		if ((agent.tested_false_negative() && (agent.hospital_non_covid_patient()))
						|| (agent.hospital_non_covid_patient())){
			Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
			hospital.add_symptomatic_patient(inf_var);	
		} else {
			// If regular symptomatic
			compute_regular_symptomatic_contributions(agent, inf_var, households,
								schools, workplaces, retirement_homes);
		}
	}
}

// Compute the total contribution to infection probability at every place
void Contributions::total_place_contributions(std::vector<Household>& households, 
					std::vector<School>& schools, std::vector<Workplace>& workplaces, 
					std::vector<Hospital>& hospitals, std::vector<RetirementHome>& retirement_homes)
{	
	auto infected_contribution = [](Place& place){ place.compute_infected_contribution(); };

	std::for_each(households.begin(), households.end(), infected_contribution);
	std::for_each(retirement_homes.begin(), retirement_homes.end(), infected_contribution);
	std::for_each(schools.begin(), schools.end(), infected_contribution);
	std::for_each(workplaces.begin(), workplaces.end(), infected_contribution);
	std::for_each(hospitals.begin(), hospitals.end(), infected_contribution);
}

// Count contributions of a untreated and not tested symptomatic agent
void Contributions::compute_regular_symptomatic_contributions(const Agent& agent, 
				const double inf_var, std::vector<Household>& households, 
				std::vector<School>& schools, std::vector<Workplace>& workplaces,
				std::vector<RetirementHome>& retirement_homes)
{
	// Household or retirement home
	if (agent.retirement_home_resident()){
		RetirementHome& rh = retirement_homes.at(agent.get_household_ID()-1);
		rh.add_symptomatic(inf_var);
	} else {
		Household& household = households.at(agent.get_household_ID()-1);
		household.add_symptomatic(inf_var);
	}

	// Other places
	if (agent.student() == true){
		School& school = schools.at(agent.get_school_ID()-1);
		school.add_symptomatic_student(inf_var);	
	}
	if (agent.works() == true){
		if (agent.retirement_home_employee()){
			RetirementHome& rh = retirement_homes.at(agent.get_work_ID()-1);
			rh.add_symptomatic_employee(inf_var);
		} else if (agent.school_employee()){
			School& sch = schools.at(agent.get_work_ID()-1);
			sch.add_symptomatic_employee(inf_var);
		} else {
			Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
			workplace.add_symptomatic(inf_var);
		}
	}
}

/// \brief Count contributions of a symptomatic-hospital tested agent
/// \details At that time step this only refers to testing
void Contributions::compute_hospital_tested_contributions(const Agent& agent, 
				const double inf_var, std::vector<Hospital>& hospitals)   
{
	Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
	if (agent.exposed()){
		hospital.add_exposed_hospital_tested(inf_var);
	}else{
		hospital.add_hospital_tested(inf_var);
	}
	if (agent.home_isolated()){
		hospital.increase_total_tested();
	}
}

/// \brief Count contributions of a home-isolated agent 
void Contributions::compute_home_isolated_contributions(const Agent& agent, 
				const double inf_var, std::vector<Household>& households,
				std::vector<RetirementHome>& retirement_homes)   
{
	if (agent.retirement_home_resident()){
		RetirementHome& rh = retirement_homes.at(agent.get_household_ID()-1);
		if (agent.exposed()){
			rh.add_exposed_home_isolated(inf_var);
		}else{
			rh.add_symptomatic_home_isolated(inf_var);
		}			
	} else {
		Household& household = households.at(agent.get_household_ID()-1);
		if (agent.exposed()){
			household.add_exposed_home_isolated(inf_var);
		}else{
			household.add_symptomatic_home_isolated(inf_var);
		}	
	}
}

/// \brief Count contributions of a hospitalized agent
void Contributions::compute_hospitalized_contributions(const Agent& agent, 
				const double inf_var, std::vector<Hospital>& hospitals)   
{
	Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
	hospital.add_hospitalized(inf_var);
}

/// \brief Count contributions of an agent hospitalized in ICU
void Contributions::compute_hospitalized_ICU_contributions(const Agent& agent, 
				const double inf_var, std::vector<Hospital>& hospitals)   
{
	Hospital& hospital = hospitals.at(agent.get_hospital_ID()-1);
	hospital.add_hospitalized_ICU(inf_var);
}

/// \brief Set contributions/sums from all agents in places to 0.0 
void Contributions::reset_sums(std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals, 
					std::vector<RetirementHome>& retirement_homes)
{
	auto reset_contributions = [](Place& place){ place.reset_contributions(); };

	std::for_each(households.begin(), households.end(), reset_contributions);
	std::for_each(retirement_homes.begin(), retirement_homes.end(), reset_contributions);
	std::for_each(schools.begin(), schools.end(), reset_contributions);
	std::for_each(workplaces.begin(), workplaces.end(), reset_contributions);
	std::for_each(hospitals.begin(), hospitals.end(), reset_contributions);
}


