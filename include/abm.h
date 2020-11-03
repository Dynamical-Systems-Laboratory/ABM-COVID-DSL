#ifndef ABM_H
#define ABM_H

#include "abm_include.h"

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

class ABM{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates an ABM object with default attributes
	 */
	ABM() = default;

	/**
	 * \brief Creates an ABM object with custom attributes
	 * \details Reads the infection parameters from the provided 
	 * 				file and initializes Infection object;
	 * 				The map key represents a tag to recognize which dataset
	 * 				in question is it. Tags are currently: hospitalization, ICU, mortality
	 * 				All 3 are required. 
	 *
	 * @param del_t - time step, days
	 * @param infile - name of the file with the input parameters
	 * @param dist_files - map of keys-tags and file names where different distribution files are stored 
	 *
	 */
	ABM(double del_t, const std::string infile, const std::map<std::string, std::string> dist_files,
					const std::string tfile) : 
					dt(del_t), infection(del_t) 
		{
			time = 0.0;	
			load_infection_parameters(infile); 
			load_age_dependent_distributions(dist_files);
			load_testing(tfile);
			initialize_data_collection();
		}

	//
	// Initialization and object construction
	//

	/**
	 * \brief Create households based on information in a file
	 * \details Constructs households based on the ID and
	 * 				locations as defined in the file; One line
	 * 				in the file defines one household 
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_households(const std::string filename);

	/**
	 * \brief Create retirement homes based on information in a file
	 * \details Constructs retirement homes based on the ID and
	 * 				locations as defined in the file; One line
	 * 				in the file defines one household 
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_retirement_homes(const std::string filename);

	/**
	 * \brief Create schools based on information in a file
	 * \details Constructs schools based on the ID,
	 * 				locations, and school type as defined in the file; 
	 * 				One line in the file defines one school;
	 * 				School types are "daycare", "primary", "middle",
	 * 				"high", and "college"  
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_schools(const std::string filename);

	/**
	 * \brief Create workplaces based on information in a file
	 * \details Constructs workplaces based on the ID and
	 * 				locations as defined in the file; One line
	 * 				in the file defines one workplace 
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_workplaces(const std::string filename);

	/**
	 * \brief Create hospitals based on information in a file
	 * \details Constructs hospitals based on the ID and
	 * 				locations as defined in the file; One line
	 * 				in the file defines one hospital 
	 *	
	 * @param filename - path of the file with input information
	 * 
	 */	
	void create_hospitals(const std::string filename);

	/**
	 * \brief Create agents based on information in a file
	 * \details Constructs agents from demographic information
	 * 		in a file with agent per row, columns being the
	 *		information as it currently appears in the Agent
	 *		constructor; assigns agents to households, schools,
	 *		workplaces, and hospitals - needs to be called AFTER creating
	 *		those places; Optionally it can randomly choose initially
	 *		infected as specified by the input parameter;
	 *	
	 * @param filename - path of the file with input information
	 * @param ninf0 - number of initially infected - overwriting input file
	 * 
	 */	
	void create_agents(const std::string filename, const int ninf0 = 0);

	/// Set up vaccination of nv members of the random population members activated with testing
	void set_random_vaccination(int nv) 
		{ random_vaccines = true; n_vaccinated = nv;}
	/// Set up vaccination of specific population group activated with testing
	/// @param vaccine_group_name - recognizable name of the group (see manual)
	/// @param verbose - print the final number of vaccinated individuals
	void set_group_vaccination(std::string group_name, bool verbose = false) 
		{ group_vaccines = true; vaccine_group_name = group_name; vac_verbose = verbose; } 

	//
	// Transmission of infection
	//

	/**
	 * \brief Transmit infection according to Infection model
	 */
	void transmit_infection();

	/// \brief Count contributions of all infectious agents in each place 
	void compute_place_contributions();

	/// \brief Propagate infection and determine state transitions
	void compute_state_transitions();

	/// \brief Set the lambda factors to 0.0
	void reset_contributions()
		{ contributions.reset_sums(households, schools, workplaces, hospitals, retirement_homes); }

	// Increasing time
	void advance_in_time() { time += dt; }

	/// Verify if anything that requires parameter changes happens at this step 
	void check_events(std::vector<School>&, std::vector<Workplace>&);

	//
	// Getters
	//
	
	/// Retrieve number of infected agents at this time step
	int get_num_infected() const;
	/// Retrieve number of exposed
	int get_num_exposed() const;
	/// Number of infected - confirmed
	int get_num_active_cases() const;
	std::vector<int> get_treatment_data() const;
	/// Current simulation time
	double get_time() const { return time; }

	/// Retrieve number of total infected
	int get_total_infected() const { return n_infected_tot; }
	/// Retrieve number of total dead 
	int get_total_dead() const { return n_dead_tot; }
	/// Retrieve number of dead that were tested 
	int get_tested_dead() const { return n_dead_tested; }
	/// Retrieve number of dead that were not tested
	int get_not_tested_dead() const { return n_dead_not_tested; }
	/// Retrieve number of total recovered
	int get_total_recovered() const { return n_recovered_tot; }
	/// Retrieve cumulative number of exposed that never developed symptoms
	int get_tot_recovering_exposed() { return n_recovering_exposed; }

	// Total tested and confirmed COVID-19 + false positives 
	int get_total_tested() const { return tot_tested; }
	int get_total_tested_positive() const { return tot_tested_pos; }
	int get_total_tested_negative() const { return tot_tested_neg; }
	int get_total_tested_false_positive() const { return tot_tested_false_pos; }
	int get_total_tested_false_negative() const { return tot_tested_false_neg; }
	// Daily statistics
	const std::vector<int> get_infected_day() const { return n_infected_day; }
	const std::vector<int> get_dead_day() const { return n_dead_day; }
	const std::vector<int> get_recovered_day() const { return n_recovered_day; }
	const std::vector<int> get_tested_day() const { return tested_day; }
	const std::vector<int> get_tested_positive_day() const { return tested_pos_day; }	  
	const std::vector<int> get_tested_negative_day() const { return tested_neg_day; }
	const std::vector<int> get_tested_false_positive_day() const { return tested_false_pos_day; }
	const std::vector<int> get_tested_false_negative_day() const { return tested_false_neg_day; }

	//
	// Saving simulation state
	//

	/**
	 * \brief Save infection parameter information
	 *
     * @param filename - path of the file to print to
	 */
	void print_infection_parameters(const std::string filename) const;	

	/**
	 * \brief Save age-dependent distributions 
	 *
     * @param filename - path of the file to print to
	 */
	void print_age_dependent_distributions(const std::string filename) const;

	/**
	 * \brief Save current household information 
	 * \details Outputs household information as 
	 * 		household ID, x and y location, total number
	 * 		of agents, number of infected agents
	 * 		One line per household		
	 * @param filename - path of the file to print to
	*/
	void print_households(const std::string filename) const;	

	/**
	 * \brief Save current retirement home information 
	 * \details Outputs retirement home information as 
	 * 		school ID, x and y location, total number
	 * 		of agents, number of infected agents
	 * 		One line per institution		
	 * @param filename - path of the file to print to
	*/
	void print_retirement_home(const std::string filename) const;	

	/**
	 * \brief Save current school information 
	 * \details Outputs school information as 
	 * 		school ID, x and y location, total number
	 * 		of agents, number of infected agents
	 * 		One line per school		
	 * @param filename - path of the file to print to
	*/
	void print_schools(const std::string filename) const;	

	/**
	 * \brief Save current workplace information 
	 * \details Outputs workplace information as 
	 * 		workplace ID, x and y location, total number
	 * 		of agents, number of infected agents, ck, beta, psi
	 * 		One line per workplace		
	 * @param filename - path of the file to print to
	 */
	void print_workplaces(const std::string filename) const;	

	/**
	 * \brief Save current hospital information 
	 * \details Outputs hospital information as 
	 * 		hospital ID, x and y location, total number
	 * 		of agents, number of infected agents, ck, betas
	 * 		One line per hospital		
	 * @param filename - path of the file to print to
	 */
	void print_hospitals(const std::string filename) const;

	/**
	 * \brief Save IDs of all agents in all households
	 * \details One line per household, 0 if no agents present
	 * @param filename - path of the file to save to
	 */
	void print_agents_in_households(const std::string filename) const;
	
	/**
	 * \brief Save IDs of all agents in all retirement homes 
	 * \details One line per retirement home, 0 if no agents present
	 * @param filename - path of the file to save to
	 */
	void print_agents_in_retirement_homes(const std::string filename) const;
	
	/**
	 * \brief Save IDs of all agents in all schools
	 * \details One line per school, 0 if no agents present
	 * @param filename - path of the file to save to
	 */
	void print_agents_in_schools(const std::string filename) const;
	
	/**
	 * \brief Save IDs of all agents in all workplaces
	 * \details One line per workplace, 0 if no agents present
	 * @param filename - path of the file to save to
	 */
	void print_agents_in_workplaces(const std::string filename) const;

	/**
	 * \brief Save IDs of all agents in all hospitals 
	 * \details One line per hospital, 0 if no agents present
	 * @param filename - path of the file to save to
	 */
	void print_agents_in_hospitals(const std::string filename) const;

	/**
	 * \brief Save current agent information 
	 * \details Outputs agent information as 
	 *		indicated in Agent constructor 
	 * 		One line per agent		
	 * @param filename - path of the file to print to
	*/
	void print_agents(const std::string filename) const;	
	
	//
	// Functions mainly for testing
	//
	
	/// Return a const reference to a House object vector
	const std::vector<Household>& get_vector_of_households() const { return households; }
	/// Return a const reference to a RetirementHome object vector
	const std::vector<RetirementHome>& get_vector_of_retirement_homes() const { return retirement_homes; }
	/// Return a const reference to a School object vector
	const std::vector<School>& get_vector_of_schools() const { return schools; }
	/// Return a const reference to a Workplace object vector
	const std::vector<Workplace>& get_vector_of_workplaces() const { return workplaces; }
	/// Return a const reference to a Hospital object vector
	const std::vector<Hospital>& get_vector_of_hospitals() const { return hospitals; }
	/// Return a const reference to an Agent object vector
	const std::vector<Agent>& get_vector_of_agents() const { return agents; }
	/// Return a non-const reference to an Agent object vector
	std::vector<Agent>& vector_of_agents() { return agents; }
	/// Return a reference to a Hospital object vector
	std::vector<Hospital>& vector_of_hospitals() { return hospitals; }
	/// Return a reference to a Household object vector
	std::vector<Household>& vector_of_households() { return households; }
	/// Return a reference to a RetirementHome object vector
	std::vector<RetirementHome>& vector_of_retirement_homes() { return retirement_homes; }
	/// Return a reference to a School object vector
	std::vector<School>& vector_of_schools() { return schools; }
	/// Return a reference to a Workplace object vector
	std::vector<Workplace>& vector_of_workplaces() { return workplaces; }

	/// Return a reference to an Agent object vector
	std::vector<Agent>& get_vector_of_agents_non_const()  { return agents; }
	/// Return a copy of a House object vector
	std::vector<Household> get_copied_vector_of_households() const { return households; }
	/// Return a copy of a RetirementHome object vector
	std::vector<RetirementHome> get_copied_vector_of_retirement_homes() const { return retirement_homes; }
	/// Return a copy of a School object vector
	std::vector<School> get_copied_vector_of_schools() const { return schools; }
	/// Return a copy of a Workplace object vector
	std::vector<Workplace> get_copied_vector_of_workplaces() const { return workplaces; }
	/// Return a copy of a Hospital object vector
	std::vector<Hospital> get_copied_vector_of_hospitals() const { return hospitals; }
	/// Return a copy of Infection object
	Infection get_copied_infection_object() const { return infection; }
	/// Return a reference to an Infection object
	Infection& get_infection_object() { return infection; }
	/// Return a const reference to parameter map
	const std::map<std::string, double> get_infection_parameters() const
		{ return infection_parameters; }
	/// Return a copy of the Flu object
	Flu get_flu_object() const { return flu; }
	/// Return a reference to Flu object
	Flu& get_flu_object() { return flu; }
	/// Return a copy of the Testing object
	Testing get_testing_object() const { return testing; }
	/// Return the Testing object
	Testing& get_testing_object() { return testing; }
private:

	using type_getter = bool(Agent::*)() const;

	// General model attributes
	// Time step
	double dt = 1.0;
	// Time - updated contiuously throughout the simulation
	double time = 0.0;

	// Data collection
	// Total number of infected, dead and recovered
	int n_infected_tot = 0;
	int n_dead_tot = 0;
	int n_dead_tested = 0;
	int n_dead_not_tested = 0;
	int n_recovered_tot = 0;
	int n_recovering_exposed = 0;
	// Total tested
	int tot_tested = 0;
	int tot_tested_pos = 0;
	int tot_tested_neg = 0;
	int tot_tested_false_pos = 0;
	int tot_tested_false_neg = 0;
	// Daily new cases and new tested
	std::vector<int> n_infected_day = {};
	std::vector<int> n_dead_day = {};
	std::vector<int> n_recovered_day = {};
	std::vector<int> tested_day = {};
	std::vector<int> tested_pos_day = {};	
	std::vector<int> tested_neg_day = {};
	std::vector<int> tested_false_pos_day = {};
	std::vector<int> tested_false_neg_day = {};

	// Infection parameters
	std::map<std::string, double> infection_parameters = {};

	// Age-dependent distributions
	std::map<std::string, std::map<std::string, double>> age_dependent_distributions = {};

	// Infection properties and transmission model
	Infection infection;
	// Testing properties and their time dependence
	Testing testing;	
	// Class for computing infection contributions
	Contributions contributions;
	// Class for computing agent transitions
	Transitions transitions;
	// Class for setting agent state transitions
	StatesManager states_manager;
	// Class for creating and maintaining a population
	// with flu i.e.  non-covid symptomatic
	Flu flu;

	// Vectors of individual model objects
	std::vector<Agent> agents;
	std::vector<Household> households;
	std::vector<RetirementHome> retirement_homes;
	std::vector<School> schools;
	std::vector<Workplace> workplaces;
	std::vector<Hospital> hospitals;

	// Vaccination properties
	bool random_vaccines = false;
	int n_vaccinated = 0;
	bool group_vaccines = false;
	std::string vaccine_group_name;
	bool vac_verbose = false;

	// Private methods

	/// Set initial values on all the data collection variables and containers
	void initialize_data_collection();

	/// Load infection parameters, store in a map
	void load_infection_parameters(const std::string);

	/// Load age-dependent distributions as vectors stored in a map
	void load_age_dependent_distributions(const std::map<std::string, std::string>);

	/// Initialize testing and its time dependence
	void load_testing(const std::string);

	/**
	 * \brief Read object information from a file	
	 * @param filename - path of the file to print to
	 */
	std::vector<std::vector<std::string>> read_object(std::string filename);

	/// \brief Set properties of initially infected - exposed
	void initial_exposed(Agent&);

	/// Vaccinate random members of the population that are not Flu or infected agents
	void vaccinate_random();
	/// Vaccinate specific group of agents in the population
	void vaccinate_group();
	/// Assign removed (vaccinated equivalent) flag to the group with specified type
	/// @param atype - agent member function that checks if the agent is of requested type 
	void implement_group_vaccination(type_getter atype);

	/**
	 * \brief Print basic places information to a file
	 */
	template <typename T>
	void print_places(std::vector<T> places, const std::string fname) const;

	/// \brief Print all agent IDs in a particular type of place to a file
	template <typename T>
	void print_agents_in_places(std::vector<T> places, const std::string fname) const;

	/**
	 * \brief Retrieve information about agents from a file and store all in a vector
	 * \details Optional parameter overwrites the loaded initially infected with custom
	 */
	void load_agents(const std::string fname, const int ninf0 = 0);

	/**
	 * \brief Assign agents to households, schools, and worplaces
	 */
	void register_agents();
};

// Write Place objects
template <typename T>
void ABM::print_places(std::vector<T> places, const std::string fname) const
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};	

	// Write data to file
	AbmIO abm_io(fname, delim, sflag, dims);
	abm_io.write_vector<T>(places);
}

// Write agent IDs in Place objects
template <typename T>
void ABM::print_agents_in_places(std::vector<T> places, const std::string fname) const
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};	

	// First collect the data into a nested vector
	std::vector<std::vector<int>> agents_all_places;
	for (const auto& place : places){
		std::vector<int> agent_IDs = place.get_agent_IDs();
		// If no agents, store a 0
		if (agent_IDs.empty())
			agents_all_places.push_back({0});
		else
			agents_all_places.push_back(agent_IDs);
	}

	// Then write data to file, one line per place
	AbmIO abm_io(fname, delim, sflag, dims);
	abm_io.write_vector<int>(agents_all_places);
}

#endif
