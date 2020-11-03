#include "../include/infection.h"

/***************************************************** 
 * class: Infection
 * 
 * Attributes and functionality of infection and its
 * transmission
 * 
 *****************************************************/

//
// I/O
//

// Print infection parameters	
void Infection::print_basic(std::ostream& where) const
{
	where << dt << " " << ln_mean_lat << " " << ln_std_lat
		  << " " << inf_var_k << " " << inf_var_theta << " "
		  << otd_mean << " " << otd_std << " " << oth_k << " "
		  << oth_theta << " " << htd_k << " " << htd_theta << " "
		  << prob_sy_tested << " " << prob_death_icu << " " 
		  << prob_death_not_admitted; 
}

//
// Infection transmission
//

// Get latency period from a distribution
double Infection::latency()
{
	return rng.get_random_lognormal(ln_mean_lat, ln_std_lat);
}

// Get infection variability from a distribution
double Infection::inf_variability()
{
	return rng.get_random_gamma(inf_var_k, inf_var_theta); 
}

// Determine if exposed will go directly to recovered
bool Infection::recovering_exposed(const int age)
{
	// Probability of recovery without symptoms
	double prob = 0;
	for (const auto& pxp : expN2sy_fractions){
		if ( age >= std::get<0>(pxp.second) &&
			 age <= std::get<1>(pxp.second)){
			prob = std::get<2>(pxp.second);
		}
	}
	if (rng.get_random(0.0, 1.0) <= prob)
		return true;
	else
		return false;
}

// Determines if the agent will get tested based on probability prob
bool Infection::will_be_tested(const double prob)
{
	return (rng.get_random(0, 1) <= prob);
}

// Determines if the agent will get tested in a hospital based on probability prob
bool Infection::tested_in_hospital(const double prob)
{
	return (rng.get_random(0, 1) <= prob);
}

// True if the test is false negative 
bool Infection::false_negative_test_result(const double prob)
{
	return (rng.get_random(0, 1) <= prob);
}

// True if the test is false positive 
bool Infection::false_positive_test_result(const double prob)
{
	return (rng.get_random(0, 1) <= prob);
}

// Determine if the agent will be hospitalized
bool Infection::agent_hospitalized(const int age)
{
	// Probability of hospitalization 
	double prob = 0;
	for (const auto& hsp : hospitalization_rates){
		if ( age >= std::get<0>(hsp.second) &&
			 age <= std::get<1>(hsp.second)){
			prob = std::get<2>(hsp.second);
		}
	}

	// true if going to be hospitalized 
	if (rng.get_random(0.0, 1.0) <= prob)
		return true;
	else
		return false;	
}

// Determine if the agent will be hospitalized in ICU
bool Infection::agent_hospitalized_ICU(const int age)
{
	// Probability of hospitalization in ICU
	double prob = 0;
	for (const auto& icu : ICU_rates){
		if ( age >= std::get<0>(icu.second) &&
			 age <= std::get<1>(icu.second)){
			prob = std::get<2>(icu.second);
		}
	}

	// true if going to be hospitalized in ICU
	if (rng.get_random(0.0, 1.0) <= prob)
		return true;
	else
		return false;	
}

// Determine if agent will die 
bool Infection::will_die_non_icu(const int age, const bool is_hsp)
{
	double tot_prob = 0.0, non_icu_prob =0.0;
	double prob_hsp = 0.0, prob_hsp_icu = 0.0;
	double prob_need_icu = 0.0, prob_die_need_icu = 0.0;
	double exp_never_sy_age = 0.0;

	// Probability exposed never symptomatic
	for (const auto& pxp : expN2sy_fractions){
		if ( age >= std::get<0>(pxp.second) &&
			 age <= std::get<1>(pxp.second)){
			exp_never_sy_age = std::get<2>(pxp.second);
		}
	}

	// Probability of death (corrected IFR) 
	for (const auto& mrt : mortality_rates){
		if ( age >= std::get<0>(mrt.second) &&
			 age <= std::get<1>(mrt.second)){
			tot_prob = std::get<2>(mrt.second)/(1-exp_never_sy_age);
		}
	}
	// Probability of hospitalization
	for (const auto& hsp : hospitalization_rates){
		if ( age >= std::get<0>(hsp.second) &&
			 age <= std::get<1>(hsp.second)){
			prob_hsp = std::get<2>(hsp.second);
		}
	}
	// Probability of hospitalization in ICU
	for (const auto& icu : ICU_rates){
		if ( age >= std::get<0>(icu.second) &&
			 age <= std::get<1>(icu.second)){
			prob_hsp_icu = std::get<2>(icu.second);
		}
	}

	prob_need_icu = prob_hsp*prob_hsp_icu;
	
	if (is_hsp == false){
		prob_die_need_icu = prob_death_icu*prob_sy_tested + prob_death_not_admitted*(1-prob_sy_tested);
	} else {
		prob_die_need_icu = prob_death_icu;
	}

	if (equal_floats<double>(prob_need_icu, 1.0, 1e-5)){
		non_icu_prob = 0.0;
	}else{
		non_icu_prob = (tot_prob - prob_die_need_icu*prob_need_icu)/(1-prob_need_icu); 
	}

	// true if going to die
	if (rng.get_random(0.0, 1.0) <= non_icu_prob){
		return true;
	}else{
		return false;
	}	
}

// Determine if the agent will die in ICU
bool Infection::will_die_ICU()
{
	return rng.get_random(0, 1) <= prob_death_icu;
}

// Determine time to death
double Infection::time_to_death()
{
	return rng.get_random_lognormal(otd_mean, otd_std);
}

// Returns time from symptomatic to hospitalization
double Infection::get_onset_to_hospitalization()
{
	return rng.get_random_gamma(oth_k, oth_theta);
}

// Returns time from hospitalization to death
double Infection::get_hospitalization_to_death()
{
	return rng.get_random_weibull(htd_k, htd_theta);
}

// Returns random hospital ID for testing
int Infection::get_random_hospital_ID(const int n_hsp)
{
	return rng.get_random_int(1, n_hsp);
}

// Period before the test, drawn from gamma 
double Infection::wait_time_for_test(double t0)
{
	double k = 25.4621, theta = 1.4301;
	return std::max(1.0, (t0 + rng.get_random_gamma(k, theta)));
}

// Returns random household ID for testing
int Infection::get_random_household_ID(const int n_hs)
{
	return rng.get_random_int(1, n_hs);
}

// Returns random household ID for testing
int Infection::get_random_agent_ID(const int n_ag)
{
	return rng.get_random_int(1, n_ag);
}

//
// Setters
//

// Process and store the age-dependent exposed to symptomatic fractions 
void Infection::set_expN2sy_fractions(const std::map<std::string, double> raw_rates)
{
	std::vector<int> ages = {0,0};
	expN2sy_fractions.clear();
	for (const auto& rr : raw_rates){
		ages = parse_age_group(rr.first);
		expN2sy_fractions[rr.first] = std::make_tuple(ages[0], ages[1], rr.second);
	}
}

// Process and store the age-dependent mortality rate distribution
void Infection::set_mortality_rates(const std::map<std::string, double> raw_rates)
{
	std::vector<int> ages = {0,0};
	mortality_rates.clear();
	for (const auto& rr : raw_rates){
		ages = parse_age_group(rr.first);
		mortality_rates[rr.first] = std::make_tuple(ages[0], ages[1], rr.second);
	}
}

// Process and store the age-dependent hospitalization fraction distribution
void Infection::set_hospitalized_fractions(const std::map<std::string, double> raw_rates)
{
	std::vector<int> ages = {0,0};
	hospitalization_rates.clear();
	for (const auto& rr : raw_rates){
		ages = parse_age_group(rr.first);
		hospitalization_rates[rr.first] = std::make_tuple(ages[0], ages[1], rr.second);
	}
}

// Process and store the age-dependent ICU hospitalization fraction distribution
void Infection::set_hospitalized_ICU_fractions(const std::map<std::string, double> raw_rates)
{
	std::vector<int> ages = {0,0};
	ICU_rates.clear();
	for (const auto& rr : raw_rates){
		ages = parse_age_group(rr.first);
		ICU_rates[rr.first] = std::make_tuple(ages[0], ages[1], rr.second);
	}
}

//
// Private functions
//

// Extract min and max age in a group from age-dependent distributions
std::vector<int> Infection::parse_age_group(const std::string group_range)
{
	int age = 0;
	std::vector<int> ages;

	std::istringstream group(group_range);
	std::string token;
	
	// Assumes only two numbers, first being min age
	while(std::getline(group, token, '-')){
		std::istringstream(token) >> age;
   		ages.push_back(age);
	}
	return ages;
}

// Compute if agent got infected
bool Infection::infected(const double lambda)
{
	// Probability of infection
	double prob = 1 - std::exp(-dt*lambda);

	// true if infected
	if (rng.get_random(0.0, 1.0) <= prob)
		return true;
	else
		return false; 
}

//
// Supporting functions
//

// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Infection& infection)
{
	infection.print_basic(out);
	return out;
}
