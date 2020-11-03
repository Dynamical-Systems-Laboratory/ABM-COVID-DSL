#ifndef INFECTION_H
#define INFECTION_H

#include "common.h"
#include "rng.h"
#include "utils.h"
#include <tuple>

class RNG;

/***************************************************** 
 * class: Infection
 * 
 * Attributes and functionality of infection and its
 * transmission
 * 
 *****************************************************/

class Infection{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates an Infection object with default attributes
	 */
	Infection() = default;

	/**
	 * \brief Creates an Infection object with custom attributes
	 *
	 * @param del_t - time step 
	 */
	Infection(const double del_t) : dt(del_t) { }

	//
	// Infection transmission
	//
	
	/// \brief Compute if agent got infected
	/// @param lambda - probability factor
	bool infected(const double lambda);

	/// \brief Get latency period from a distribution
	double latency();

	/// \brief Get infection variability from a distribution
	double inf_variability();

	/// \brief Determine if the exposed agent will recover without symptoms
	/// @param age - agent's age
	bool recovering_exposed(const int age);

	/// \brief Determine if the agent will die based on agents age and total rate
	/// \brief Computes probability from total rate and probability of dying in ICU
	/// @param age - agents age
	/// @param is_hsp - true if an agent is a hospital employee or a former non-COVID-19 patient
	bool will_die_non_icu(const int age, const bool is_hsp = false);

	/// \brief Determine if the agent will be hospitalized 
	bool agent_hospitalized(const int age);

	/// \brief Determine if hospitalized in ICU
	bool agent_hospitalized_ICU(const int age);

	/// Determine if the agent will die in ICU
	bool will_die_ICU();
	
	/// \brief Returns randomly chosen time left for agent to live
	double time_to_death();

	/// \brief Determines if the agent will get tested based on probability prob
	bool will_be_tested(const double prob);
	/// \brief Determines if the agent will get tested in a hospital based on probability prob
	bool tested_in_hospital(const double prob);
	/// \brief True if the test is false negative 
	bool false_negative_test_result(const double);
	/// \brief True if the test is false positive 
	bool false_positive_test_result(const double);

	/// \brief Returns time from symptomatic to hospitalization
	double get_onset_to_hospitalization();
	/// \brief Returns time from hospitalization to death
	double get_hospitalization_to_death(); 

	/// \brief Returns random hospital ID 
	/// @param n_hsp - total number of hospitals
	int get_random_hospital_ID(const int n_hsp); 
	/// \brief Returns random house ID  
	/// @param n_hs - total number of households 
	int get_random_household_ID(const int n_hs);
	/// \brief Returns random agent ID
	int get_random_agent_ID(const int n_ag);

	/// \brief Period before the test, drawn from lognormal 
	/// @param t_max - maximum length of the period
	double wait_time_for_test(double t_max);

	/// Randomly shuffles a vector of ints
	void vector_shuffle(std::vector<int>& v) 
		{ rng.vector_shuffle(v); }
		
	//
	// Setters
	//

	void set_latency_distribution(const double mean, const double std)
		{ ln_mean_lat = mean; ln_std_lat = std; }

	void set_inf_variability_distribution(const double k, const double theta)
		{ inf_var_k = k; inf_var_theta = theta; }

	void set_onset_to_death_distribution(const double mean, const double std)
		{ otd_mean = mean; otd_std = std; }
	
	void set_onset_to_hospitalization_distribution(const double k, const double theta)
		{ oth_k = k; oth_theta = theta; }
	
	void set_hospitalization_to_death_distribution(const double k, const double theta)
		{ htd_k = k; htd_theta = theta; }

	/**
	 * \brief Assigning various single number probabilities
	 * @param pr_dth_icu - probability of death in ICU
	 */
	void set_other_probabilities(const double fr_sy_tested, const double pr_dth_icu,
								 const double fr_death_not_icu)
		{ prob_sy_tested = fr_sy_tested; prob_death_icu = pr_dth_icu; 
		  prob_death_not_admitted = fr_death_not_icu; }

	/**
	 * \brief Process and store the age-dependent exposed to symptomatic fractions
	 * @param raw_rates - map with string of age range (inclusive) to probability 
	 */
	void set_expN2sy_fractions(const std::map<std::string, double> raw_rates);

	/**
	 * \brief Process and store the age-dependent mortality rate distribution
	 * @param raw_rates - map with string of age range (inclusive) to probability 
	 */
	void set_mortality_rates(const std::map<std::string, double> raw_rates);

	/**
	 * \brief Process and store the age-dependent hospitalization fraction distribution
	 * @param raw_rates - map with string of age range (inclusive) to probability 
	 */
	void set_hospitalized_fractions(const std::map<std::string, double> raw_rates);

	/**
	 * \brief Process and store the age-dependent ICU hospitalization fraction distribution
	 * @param raw_rates - map with string of age range (inclusive) to probability 
	 */
	void set_hospitalized_ICU_fractions(const std::map<std::string, double> raw_rates);

	//
	// Getters
	//

	/// Return map with mortality rates
	const std::map<std::string, std::tuple<int, int, double>>& get_mortality_rates() const 
		{ return mortality_rates; }
	/// Return map with hospitalization rates
	const std::map<std::string, std::tuple<int, int, double>>& get_hospitalization_rates() const 
		{ return hospitalization_rates; }
	/// Return map with ICU rates
	const std::map<std::string, std::tuple<int, int, double>>& get_ICU_rates() const 
		{ return ICU_rates; }
	/// Return map with ICU rates
	const std::map<std::string, std::tuple<int, int, double>>& get_exp2nsy_fractions() const 
		{ return expN2sy_fractions; }

	//
	// I/O
	//

	/**
	 * \brief Print infection parameters
	 * \details The parameters are in the same order as in the constructor
     * It prints recovery time in days, not the mu_rec which is its' inverse
	 * @param where - output stream
	 */	
	void print_basic(std::ostream& where) const;

protected:
	
	//
	// Modeling parameters
	//

	// Time step
	double dt = 1.0;

	//
	// Distribution parameters
	//
	
	// Latency
	double ln_mean_lat = 0.0;
   	double ln_std_lat = 0.0;	

	// Infectiousness variability factor
	double inf_var_k = 0.0;
	double inf_var_theta = 0.0;

	// Onset to death log-normal
	double otd_mean = 0.0;
	double otd_std = 0.0;

	// Onset to hospitalization - gamma 
	double oth_k = 0.0;
	double oth_theta = 0.0;

	// Hospitalization to death - weibull
	double htd_k = 0.0;
	double htd_theta = 0.0;
	
	//
	// Other probabilities
	//

	// Probability of symptomatic agent 
	// getting tested
	double prob_sy_tested = 0.0;
	// Probability of dying in ICU
	double prob_death_icu = 0.0;
	// Probability of death when not admitted to ICU and needing one
	double prob_death_not_admitted = 0.0;

	// Random distribution generator
	RNG rng;
	
	//
	// Age-dependent distributions
	//
	
	// Fraction of exposed never developing symptoms in each age group
	std::map<std::string, std::tuple<int, int, double>> expN2sy_fractions;
	// Mortality rates (age group: min age, max age, probability)
	std::map<std::string, std::tuple<int, int, double>> mortality_rates;
	// Hospitalization rates (age group: min age, max age, probability)
	std::map<std::string, std::tuple<int, int, double>> hospitalization_rates;
	// ICU rates (age group: min age, max age, probability)
	std::map<std::string, std::tuple<int, int, double>> ICU_rates;

	//
	// Private functions
	//

	// Extract min and max age in a group from age-dependent distributions
	std::vector<int> parse_age_group(const std::string group_range);
};

/// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Infection& infection);

#endif 
