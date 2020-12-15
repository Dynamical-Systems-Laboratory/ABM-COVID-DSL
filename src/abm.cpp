#include "../include/abm.h"

/***************************************************** 
 * class: ABM
 * 
 * Interface for agent-based modeling 
 *
 * Provides operations for creation, management, and
 * progression of an agent-based model
 *
 * Stores model-related data
 *
 * NOTE: IDs of objects correspond to their positions
 * in the vectors of objects and determine the way
 * they are accessed; IDs start with 1 but are corrected
 * by -1 when accessing;   
 * 
******************************************************/

//
// Initialization and object construction
//

// Set initial values on all the data collection variables and containers
void ABM::initialize_data_collection()
{
	n_infected_tot = 0;
	n_dead_tot = 0;
	n_dead_tested = 0;
	n_dead_not_tested = 0;
	n_recovered_tot = 0;
	n_recovering_exposed = 0;

	tot_tested = 0;
	tot_tested_pos = 0;
	tot_tested_neg = 0;
	tot_tested_false_pos = 0;
	tot_tested_false_neg = 0;

	n_infected_day = {};
	n_dead_day = {};
	n_recovered_day = {};
	tested_day = {};
	tested_pos_day = {};	
	tested_neg_day = {};
	tested_false_pos_day = {};
	tested_false_neg_day = {};
}

// Load infection parameters, store in a map
void ABM::load_infection_parameters(const std::string infile)
{
	// Load parameters
	LoadParameters ldparam;
	infection_parameters = ldparam.load_parameter_map(infile);

	// Set infection distributions
	infection.set_latency_distribution(infection_parameters.at("latency log-normal mean"),
					infection_parameters.at("latency log-normal standard deviation"));	
	infection.set_inf_variability_distribution(infection_parameters.at("agent variability gamma shape"),
					infection_parameters.at("agent variability gamma scale"));
	infection.set_onset_to_death_distribution(infection_parameters.at("otd logn mean"), 
					infection_parameters.at("otd logn std"));
	infection.set_onset_to_hospitalization_distribution(infection_parameters.at("oth gamma shape"), infection_parameters.at("oth gamma scale"));
	infection.set_hospitalization_to_death_distribution(infection_parameters.at("htd wbl shape"), infection_parameters.at("htd wbl scale"));

	// Set single-number probabilities
	infection.set_other_probabilities(infection_parameters.at("average fraction to get tested"),
									  infection_parameters.at("probability of death in ICU"), 
								  infection_parameters.at("probability dying if needing but not admitted to icu"));
}

// Load age-dependent distributions, store in a map of maps
void ABM::load_age_dependent_distributions(const std::map<std::string, std::string> dist_files)
{
	LoadParameters ldparam;
	std::map<std::string, double> one_file;
	for (const auto& dfile : dist_files){
		one_file = ldparam.load_age_dependent(dfile.second);
		for (const auto& entry : one_file){
			age_dependent_distributions[dfile.first][entry.first] = entry.second;
		}
		one_file.clear();
	}

	// Send to Infection class for further processing 
	infection.set_expN2sy_fractions(age_dependent_distributions.at("exposed never symptomatic"));
	infection.set_mortality_rates(age_dependent_distributions.at("mortality"));
	infection.set_hospitalized_fractions(age_dependent_distributions.at("hospitalization"));
	infection.set_hospitalized_ICU_fractions(age_dependent_distributions.at("ICU"));
}

// Initialize testing and its time dependence
void ABM::load_testing(const std::string fname) 
{
	// Regular properties
	testing.initialize_testing(infection_parameters.at("start testing"),
					infection_parameters.at("negative tests fraction"),
					infection_parameters.at("fraction false negative"),
					infection_parameters.at("fraction false positive"),
					infection_parameters.at("fraction to get tested"),
					infection_parameters.at("exposed fraction to get tested"));	
	// Time-dependent test fractions
	std::vector<std::vector<std::string>> file = read_object(fname);
	std::vector<std::vector<double>> fractions_times = {};
	std::vector<double> temp(3,0.0);
	for (auto& entry : file){
		for (int i=0; i<3; ++i){
			temp.at(i) = std::stod(entry.at(i));
		}
		fractions_times.push_back(temp);
	}
	testing.set_time_varying(fractions_times);
}

// Generate and store household objects
void ABM::create_households(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One household per line
	for (auto& house : file){
		// Extract properties, add infection parameters
		Household temp_house(std::stoi(house.at(0)), 
			std::stod(house.at(1)), std::stod(house.at(2)),
			infection_parameters.at("household scaling parameter"),
			infection_parameters.at("severity correction"),
			infection_parameters.at("household transmission rate"),
			infection_parameters.at("transmission rate of home isolated"));
		// Store 
		households.push_back(temp_house);
	}
}

// Generate and store retirement homes objects
void ABM::create_retirement_homes(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One household per line
	for (auto& rh : file){
		// Extract properties, add infection parameters
		RetirementHome temp_RH(std::stoi(rh.at(0)), 
			std::stod(rh.at(1)), std::stod(rh.at(2)),
			infection_parameters.at("severity correction"),
			infection_parameters.at("RH employee absenteeism factor"),
			infection_parameters.at("RH employee transmission rate"),
			infection_parameters.at("RH resident transmission rate"),
			infection_parameters.at("RH transmission rate of home isolated"));
		// Store 
		retirement_homes.push_back(temp_RH);
	}
}

// Generate and store school objects
void ABM::create_schools(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One workplace per line
	for (auto& school : file){
		// Extract properties, add infection parameters

		// School-type dependent absenteeism
		double psi = 0.0;
		std::string school_type = school.at(3);
		if (school_type == "daycare")
 			psi = infection_parameters.at("daycare absenteeism correction");
		else if (school_type == "primary" || school_type == "middle")
 			psi = infection_parameters.at("primary and middle school absenteeism correction");
		else if (school_type == "high")
 			psi = infection_parameters.at("high school absenteeism correction");
		else if (school_type == "college")
 			psi = infection_parameters.at("college absenteeism correction");
		else
			throw std::invalid_argument("Wrong school type: " + school_type);

		School temp_school(std::stoi(school.at(0)), 
			std::stod(school.at(1)), std::stod(school.at(2)),
			infection_parameters.at("severity correction"),	
			infection_parameters.at("school employee absenteeism correction"), psi,
			infection_parameters.at("school employee transmission rate"), 
			infection_parameters.at("school transmission rate"));
		
		// Store 
		schools.push_back(temp_school);
	}
}

// Generate and store workplace objects
void ABM::create_workplaces(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One workplace per line
	for (auto& work : file){
		// Extract properties, add infection parameters
		Workplace temp_work(std::stoi(work.at(0)), 
			std::stod(work.at(1)), std::stod(work.at(2)),
			infection_parameters.at("severity correction"),
			infection_parameters.at("work absenteeism correction"),
			infection_parameters.at("workplace transmission rate"));

		// Store 
		workplaces.push_back(temp_work);
	}
}

// Create hospitals based on information in a file
void ABM::create_hospitals(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One hospital per line
	for (auto& hospital : file){
		// Make a map of transmission rates for different 
		// hospital-related categories
		std::map<const std::string, const double> betas = 
			{{"hospital employee", infection_parameters.at("healthcare employees transmission rate")}, 
			 {"hospital non-COVID patient", infection_parameters.at("hospital patients transmission rate")},
			 {"hospital testee", infection_parameters.at("hospital tested transmission rate")},
			 {"hospitalized", infection_parameters.at("hospitalized transmission rate")}, 
			 {"hospitalized ICU", infection_parameters.at("hospitalized ICU transmission rate")}};

		Hospital temp_hospital(std::stoi(hospital.at(0)), 
			std::stod(hospital.at(1)), std::stod(hospital.at(2)),
			infection_parameters.at("severity correction"), betas);

		// Store 
		hospitals.push_back(temp_hospital);
	}
}

// Create agents and assign them to appropriate places
void ABM::create_agents(const std::string fname, const int ninf0)
{
	load_agents(fname, ninf0);
	register_agents();
}

// Retrieve agent information from a file
void ABM::load_agents(const std::string fname, const int ninf0)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);

	// Flu settings
	// Set fraction of flu (non-covid symptomatic)
	flu.set_fraction(infection_parameters.at("fraction with flu"));
	flu.set_fraction_tested_false_positive(infection_parameters.at("fraction false positive"));
	// Time interval for testing
	flu.set_testing_duration(infection_parameters.at("flu testing duration"));

	// For custom generation of initially infected
	std::vector<int> infected_IDs(ninf0);
	bool not_unique = true;
	int inf_ID = 0;
	if (ninf0 != 0){
		int nIDs = file.size();
		// Random choice of IDs
		for (int i=0; i<ninf0; ++i){
			not_unique = true;
			while (not_unique){
				inf_ID = infection.get_random_agent_ID(nIDs);
				auto iter = std::find(infected_IDs.begin(), infected_IDs.end(), inf_ID);
				not_unique = (iter != infected_IDs.end()); 
			}
			infected_IDs.at(i) = inf_ID;
		}
	}

	// Counter for agent IDs
	int agent_ID = 1;
	
	// One agent per line, with properties as defined in the line
	for (auto agent : file){
		// Agent status
		bool student = false, works = false, livesRH = false, worksRH = false,  
			 worksSch = false, patient = false, hospital_staff = false;
		int house_ID = -1;

		// Household ID only if not hospitalized with condition
		// different than COVID-19
		if (std::stoi(agent.at(6)) == 1){
			patient = true;
			house_ID = 0;
		}else{
			house_ID = std::stoi(agent.at(5));
		}

		// No school or work if patient with condition other than COVID
		if (std::stoi(agent.at(12)) == 1 && !patient){
			hospital_staff = true;
		}
		if (std::stoi(agent.at(0)) == 1 && !patient){
			student = true;
		}
	   	// No work if a hospital employee	
		if (std::stoi(agent.at(1)) == 1 && !(patient || hospital_staff)){
			works = true; 
		}
			
		// Random or from the input file
		bool infected = false;
		if (ninf0 != 0){
			auto iter = std::find(infected_IDs.begin(), infected_IDs.end(), agent_ID); 
			if (iter != infected_IDs.end()){
				infected_IDs.erase(iter);
				infected = true;
				n_infected_tot++;
			}
		} else {
			if (std::stoi(agent.at(14)) == 1){
				infected = true;
				n_infected_tot++;
			}
		}

		// Retirement home resident
		if (std::stoi(agent.at(8)) == 1){
			 livesRH = true;
		}

		// Retirement home or school employee
		if (std::stoi(agent.at(9)) == 1){
			 worksRH = true;
		}
		
		if (std::stoi(agent.at(10)) == 1){
			 worksSch = true;
		}

		Agent temp_agent(student, works, std::stoi(agent.at(2)), 
			std::stod(agent.at(3)), std::stod(agent.at(4)), house_ID,
			patient, std::stoi(agent.at(7)), livesRH, worksRH,
		    worksSch, std::stoi(agent.at(11)), 
			hospital_staff, std::stoi(agent.at(13)), infected);

		// Set Agent ID
		temp_agent.set_ID(agent_ID++);
		
		// Set properties for exposed if initially infected
		if (temp_agent.infected() == true){
			initial_exposed(temp_agent);
		}	
		
		// Store
		agents.push_back(temp_agent);
	}
}

// Assign agents to households, schools, and worplaces
void ABM::register_agents()
{
	int house_ID = 0, school_ID = 0, work_ID = 0, hospital_ID = 0;
	int agent_ID = 0;
	bool infected = false;

	for (const auto& agent : agents){
		
		// Agent ID and infection status
		agent_ID = agent.get_ID();
		infected = agent.infected();

		// If not a non-COVID hospital patient, 
		// register in the household or a retirement home
		if (agent.hospital_non_covid_patient() == false){
			if (agent.retirement_home_resident()){
				house_ID = agent.get_household_ID();
				RetirementHome& rh = retirement_homes.at(house_ID - 1); 
				rh.register_agent(agent_ID, infected);
			} else {
				house_ID = agent.get_household_ID();
				Household& house = households.at(house_ID - 1); 
				house.register_agent(agent_ID, infected);
			}
		}

		// Register in schools, workplaces, and hospitals 
		if (agent.student()){
			school_ID = agent.get_school_ID();
			School& school = schools.at(school_ID - 1); 
			school.register_agent(agent_ID, infected);		
		}

		if (agent.works()){
			work_ID = agent.get_work_ID();
			if (agent.retirement_home_employee()){
				RetirementHome& rh = retirement_homes.at(work_ID - 1);
				rh.register_agent(agent_ID, infected);
			} else if (agent.school_employee()){
				School& school = schools.at(work_ID - 1); 
				school.register_agent(agent_ID, infected);
			} else {
				Workplace& work = workplaces.at(work_ID - 1);
				work.register_agent(agent_ID, infected);
			}		
		}

		if (agent.hospital_employee() || 
				agent.hospital_non_covid_patient()){
			hospital_ID = agent.get_hospital_ID();
			Hospital& hospital = hospitals.at(hospital_ID - 1);
			hospital.register_agent(agent_ID, infected);	
		}
	} 
}

// Initial set-up of exposed agents
void ABM::initial_exposed(Agent& agent)
{
	bool never_sy = infection.recovering_exposed(agent.get_age());
	// Total latency period
	double latency = infection.latency();
	// Portion of latency when the agent is not infectious
	double dt_ninf = std::min(infection_parameters.at("time from exposed to infectiousness"), latency);
	if (never_sy){
		// Set to total latency + infectiousness duration
		double rec_time = infection_parameters.at("recovery time");
		agent.set_latency_duration(latency + rec_time);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}else{
		// If latency shorter, then  not infectious during the entire latency
		agent.set_latency_duration(latency);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}
	agent.set_inf_variability_factor(infection.inf_variability());
	agent.set_exposed(true);
	agent.set_recovering_exposed(never_sy);
}

// Vaccinate random members of the population that are not Flu or infected agents
void ABM::vaccinate_random()
{
	std::vector<int> can_be_vaccinated;
	// Select qualifying agents
	for (auto& agent : agents){
		if (!agent.symptomatic_non_covid() && !agent.infected()
				&& !agent.exposed() && !agent.symptomatic() 
				&& !agent.removed()){
			can_be_vaccinated.push_back(agent.get_ID());
		}
	}
	// Shuffle randomly
	infection.vector_shuffle(can_be_vaccinated);
	// Remove first n_vaccinated
	if (n_vaccinated > can_be_vaccinated.size()){
		throw std::runtime_error("Requested number of agents to vaccinate larger than number of available agents");
	}
	for (int i=0; i<n_vaccinated; ++i){
		agents.at(can_be_vaccinated.at(i)-1).set_vaccinated(true);
	}	
}

// Vaccinate specific group of agents in the population
void ABM::vaccinate_group()
{
	if (vaccine_group_name == "hospital employees") {
		implement_group_vaccination(&Agent::hospital_employee);
	} else if (vaccine_group_name == "school employees") {
		implement_group_vaccination(&Agent::school_employee);
	} else if (vaccine_group_name == "retirement home employees") {
		implement_group_vaccination(&Agent::retirement_home_employee);
	} else if (vaccine_group_name == "retirement home residents") {
		implement_group_vaccination(&Agent::retirement_home_resident);
	} else{
		throw std::invalid_argument("Wrong vaccination group type: " + vaccine_group_name);
	}		
}

// Assign removed (vaccinated equivalent) flag to the group with specified type
void ABM::implement_group_vaccination(type_getter atype)
{
	int v_final = 0;
	for (auto& agent : agents){
		if ((agent.*atype)() == true && !agent.infected()
				&& !agent.exposed() && !agent.symptomatic()
				&& !agent.removed()){
			agent.set_vaccinated(true);
			++v_final;			
		}
	}
	if (vac_verbose){
		std::cout << "Total number of vaccinated in the group " 
				  << vaccine_group_name << " " << v_final << std::endl;
	}
}

//
// Transmission of infection
//

// Transmit infection according to Infection model
void ABM::transmit_infection() 
{
	testing.check_switch_time(time);	
	check_events(schools, workplaces);
	compute_place_contributions();	
	compute_state_transitions();
	reset_contributions();
	advance_in_time();	
}

// Verify if anything that requires parameter changes happens at this step 
void ABM::check_events(std::vector<School>& schools, std::vector<Workplace>& workplaces)
{
	double tol = 1e-3;
	double new_tr_rate = 0.0;

	// Initialize agents with flu the time step the testing starts 
	// Optionally also vaccinate part of the population or/and specific groups
	if (equal_floats<double>(time, infection_parameters.at("start testing"), tol)){
		// Vaccinate
		if (random_vaccines == true){
			vaccinate_random();
		}
		if (group_vaccines == true){
			vaccinate_group();
		}
		// Initialize flu agents
		for (const auto& agent : agents){
			if (!agent.infected() && !agent.removed() && !agent.vaccinated()){
				// If not patient or hospital employee
				// Add to potential flu group
				if (!agent.hospital_employee() && !agent.hospital_non_covid_patient()){
					flu.add_susceptible_agent(agent.get_ID());
				}
			}
		}
		// Randomly assign portion of susceptible with flu
		// The set agents flags
		std::vector<int> flu_IDs = flu.generate_flu();
		for (const auto& ind : flu_IDs){
			Agent& agent = agents.at(ind-1);
			const int n_hospitals = hospitals.size();
			transitions.process_new_flu(agent, n_hospitals, time,
					   		 schools, workplaces, retirement_homes, 
							 infection, infection_parameters, flu, testing);
		}
	}

	// Closures
	if (equal_floats<double>(time, infection_parameters.at("school closure"), tol)){
		new_tr_rate = 0.0;
		for (auto& school : schools){
			school.change_transmission_rate(new_tr_rate);
			school.change_employee_transmission_rate(new_tr_rate);
		}
	}
	if (equal_floats<double>(time, infection_parameters.at("lockdown"), tol)){
		new_tr_rate = infection_parameters.at("workplace transmission rate")*infection_parameters.at("fraction of ld businesses");
		for (auto& workplace : workplaces){
			workplace.change_transmission_rate(new_tr_rate);
			workplace.change_absenteeism_correction(infection_parameters.at("lockdown absenteeism"));
		}
	}

	// Reopening
	if (equal_floats<double>(time, infection_parameters.at("reopening phase 1"), tol)){
		new_tr_rate = infection_parameters.at("workplace transmission rate") * 
				infection_parameters.at("fraction of phase 1 businesses");
		for (auto& workplace : workplaces){
			workplace.change_transmission_rate(new_tr_rate);
			workplace.change_absenteeism_correction(infection_parameters.at("lockdown absenteeism"));
		}
	}
	if (equal_floats<double>(time, infection_parameters.at("reopening phase 2"), tol)){
		new_tr_rate = infection_parameters.at("workplace transmission rate") * 
				infection_parameters.at("fraction of phase 2 businesses");
		for (auto& workplace : workplaces){
			workplace.change_transmission_rate(new_tr_rate);
			workplace.change_absenteeism_correction(infection_parameters.at("lockdown absenteeism"));
		}
	}
	if (equal_floats<double>(time, infection_parameters.at("reopening phase 3"), tol)){
		new_tr_rate = infection_parameters.at("workplace transmission rate") * 
				infection_parameters.at("fraction of phase 3 businesses");
		for (auto& workplace : workplaces){
			workplace.change_transmission_rate(new_tr_rate);
			workplace.change_absenteeism_correction(infection_parameters.at("lockdown absenteeism"));
		}
	}
}

// Count contributions of all infectious agents in each place
void ABM::compute_place_contributions()
{
	for (const auto& agent : agents){

		// Removed and vaccinated don't contribute
		if (agent.removed() == true || agent.vaccinated() == true){
			continue;
		}

		// If susceptible and being tested - add to hospital's
		// total number of people present at this time step
		if (agent.infected() == false){
			if ((agent.tested() == true) && 
				(agent.tested_in_hospital() == true) &&
				(agent.get_time_of_test() <= time) && 
		 		(agent.tested_awaiting_test() == true)){
					hospitals.at(agent.get_hospital_ID() - 1).increase_total_tested();
			}			
			continue;
		}

		// Consider all infectious cases, raise 
		// exception if no existing case
		if (agent.exposed() == true){
			contributions.compute_exposed_contributions(agent, time, households, 
							schools, workplaces, hospitals, retirement_homes);
		}else if (agent.symptomatic() == true){
			contributions.compute_symptomatic_contributions(agent, time, households, 
							schools, workplaces, hospitals, retirement_homes);
		}else{
			throw std::runtime_error("Agent does not have any state");
		}
	}
	contributions.total_place_contributions(households, schools, 
											workplaces, hospitals, retirement_homes);
}

// Determine infection propagation and
// state changes 
void ABM::compute_state_transitions()
{
	int newly_infected = 0, is_recovered = 0;
	// Infected state change flags: 
	// recovered - healthy, recovered - dead, tested at this step,
	// tested positive at this step, tested false negative
	std::vector<int> state_changes = {0, 0, 0, 0, 0};
	// Susceptible state changes
	// infected, tested, tested negative, tested false positive
	std::vector<int> s_state_changes = {0, 0, 0, 0};
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};

	// Store information for that day
	n_infected_day.push_back(0);
	tested_day.push_back(0);
	tested_pos_day.push_back(0);
	tested_neg_day.push_back(0);
	tested_false_pos_day.push_back(0);
	tested_false_neg_day.push_back(0);

	for (auto& agent : agents){

		// Skip the removed and the vaccinated 
		if (agent.removed() == true || agent.vaccinated() == true){
			continue;
		}
		
		std::fill(state_changes.begin(), state_changes.end(), 0);
		std::fill(s_state_changes.begin(), s_state_changes.end(), 0);

		if (agent.infected() == false){
			s_state_changes = transitions.susceptible_transitions(agent, time,
							dt, infection, households, schools, workplaces, 
							hospitals, retirement_homes, 
							infection_parameters, agents, flu, testing);
			n_infected_tot += s_state_changes.at(0);
			// True infected by timestep, from the first time step
			if (s_state_changes.at(0) == 1){
				++n_infected_day.back();
			}
		}else if (agent.exposed() == true){
			state_changes = transitions.exposed_transitions(agent, infection, time, dt, 
										households, schools, workplaces, hospitals,
										retirement_homes, infection_parameters, testing);
			n_recovering_exposed += state_changes.at(0);
			n_recovered_tot += state_changes.at(0);
		}else if (agent.symptomatic() == true){
			state_changes = transitions.symptomatic_transitions(agent, time, dt,
						infection, households, schools, workplaces, hospitals,
							retirement_homes, infection_parameters);
			n_recovered_tot += state_changes.at(0);
			// Collect only after a specified time
			if (time >= infection_parameters.at("time to start data collection")){
				if (state_changes.at(1) == 1){
					// Dead after testing
					++n_dead_tested;
					++n_dead_tot;
				} else if (state_changes.at(1) == 2){
					// Dead with no testing
					++n_dead_not_tested;
					++n_dead_tot;
				}
			}
		}else{
			throw std::runtime_error("Agent does not have any infection-related state");
		}

		// Recording testing changes for this agent
		if (time >= infection_parameters.at("time to start data collection")){
			if (agent.exposed() || agent.symptomatic()){
				if (state_changes.at(2) == 1){
					++tested_day.back();
					++tot_tested;
				}
				if (state_changes.at(3) == 1){
					++tested_pos_day.back();
					++tot_tested_pos;
				}
				if (state_changes.at(4) == 1){
					++tested_false_neg_day.back();
					++tot_tested_false_neg;
				}
			} else {
				// Susceptible
				if (s_state_changes.at(1) == 1){
					++tested_day.back();
					++tot_tested;
				}
				if (s_state_changes.at(2) == 1){
					++tested_neg_day.back();
					++tot_tested_neg;
				}
				if (s_state_changes.at(3) == 1){
					++tested_false_pos_day.back();
					++tot_tested_false_pos;
				}
			}
		}
	}
}

//
// Getters
//

// Retrieve number of infected agents at this time step
int ABM::get_num_infected() const
{
	int infected_count = 0;
	for (const auto& agent : agents){
		if (agent.infected())
			++infected_count;
	}
	return infected_count;
}

// Retrieve number of exposed agents at this time step
int ABM::get_num_exposed() const
{
	int exposed_count = 0;
	for (const auto& agent : agents){
		if (agent.exposed())
			++exposed_count;
	}
	return exposed_count;
}

// Number of infected - confirmed
int ABM::get_num_active_cases() const
{
	int active_count = 0;
	for (const auto& agent : agents){
		if ((agent.infected() && agent.tested_covid_positive())
			 || (agent.symptomatic_non_covid() && agent.home_isolated()
					 && agent.tested_false_positive())){
			++active_count;
		}
	}
	return active_count;
}

// Number of infected - confirmed
std::vector<int> ABM::get_treatment_data() const
{
	// IH, HN, ICU
	std::vector<int> treatments(3,0);
	for (const auto& agent : agents){
		if (agent.home_isolated()){
			++treatments.at(0);
		}else if (agent.hospitalized()){
			++treatments.at(1);
		}else if (agent.hospitalized_ICU()){
			++treatments.at(2);
		}
	}
	return treatments;
}

//
// I/O
//

// General function for reading an object from a file
std::vector<std::vector<std::string>> ABM::read_object(std::string fname)
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};

	// 2D vector with a parsed line in each inner
	// vector
	std::vector<std::vector<std::string>> obj_vec_2D;

	// Read and return a copy
	AbmIO abm_io(fname, delim, sflag, dims);
	obj_vec_2D = abm_io.read_vector<std::string>();

	return obj_vec_2D;
}

//
// Saving simulation state
//

// Save infection information
void ABM::print_infection_parameters(const std::string filename) const
{
	FileHandler file(filename, std::ios_base::out | std::ios_base::trunc);
	std::fstream &out = file.get_stream();	

	for (const auto& entry : infection_parameters){
		out << entry.first << " " << entry.second << "\n";
	}	
}

// Save age-dependent distributions
void ABM::print_age_dependent_distributions(const std::string filename) const
{
	FileHandler file(filename, std::ios_base::out | std::ios_base::trunc);
	std::fstream &out = file.get_stream();	

	for (const auto& entry : age_dependent_distributions){
		out << entry.first << "\n";
		for (const auto& e : entry.second)
			out << e.first << " " << e.second << "\n";
	}	
}


// Save current household information to file 
void ABM::print_households(const std::string fname) const
{
	print_places<Household>(households, fname);
}	

// Save current school information to file 
void ABM::print_schools(const std::string fname) const
{
	print_places<School>(schools, fname);
}


// Save current workplaces information to file 
void ABM::print_workplaces(const std::string fname) const
{
	print_places<Workplace>(workplaces, fname);
}

// Save current hospital information to file 
void ABM::print_hospitals(const std::string fname) const
{
	print_places<Hospital>(hospitals, fname);
}

// Save current retirement home information to file 
void ABM::print_retirement_home(const std::string fname) const
{
	print_places<RetirementHome>(retirement_homes, fname);
}

// Save IDs of all agents in all households
void ABM::print_agents_in_households(const std::string filename) const
{
	print_agents_in_places<Household>(households, filename);
}

// Save IDs of all agents in all schools
void ABM::print_agents_in_schools(const std::string filename) const
{
	print_agents_in_places<School>(schools, filename);
}

// Save IDs of all agents in all workplaces 
void ABM::print_agents_in_workplaces(const std::string filename) const
{
	print_agents_in_places<Workplace>(workplaces, filename);
}

// Save IDs of all agents in all hospitals 
void ABM::print_agents_in_hospitals(const std::string filename) const
{
	print_agents_in_places<Hospital>(hospitals, filename);
}

// Save current agent information to file 
void ABM::print_agents(const std::string fname) const
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};	

	// Write data to file
	AbmIO abm_io(fname, delim, sflag, dims);
	abm_io.write_vector<Agent>(agents);	
}

