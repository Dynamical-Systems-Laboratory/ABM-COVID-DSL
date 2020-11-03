#include "contributions_tests.h"

/***************************************************** 
 *
 * Test suite for Contributions class
 *
 ******************************************************/

// Tests
bool contributions_main_test();
bool contributions_treatment_test();
bool contributions_misc_test();

// Supporting functions
bool check_all_places(ABM&, const std::vector<Agent>&);
void remove_agent_from_public_places(Agent& agent, std::vector<RetirementHome>& retirement_homes, 
										std::vector<School>& schools, std::vector<Workplace>& workplaces, 
										std::vector<Hospital>& hospitals);
// Necessary files
// test_data/houses_test.txt
// test_data/schools_test.txt
// test_data/workplaces_test.txt
// test_data/hospitals_test.txt
// test_data/agents_test.txt
// test_data/infection_parameters.txt
// test_data/tests_with_time.txt
// test_data/age_dist_hospitalization.txt
// test_data/age_dist_hosp_ICU.txt
// test_data/age_dist_mortality.txt
// test_data/age_dist_exposed_never_sy.txt

int main()
{
	test_pass(contributions_main_test(), "Computations of contributions, regular and tested");
	test_pass(contributions_treatment_test(), "Computations of contributions, treated");
	test_pass(contributions_misc_test(), "Computations of contributions, misc");
}

/// Test for correct computing of infection contributions
bool contributions_main_test()
{
	// Create agents 
	std::string fin("test_data/agents_test.txt");

	// Files with place info
	std::string hfile("test_data/houses_test.txt");
	std::string sfile("test_data/schools_test.txt");
	std::string wfile("test_data/workplaces_test.txt");
	std::string hspfile("test_data/hospitals_test.txt");
	std::string rh_file("test_data/rh_test.txt");

	// Model parameters
	double dt = 0.5;
	double time = 1.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dexp_name("test_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"exposed never symptomatic", dexp_name}, {"hospitalization", dh_name}, 
		  {"ICU", dhicu_name}, {"mortality", dmort_name} };	
	// File with 	
	std::string tfname("test_data/tests_with_time.txt");
	
	ABM abm(dt, pfname, dfiles, tfname);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	abm.create_hospitals(hspfile);
	abm.create_retirement_homes(rh_file);
	// Then the agents
	abm.create_agents(fin);
	const std::vector<Agent>& agents = abm.get_vector_of_agents();
	// Hospital reference for testing
	std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	// Hospital where agents are tested (unless employees)
	int tst_hsp_ID = 1;

	// 1) Initial contributions - exposed only
	if (!check_all_places(abm, agents)){
		std::cerr << "Error in initial contributions" << std::endl;
		return false;
	}

	// 2) Contributions after introducing symptomatic
	std::vector<Agent>& nc_agents = abm.vector_of_agents();	
	// Change ~75% of exposed to symptomatic
	for (auto& agent : nc_agents){
		if (agent.hospital_employee() || agent.hospital_non_covid_patient()){
			continue;
		}
		if (agent.exposed() && static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.75){
			agent.set_exposed(false);
			agent.set_symptomatic(true);
		}
	}
	if (!check_all_places(abm, nc_agents)){
		std::cerr << "Error in contributions of exposed and symptomatic" << std::endl;
		return false;
	}

	// 3) Contributions with testing
	for (auto& agent : nc_agents){
		if (agent.infected() == false){
			continue;
		}
		if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.50){
			// Add symptomatic hospital patients and employees
			if (agent.hospital_employee() || agent.hospital_non_covid_patient()){
				if (agent.exposed() && std::rand()/RAND_MAX<0.75){
					agent.set_exposed(false);
					agent.set_symptomatic(true);
					agent.set_tested_in_hospital(true);
					if (agent.hospital_non_covid_patient()){
						agent.set_home_isolated(false);
					}else{
						// Remove from the hospital
						agent.set_home_isolated(true);
						hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent.get_ID());
					}	
				}
			}else{
				agent.set_home_isolated(true);
				// Locations
				if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.50){
					agent.set_tested_in_car(true);
				}else{
					agent.set_tested_in_hospital(true);
					agent.set_hospital_ID(tst_hsp_ID);
				}
			}
			agent.set_tested(true);
		
			// Phase
			if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.50){
				// Waiting for test - home isolation
				agent.set_tested_awaiting_test(true);
				if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.50){
					// Tested now
					agent.set_time_to_test(-1.0);
				    agent.set_time_of_test(0.0);
					if (agent.tested_in_hospital() && agent.home_isolated()){
						// Register so this agent is accounted for though the ntot will
						// be higher than it should
						if (agent.hospital_employee()){
							hospitals.at(agent.get_hospital_ID()-1).add_agent(agent.get_ID());
						} else {
							hospitals.at(tst_hsp_ID-1).add_agent(agent.get_ID());
						}
					}
				}else{
					agent.set_time_to_test(1.0);
				    agent.set_time_of_test(0.0);
				}
			}else{
				// Waiting for results
				agent.set_tested_awaiting_results(true);
				agent.set_time_to_test(1.0);
				agent.set_time_of_test(0.0);
			}
		}
	}
	if (!check_all_places(abm, nc_agents)){
		std::cerr << "Error in contributions with tested agents" << std::endl;
		return false;
	}
	return true;
}

/// Test for correct computing of infection contributions for treated agents
bool contributions_treatment_test()
{
	// Create agents 
	std::string fin("test_data/agents_test.txt");

	// Files with place info
	std::string hfile("test_data/houses_test.txt");
	std::string sfile("test_data/schools_test.txt");
	std::string wfile("test_data/workplaces_test.txt");
	std::string hspfile("test_data/hospitals_test.txt");
	std::string rh_file("test_data/rh_test.txt");

	// Model parameters
	double dt = 0.5;
	double time = 1.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dexp_name("test_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"exposed never symptomatic", dexp_name}, {"hospitalization", dh_name}, 
		  {"ICU", dhicu_name}, {"mortality", dmort_name} };	
	// File with 	
	std::string tfname("test_data/tests_with_time.txt");
	
	ABM abm(dt, pfname, dfiles, tfname);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	abm.create_hospitals(hspfile);
	abm.create_retirement_homes(rh_file);
	// Then the agents
	abm.create_agents(fin);
	const std::vector<Agent>& agents = abm.get_vector_of_agents();
	// Hospital reference for testing
	std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	// Household references for testing
	std::vector<Household>& households = abm.vector_of_households();
	// Retirement home references for testing
	std::vector<RetirementHome>& retirement_homes = abm.vector_of_retirement_homes();
	// Schools references for testing
	std::vector<School>& schools = abm.vector_of_schools();
	// Workplaces references for testing
	std::vector<Workplace>& workplaces = abm.vector_of_workplaces();

	// Hospital where agents are tested (unless employees)
	int tst_hsp_ID = 1;
	// Hospital for treatment
	int tr_hsp_ID = hospitals.size();
	// Household for home isolated hospital patients
	int tr_hs_ID = std::max(1, static_cast<int>(households.size())-5);

	// Introduce symptomatic agents
	std::vector<Agent>& nc_agents = abm.vector_of_agents();	
	for (auto& agent : nc_agents){
		if (agent.exposed() && static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.75){
			agent.set_exposed(false);
			agent.set_symptomatic(true);
		}
	}

	// Contributions from treatment
	for (auto& agent : nc_agents){
		if (agent.infected() == false){
			continue;
		}
		// Reset testing and old home isolation flags
		agent.set_tested(false);
		agent.set_home_isolated(false);
		if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.75){
			if (agent.exposed()){
				// Only IH
				agent.set_being_treated(true);
				agent.set_home_isolated(true);
				// If not tested than IH only for confirmed positive
				agent.set_tested_covid_positive(true);
				if (agent.hospital_non_covid_patient()){
					hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent.get_ID());
					households.at(tr_hs_ID-1).add_agent(agent.get_ID());
					agent.set_household_ID(tr_hs_ID);
				} else {
					remove_agent_from_public_places(agent, retirement_homes, schools, workplaces, 
									hospitals);
				}
			}else{
				agent.set_being_treated(true);
				if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.25){
					// Hospitalized
					agent.set_hospitalized(true);
					if ((!agent.hospital_non_covid_patient()) && (!agent.hospital_employee())){
						hospitals.at(tr_hsp_ID-1).add_agent(agent.get_ID());
						agent.set_hospital_ID(tr_hsp_ID);
						households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
						remove_agent_from_public_places(agent, retirement_homes, schools, workplaces, 
									hospitals);
					}else if (agent.hospital_employee()){
						households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
						remove_agent_from_public_places(agent, retirement_homes, schools, workplaces, 
									hospitals);
					}
				} else if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.5) {
					// Hospitalized ICU
					agent.set_hospitalized_ICU(true);
					if ((!agent.hospital_non_covid_patient()) && (!agent.hospital_employee())){
						hospitals.at(tr_hsp_ID-1).add_agent(agent.get_ID());
						agent.set_hospital_ID(tr_hsp_ID);
						households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
						remove_agent_from_public_places(agent, retirement_homes, schools, workplaces, 
									hospitals);
					} else if (agent.hospital_employee()){
						households.at(agent.get_household_ID()-1).remove_agent(agent.get_ID());
						remove_agent_from_public_places(agent, retirement_homes, schools, workplaces, 
									hospitals);
					}
				} else {
					agent.set_home_isolated(true);
					if (agent.hospital_non_covid_patient()){
						hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent.get_ID());
						households.at(tr_hs_ID-1).add_agent(agent.get_ID());
						agent.set_household_ID(tr_hs_ID);
						remove_agent_from_public_places(agent, retirement_homes, schools, workplaces, 
									hospitals);
					}
					remove_agent_from_public_places(agent, retirement_homes, schools, workplaces, 
									hospitals);
				}
			}
		} else if (agent.hospital_non_covid_patient()){
			agent.set_being_treated(true);
			agent.set_hospitalized(true);		
		}
	}
	if (!check_all_places(abm, nc_agents)){
		std::cerr << "Error in contributions with treated agents" << std::endl;
		return false;
	}
	return true;
}

/// Test for correct computing of infection contributions of various other types of agents
bool contributions_misc_test()
{
	// Create agents 
	std::string fin("test_data/agents_test.txt");

	// Files with place info
	std::string hfile("test_data/houses_test.txt");
	std::string sfile("test_data/schools_test.txt");
	std::string wfile("test_data/workplaces_test.txt");
	std::string hspfile("test_data/hospitals_test.txt");
	std::string rh_file("test_data/rh_test.txt");

	// Model parameters
	double dt = 0.5;
	double time = 1.0;
	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dexp_name("test_data/age_dist_exposed_never_sy.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"exposed never symptomatic", dexp_name}, {"hospitalization", dh_name}, 
		  {"ICU", dhicu_name}, {"mortality", dmort_name} };	
	// File with 	
	std::string tfname("test_data/tests_with_time.txt");
	
	ABM abm(dt, pfname, dfiles, tfname);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	abm.create_hospitals(hspfile);
	abm.create_retirement_homes(rh_file);
	// Then the agents
	abm.create_agents(fin);
	const std::vector<Agent>& agents = abm.get_vector_of_agents();
	// Hospital reference for testing
	std::vector<Hospital>& hospitals = abm.vector_of_hospitals();
	// Household references for testing
	std::vector<Household>& households = abm.vector_of_households();
	// Retirement home references for testing
	std::vector<RetirementHome>& retirement_homes = abm.vector_of_retirement_homes();
	// Schools references for testing
	std::vector<School>& schools = abm.vector_of_schools();
	// Workplaces references for testing
	std::vector<Workplace>& workplaces = abm.vector_of_workplaces();

	// Hospital where agents are tested (unless employees)
	int tst_hsp_ID = 1;
	// Hospital for treatment
	int tr_hsp_ID = hospitals.size();
	// Household for home isolated hospital patients
	int tr_hs_ID = std::max(1, static_cast<int>(households.size())-5);

	// 1) False negatives
	// Introduce symptomatic agents
	std::vector<Agent>& nc_agents = abm.vector_of_agents();	
	for (auto& agent : nc_agents){
		if (agent.hospital_employee() || agent.hospital_non_covid_patient()){
			continue;
		}
		if (agent.exposed() && static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.75){
			agent.set_exposed(false);
			agent.set_symptomatic(true);
		}
		// Check false negatives
		if (static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)<0.75){
			agent.set_tested_false_negative(true);	
		}
	}
	if (!check_all_places(abm, nc_agents)){
		std::cerr << "Error in misc contributions" << std::endl;
		return false;
	}

	// 2) Increasing the number of agents due to testing (i.e. flu agents) without changing lambda
	Hospital& hospital = hospitals.at(0);
	double lambda_sum = hospital.get_lambda_sum();
	int ntot0 = hospital.get_agent_IDs().size();
	// Now increase number of agents and compare
	hospital.increase_total_tested();
	if (!float_equality<double>(lambda_sum, hospital.get_lambda_sum(), 1e-5)){
		std::cerr << "Error in increasing number of tested agents in a hospital - sum in lambda should stay constant" << std::endl;
		return false; 
	}
	int n_tested = 1;
	if (!float_equality<double>(n_tested, hospital.get_n_tested(), 1e-5)){
		std::cerr << "Error in increasing number of tested agents in a hospital - number of tested flu agents doesn't match" << std::endl;
		return false; 
	}	
	double new_lambda = lambda_sum/static_cast<double>(ntot0+n_tested);
	hospital.compute_infected_contribution();
	if (!float_equality<double>(new_lambda, hospital.get_infected_contribution(), 1e-5)){
		std::cerr << "Error in increasing number of tested agents in a hospital - wrong lambda" << std::endl;
		return false; 
	}
	return true;
}


bool check_all_places(ABM& abm, const std::vector<Agent>& agents)
{
    // Infection parameters as loaded
	const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 

	// Reset, then compute contributions and collect the places
	abm.reset_contributions();
	abm.compute_place_contributions();
	const std::vector<Household>& households = abm.get_vector_of_households(); 
    const std::vector<RetirementHome>& retirement_homes = abm.get_vector_of_retirement_homes();
    const std::vector<School>& schools = abm.get_vector_of_schools(); 
    const std::vector<Workplace>& workplaces = abm.get_vector_of_workplaces(); 
    const std::vector<Hospital>& hospitals = abm.get_vector_of_hospitals();

	if (!check_regular_contributions<Household>(households, agents, "household", infection_parameters)){
		std::cerr << "Wrong contribution in the household computation" << std::endl;
		return false;
	}
	if (!check_regular_contributions<RetirementHome>(retirement_homes, agents, "retirement home", infection_parameters)){
		std::cerr << "Wrong contribution in the retirement home computation" << std::endl;
		return false;
	}
	if (!check_regular_contributions<School>(schools, agents, "school", infection_parameters)){
		std::cerr << "Wrong contribution in the school computation" << std::endl;
		return false;
	}
	if (!check_regular_contributions<Workplace>(workplaces, agents, "workplace", infection_parameters)){
		std::cerr << "Wrong contribution in the workplace computation" << std::endl;
		return false;
	}
	if (!check_regular_contributions<Hospital>(hospitals, agents, "hospital", infection_parameters)){
		std::cerr << "Wrong contribution in the hospital computation" << std::endl;
		return false;
	}
	return true;
}

/// Remove agent from all public places (hospital only if not hospitalized)
void remove_agent_from_public_places(Agent& agent, std::vector<RetirementHome>& retirement_homes, 
										std::vector<School>& schools, std::vector<Workplace>& workplaces, 
										std::vector<Hospital>& hospitals)
{
	// No error if already removed due to hospitalization
	int agent_ID = agent.get_ID();

	if (agent.student()){
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
	}

	if (agent.works()){
		if (agent.retirement_home_employee()){
			retirement_homes.at(agent.get_work_ID()-1).remove_agent(agent_ID);
		} else if (agent.school_employee()){
			schools.at(agent.get_work_ID()-1).remove_agent(agent_ID);
		} else {
			workplaces.at(agent.get_work_ID()-1).remove_agent(agent_ID);
		}
	}

	if (agent.hospital_employee() && agent.home_isolated()){
		hospitals.at(agent.get_hospital_ID()-1).remove_agent(agent_ID);
	}
}
