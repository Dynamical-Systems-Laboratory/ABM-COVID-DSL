#include "infection_tests.h"

/***************************************************** 
 *
 * Test suite for functionality of the Infection class
 *
 *****************************************************/

// Aliases for pointers to member functions used in these
// tests
using dist_sampling = double (Infection::*)();
using age_rates_setter = void (Infection::*)(const std::map<std::string, double>);
using age_rates_getter = const std::map<std::string, std::tuple<int, int, double>>& (Infection::*)() const;
using age_rates_caller = bool (Infection::*)(int, bool);
using age_rates_caller_no_bool = bool (Infection::*)(int);

// Tests
bool infection_transmission_test();
bool infection_out_test();
bool infection_misc_test();

// Supporting functions
bool check_mortality_rates(Infection&);
bool check_hospitalization_rates(Infection&);
bool check_ICU_rates(Infection&);
bool check_age_dependent_rates(age_rates_setter, age_rates_getter, age_rates_caller,
								Infection&,	std::map<std::string, double>, 
								std::map<std::string, std::tuple<int, int, double>>);
bool check_age_dependent_rates(age_rates_setter, age_rates_getter, age_rates_caller_no_bool,
								Infection&,	std::map<std::string, double>, 
								std::map<std::string, std::tuple<int, int, double>>);
bool check_exposed_never_sy(Infection&);
bool check_non_icu_mortality(Infection&);
bool check_distribution(dist_sampling, Infection&, double);
bool check_simple_distributions(Infection&, std::vector<double>);
bool check_random_ID(Infection&);
bool check_testing_wait_distribution(Infection&);

int main()
{
	test_pass(infection_transmission_test(), "Infection class transmission functionality");
	test_pass(infection_out_test(), "Infection class ostream operator");
	test_pass(infection_misc_test(), "Infection class misc functionality");
}

/// Tests functionality related to infection transmission
bool infection_transmission_test()
{
	double delta_t = 1.5;
	double lat_mean = 5.0, lat_std = 0.1; 
	double rho_k = 0.2, rho_theta = 1.2;
	double otd_mu = 2.6696, otd_sigma = 0.4760;
	double oth_k = 1.2, oth_theta = 0.1;
	double ohd_k = 5.0, ohd_theta = 1.5;
	
	// Expected mean values of distributions
	double mean_latency = 149.1571, mean_var = 0.24; 
	double mean_otd = 16.1652;
	double mean_oth = 0.12;
   	double mean_ohd = 1.3773;	

	Infection infection(delta_t);
	
	infection.set_latency_distribution(lat_mean, lat_std);
	infection.set_inf_variability_distribution(rho_k, rho_theta);
	infection.set_onset_to_death_distribution(otd_mu, otd_sigma);
	infection.set_onset_to_hospitalization_distribution(oth_k, oth_theta);
	infection.set_hospitalization_to_death_distribution(ohd_k, ohd_theta);

	// Infection
	// Should be infected
	if (infection.infected(1e16) == false){
		std::cerr << "Infection computation: should be infected" << std::endl;
		return false;
	}
	// Should not be infected
	if (infection.infected(1e-16) == true){
		std::cerr << "Infection computation: should not be infected" << std::endl;
		return false;
	}

	// Other probabilities:
	// Arguments:
	// prob_sy_tested, prob_death_icu, 
	// prob_death_not_admitted
	//
	// Exposed recovering without symptoms
	// and agent not dying in ICU
	// First and last arguments shouldn't matter here
	infection.set_other_probabilities(1.0, 0.0, 1.0);
	if (infection.will_die_ICU() == true){
		std::cerr << "Should not die in ICU" << std::endl;
		return false;
	}
	// Becoming symptomatic and dying in ICU
	infection.set_other_probabilities(0.0, 1.0, 1.0);
	if (infection.will_die_ICU() == false){
		std::cerr << "Should die in ICU" << std::endl;
		return false;
	}

	if (!check_mortality_rates(infection)){
		std::cerr << "Issue with mortality rates" << std::endl;
		return false;
	}
	if (!check_hospitalization_rates(infection)){
		std::cerr << "Issue with hospitalization rates" << std::endl;
		return false;
	}
	if (!check_ICU_rates(infection)){
		std::cerr << "Issue with ICU rates" << std::endl;
		return false;
	}
	if (!check_exposed_never_sy(infection)){
		std::cerr << "Issue with exposed never symptomatic" << std::endl;
		return false;
	}
	if (!check_non_icu_mortality(infection)){
		std::cerr << "Issue with non-ICU mortality" << std::endl;
		return false;
	}

	// Check all single number distributions
	std::vector<double> dist_probs = {0.25, 0.9, 0.05, 0.1};
	if (!check_simple_distributions(infection, dist_probs)){
		std::cerr << "Issue with simple distributions" << std::endl;
		return false;
	}	

	// Check generation of random place ID functions
	if (!check_random_ID(infection)){
		std::cerr << "Issue generating random place or agent ID" << std::endl;
		return false;	
	}

	// Check distributions
	if (!check_distribution(&Infection::latency, infection, mean_latency)){
		std::cerr << "Error in latency distribution" << std::endl;
		return false;		
	}
	if (!check_distribution(&Infection::inf_variability, infection, mean_var)){
		std::cerr << "Error in distribution of infection variability" << std::endl;
		return false;		
	}
	if (!check_distribution(&Infection::time_to_death, infection, mean_otd)){
		std::cerr << "Error in distribution of time to death" << std::endl;
		return false;		
	}
	if (!check_distribution(&Infection::get_onset_to_hospitalization, infection, mean_oth)){
		std::cerr << "Error in distribution of time from symptom onset to hospitalization" << std::endl;
		return false;		
	}
	if (!check_distribution(&Infection::get_hospitalization_to_death, infection, mean_ohd)){
		std::cerr << "Error in distribution of time from hospitalization to death" << std::endl;
		return false;		
	}

	// Check wait time for test
	if (!check_testing_wait_distribution(infection)){
		std::cerr << "Error in testing wait time distribution" << std::endl;
		return false;		
	}

	return true;
}

/// Tests Infection ostream operator overload/print capabilities
bool infection_out_test()
{
	// Set up all necessary members
	double delta_t = 1.5;
	double lat_mean = 5.0, lat_std = 0.1; 
	double rho_k = 0.2, rho_theta = 1.2;
	double otd_mu = 2.6696, otd_sigma = 0.4760;
	double oth_k = 1.2, oth_theta = 0.1;
	double ohd_k = 5.0, ohd_theta = 1.5;
	double prob_sy_tested = 0.81, prob_death_icu = 0.45;
  	double prob_death_not_admitted = 0.98;	

	Infection infection(delta_t);
	
	infection.set_latency_distribution(lat_mean, lat_std);
	infection.set_inf_variability_distribution(rho_k, rho_theta);
	infection.set_onset_to_death_distribution(otd_mu, otd_sigma);
	infection.set_onset_to_hospitalization_distribution(oth_k, oth_theta);
	infection.set_hospitalization_to_death_distribution(ohd_k, ohd_theta);
	infection.set_other_probabilities(prob_sy_tested, prob_death_icu, prob_death_not_admitted);

	// Get the data as printed by an Infection object 
	std::stringstream inf_buff;
	inf_buff << infection;
	std::istringstream res(inf_buff.str());

	double test_dt = 0.0, test_lat_mean = 0.0, test_lat_std = 0.0;
   	double test_rho_k = 0.0, test_rho_theta = 0.0, test_otd_mu = 0.0;
	double test_otd_sigma = 0.0, test_oth_k = 0.0, test_oth_theta = 0.0;
	double test_ohd_k = 0.0, test_ohd_theta = 0.0;
	double test_prob_sy_tested = 0.0, test_prob_death_icu = 0.0; 
	double test_prob_death_not_admitted = 0.0;

	res >> test_dt >> test_lat_mean >> test_lat_std >>
		   test_rho_k >> test_rho_theta >> test_otd_mu >>
		   test_otd_sigma >> test_oth_k >> test_oth_theta >>
		   test_ohd_k >> test_ohd_theta >>
		   test_prob_sy_tested >> test_prob_death_icu >> 
		   test_prob_death_not_admitted;

	if (!float_equality<double>(delta_t, test_dt, 1e-5)){
		std::cerr << "Expected time step different than one from the output" << std::endl;
		return false;
	}

	if (!float_equality<double>(lat_mean, test_lat_mean, 1e-5)
			|| !float_equality<double>(lat_std, test_lat_std, 1e-5)){
		std::cerr << "Expected latency distribution parameters different than one from the output" << std::endl;
		return false;
	}

	if (!float_equality<double>(rho_k, test_rho_k, 1e-5)
			|| !float_equality<double>(rho_theta, test_rho_theta, 1e-5)){		
		std::cerr << "Expected infection variability distribution parameters different than one from the output" << std::endl;
		return false;
	}
	
	if (!float_equality<double>(otd_mu, test_otd_mu, 1e-5)
			|| !float_equality<double>(otd_sigma, test_otd_sigma, 1e-5)){		
		std::cerr << "Expected symptoms onset to death distribution parameters different than one from the output" << std::endl;
		return false;
	}

	if (!float_equality<double>(oth_k, test_oth_k, 1e-5)
			|| !float_equality<double>(oth_theta, test_oth_theta, 1e-5)){		
		std::cerr << "Expected symptoms onset to hospitalization distribution parameters different than one from the output" << std::endl;
		return false;
	}

	if (!float_equality<double>(ohd_k, test_ohd_k, 1e-5)
			|| !float_equality<double>(ohd_theta, test_ohd_theta, 1e-5)){		
		std::cerr << "Expected hospitalization to death distribution parameters different than one from the output" << std::endl;
		return false;
	}

	if (!float_equality<double>(prob_sy_tested, test_prob_sy_tested, 1e-5)
			|| !float_equality<double>(prob_death_icu, test_prob_death_icu, 1e-5)
			|| !float_equality<double>(prob_death_not_admitted, test_prob_death_not_admitted, 1e-5)){		
		std::cerr << "Expect other probability values different than one from the output" << std::endl;
		return false;
	}

	return true;
}

/// Tests for various unclassified functionality
bool infection_misc_test()
{
	double delta_t = 1.5;
	Infection infection(delta_t);

	// Random shuffle test
	std::vector<int> v2s = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> v_orig(v2s);
	infection.vector_shuffle(v2s);
	return !(v2s == v_orig);
}

/// Test if generated mortality rates correspond to actual
bool check_mortality_rates(Infection& infection)
{
	// Note - this works because hospitalization and ICU probabilities 
	// are still set 0.0
	// Input
	std::map<std::string, double> raw_mortality_rates = {{"0-9", 0.00002},  
			{"10-19", 0.00006}, {"20-29", 0.0003}, {"30-39", 0.0008},
			{"40-49", 0.0015}, {"50-59", 0.006}, {"60-69", 0.022},
			{"70-79", 0.051}, {"80-120", 0.093}};	

	// Expected output
	std::map<std::string, std::tuple<int, int, double>> mortality_rates = 
					   {{"0-9", std::make_tuple(0, 9, 0.00002)},
						{ "10-19", std::make_tuple(10, 19, 0.00006)},
						{ "20-29", std::make_tuple(20, 29, 0.0003)},
						{ "30-39", std::make_tuple(30, 39, 0.0008)},
						{ "40-49", std::make_tuple(40, 49, 0.0015)},
						{ "50-59", std::make_tuple(50, 59, 0.006)},
						{ "60-69", std::make_tuple(60, 69, 0.022)},
						{ "70-79", std::make_tuple(70, 79, 0.051)},
						{ "80-120", std::make_tuple(80, 120, 0.093)}};

	// Verification 
	return check_age_dependent_rates(&Infection::set_mortality_rates, &Infection::get_mortality_rates,
					&Infection::will_die_non_icu, infection, raw_mortality_rates, mortality_rates);
}

/// Test if generated hospitalization rates correspond to actual
bool check_hospitalization_rates(Infection& infection)
{
	// Input
	std::map<std::string, double> raw_rates = {{"0-9", 0.001},  
			{"10-19", 0.003}, {"20-29", 0.012}, {"30-39", 0.032},
			{"40-49", 0.049}, {"50-59", 0.102}, {"60-69", 0.166},
			{"70-79", 0.243}, {"80-120", 0.273}};	

	// Expected output
	std::map<std::string, std::tuple<int, int, double>> hsp_rates = 
					   {{"0-9", std::make_tuple(0, 9, 0.001)},
						{ "10-19", std::make_tuple(10, 19, 0.003)},
						{ "20-29", std::make_tuple(20, 29, 0.012)},
						{ "30-39", std::make_tuple(30, 39, 0.032)},
						{ "40-49", std::make_tuple(40, 49, 0.049)},
						{ "50-59", std::make_tuple(50, 59, 0.102)},
						{ "60-69", std::make_tuple(60, 69, 0.166)},
						{ "70-79", std::make_tuple(70, 79, 0.243)},
						{ "80-120", std::make_tuple(80, 120, 0.273)}};

	// Verification 
	return check_age_dependent_rates(&Infection::set_hospitalized_fractions, 
									 &Infection::get_hospitalization_rates,
									 &Infection::agent_hospitalized,
									 infection, raw_rates, hsp_rates);
}

/// Test if generated ICU rates correspond to actual
bool check_ICU_rates(Infection& infection)
{
	// Input
	std::map<std::string, double> raw_rates = {{"0-9", 0.05},  
			{"10-19", 0.05}, {"20-29", 0.05}, {"30-39", 0.05},
			{"40-49", 0.063}, {"50-59", 0.122}, {"60-69", 0.274},
			{"70-79", 0.432}, {"80-120", 0.709}};	

	// Expected output
	std::map<std::string, std::tuple<int, int, double>> hsp_rates = 
					   {{"0-9", std::make_tuple(0, 9, 0.05)},
						{ "10-19", std::make_tuple(10, 19, 0.05)},
						{ "20-29", std::make_tuple(20, 29, 0.05)},
						{ "30-39", std::make_tuple(30, 39, 0.05)},
						{ "40-49", std::make_tuple(40, 49, 0.063)},
						{ "50-59", std::make_tuple(50, 59, 0.122)},
						{ "60-69", std::make_tuple(60, 69, 0.274)},
						{ "70-79", std::make_tuple(70, 79, 0.432)},
						{ "80-120", std::make_tuple(80, 120, 0.709)}};

	// Verification 
	return check_age_dependent_rates(&Infection::set_hospitalized_ICU_fractions, 
									 &Infection::get_ICU_rates,
									 &Infection::agent_hospitalized_ICU,
									 infection, raw_rates, hsp_rates);
}

/// Test if implementation of age-dependent exposed never symptomatic probabiity is correct
bool check_exposed_never_sy(Infection& infection)
{
	// Input
	std::map<std::string, double> raw_rates = {{"0-12", 0.9},  
			{"13-16", 0.7}, {"17-120", 0.5}};	

	// Expected output
	std::map<std::string, std::tuple<int, int, double>> esy_rates = 
					   {{"0-12", std::make_tuple(0, 12, 0.9)},
						{ "13-16", std::make_tuple(13, 16, 0.7)},
						{ "17-120", std::make_tuple(17, 120, 0.5)}};

	// Verification 
	return check_age_dependent_rates(&Infection::set_expN2sy_fractions, 
									 &Infection::get_exp2nsy_fractions,
									 &Infection::recovering_exposed,
									 infection, raw_rates, esy_rates);
}

/// Tests for mortality
bool check_non_icu_mortality(Infection& infection)
{
	// Fixed probabilities
	double prob_sy_tested = 0.81, prob_death_icu = 0.45;
  	double prob_death_not_admitted = 0.98;	
	infection.set_other_probabilities(prob_sy_tested, prob_death_icu, prob_death_not_admitted);

	// Age-dependent rates
	std::map<std::string, double> hsp_rates = {{"30-40", 0.3},  
			{"60-63", 0.7}, {"90-100", 0.98}};
	std::map<std::string, double> icu_rates = {{"30-40", 0.2},  
			{"60-63", 0.6}, {"90-100", 0.95}};
	std::map<std::string, double> mortality_rates = {{"30-40", 0.1},  
			{"60-63", 0.3}, {"90-100", 0.8}};
	std::map<std::string, double> esy = {{"30-40", 0.5},  
			{"60-63", 0.3}, {"90-100", 0.01}};
	infection.set_hospitalized_fractions(hsp_rates);
	infection.set_hospitalized_ICU_fractions(icu_rates);
	infection.set_mortality_rates(mortality_rates);
	infection.set_expN2sy_fractions(esy);

	// Test 1 - matching expected rates - not hospital employees or former patients
	std::map<std::string, double> exp_rates = {{"30-40", 0.178},  
			{"60-63", 0.341}, {"90-100", 1.0}};
	// Lower bound, middle value, and an upper bound
	std::vector<int> ages = {30, 61, 100};
	int age_ij = 0;
	int n_tot = 1e6;
	for (const auto& rate : exp_rates){
		int n_affected = 0;
		int age = ages.at(age_ij);
		for (int i=0; i<n_tot; ++i){
			if (infection.will_die_non_icu(age) == true){
				n_affected++;
			}	
		}
		double fr_affected = (static_cast<double>(n_affected))/(static_cast<double>(n_tot));
		if (!float_equality<double>(fr_affected, rate.second, 0.01)){
			std::cerr << "Age-dependent rates: computed mortality rate outside ICU for age " << age 
					  << " not equal expected " << fr_affected << std::endl;
			return false;
		}
		++age_ij;		
	}

	// Test 2 - matching expected rates - hospital employees or former patients
	bool hsp = true;
	std::map<std::string, double> exp_rates_hsp = {{"30-40", 0.18404},  
			{"60-63", 0.4131}, {"90-100", 1.0}};
	age_ij = 0;
	// Lower bound, middle value, and an upper bound
	for (const auto& rate : exp_rates_hsp){
		int n_affected = 0;
		int age = ages.at(age_ij);
		for (int i=0; i<n_tot; ++i){
			if (infection.will_die_non_icu(age, hsp) == true){
				n_affected++;
			}	
		}
		double fr_affected = (static_cast<double>(n_affected))/(static_cast<double>(n_tot));
		if (!float_equality<double>(fr_affected, rate.second, 0.01)){
			std::cerr << "Age-dependent rates: computed mortality rate outside ICU for age " << age 
					  << " not equal expected " << fr_affected << std::endl;
			return false;
		}
		++age_ij;		
	}

	// Test 3 - edge cases
	std::map<std::string, double> no_mortality = {{"30-40", 0.0},  
			{"60-63", 0.0}, {"90-100", 0.0}};
	infection.set_mortality_rates(no_mortality);
	for (const auto& age : ages){	
		if (infection.will_die_non_icu(age) == true){
			std::cerr << "Should not die if not needing ICU" << std::endl;
			return false;		
		}	
	}
	
	std::map<std::string, double> all_mortality = {{"30-40", 1.0},  
			{"60-63", 1.0}, {"90-100", 1.0}};
	std::map<std::string, double> no_icu = {{"30-40", 0.0},  
			{"60-63", 0.0}, {"90-100", 0.0}};
	infection.set_mortality_rates(all_mortality);
	infection.set_hospitalized_ICU_fractions(no_icu);
	for (const auto& age : ages){	
		if (infection.will_die_non_icu(age) == false){
			std::cerr << "Should die if not needing ICU" << std::endl;
			return false;		
		}	
	}

	std::map<std::string, double> all_hsp = {{"30-40", 1.0},  
			{"60-63", 1.0}, {"90-100", 1.0}};
	std::map<std::string, double> all_icu = {{"30-40", 1.0},  
			{"60-63", 1.0}, {"90-100", 1.0}};
	infection.set_hospitalized_ICU_fractions(all_icu);
	infection.set_hospitalized_fractions(all_hsp);
	for (const auto& age : ages){
		// Enough calls to capture the error if present
		for (int i=0; i<n_tot; ++i){	
			if (infection.will_die_non_icu(age) == true){
				std::cerr << "All should be in ICU and have ICU mortality rates" << std::endl;
				return false;		
			}
		}	
	}

	return true;	
}

/// \brief Function for checking different types of age-dependent rates
bool check_age_dependent_rates(age_rates_setter set_rates, age_rates_getter get_rates,
								age_rates_caller call, Infection& infection, 
								std::map<std::string, double> raw_rates, 
								std::map<std::string, std::tuple<int, int, double>> expected_rates)
{
	// Create and retrieve rates 
	(infection.*set_rates)(raw_rates);
	const std::map<std::string, std::tuple<int, int, double>>& created_rates = (infection.*get_rates)();

	// Compare
	std::tuple<int, int, double> exp_rate;
	for (const auto& rate : created_rates){
		exp_rate = expected_rates[rate.first];
		if (std::get<0>(exp_rate) != std::get<0>(rate.second)){
			std::cerr << "Age-dependent rates: wrong minimum age in the interval" << std::endl;
			return false;
		}
		if (std::get<1>(exp_rate) != std::get<1>(rate.second)){
			std::cerr << "Age-dependent rates: wrong maximum age in the interval" << std::endl;
			return false;
		}
		if (!float_equality<double>(std::get<2>(exp_rate), std::get<2>(rate.second), 1e-5)){
			std::cerr << "Age-dependent rates: wrong rate" << std::endl;
			return false;
		}
	}			

	// Verify the checking and sampling for each age group
	// It samples n_tot population of a fixed age and then 
	// checks if the rate is comparable to nominal
	int n_tot = 1e6;
	bool hsp = false;
	for (const auto& rate : created_rates){
		int n_affected = 0;
		int age = std::get<0>(rate.second)+1;
		for (int i=0; i<n_tot; ++i){
			if ((infection.*call)(age, false) == true){
				n_affected++;
			}	
		}
		double fr_affected = (static_cast<double>(n_affected))/(static_cast<double>(n_tot));
		double fr_exp = std::get<2>(rate.second);
		if (!float_equality<double>(fr_affected, fr_exp, 0.01)){
			std::cerr << "Age-dependent rates: computed rate for age " << age 
					  << " not equal nominal" << std::endl;
			return false;
		}		
	}
	return true;
}

/// \brief Function for checking different types of age-dependent rates
bool check_age_dependent_rates(age_rates_setter set_rates, age_rates_getter get_rates,
								age_rates_caller_no_bool call, Infection& infection, 
								std::map<std::string, double> raw_rates, 
								std::map<std::string, std::tuple<int, int, double>> expected_rates)
{
	// Create and retrieve rates 
	(infection.*set_rates)(raw_rates);
	const std::map<std::string, std::tuple<int, int, double>>& created_rates = (infection.*get_rates)();

	// Compare
	std::tuple<int, int, double> exp_rate;
	for (const auto& rate : created_rates){
		exp_rate = expected_rates[rate.first];
		if (std::get<0>(exp_rate) != std::get<0>(rate.second)){
			std::cerr << "Age-dependent rates: wrong minimum age in the interval" << std::endl;
			return false;
		}
		if (std::get<1>(exp_rate) != std::get<1>(rate.second)){
			std::cerr << "Age-dependent rates: wrong maximum age in the interval" << std::endl;
			return false;
		}
		if (!float_equality<double>(std::get<2>(exp_rate), std::get<2>(rate.second), 1e-5)){
			std::cerr << "Age-dependent rates: wrong rate" << std::endl;
			return false;
		}
	}			

	// Verify the checking and sampling for each age group
	// It samples n_tot population of a fixed age and then 
	// checks if the rate is comparable to nominal
	int n_tot = 1e6;
	for (const auto& rate : created_rates){
		int n_affected = 0;
		int age = std::get<0>(rate.second)+1;
		for (int i=0; i<n_tot; ++i){
			if ((infection.*call)(age) == true)
				n_affected++;	
		}
		double fr_affected = (static_cast<double>(n_affected))/(static_cast<double>(n_tot));
		double fr_exp = std::get<2>(rate.second);
		if (!float_equality<double>(fr_affected, fr_exp, 0.01)){
			std::cerr << "Age-dependent rates: computed rate for age " << age 
					  << " not equal nominal" << std::endl;
			return false;
		}		
	}
	return true;
}




/// \brief Test sampling of an infection distribution against expected mean
bool check_distribution(dist_sampling dist, Infection& infection, double exp_mean)
{
	std::vector<double> rnum;
	for (int i=0; i<1000000; ++i)
		rnum.push_back((infection.*dist)());
	
	// Compare mean value
	double rng_mean = std::accumulate(rnum.begin(), rnum.end(), 0.0)/(static_cast<double>(rnum.size()));
	
	if (!float_equality<double>(exp_mean, rng_mean, 0.01)){
		std::cerr << "Expected value: " << exp_mean << "\nComputed value: " << rng_mean << std::endl;
		return false;
	}
	
	return true;
}

/// \brief Verification for single number distributions
bool check_simple_distributions(Infection& infection, std::vector<double> probabilities)
{
	// Total number of outcomes
	int n_tot = 1e6;
	// Measured number of outcomes that are true
	int n_tested = 0, n_tested_hsp = 0, n_fneg = 0, n_fpos = 0;

	// For each probability, collect agents that have
	// the value "True"
	for (int i=0; i<n_tot; ++i){
		if (infection.will_be_tested(probabilities.at(0)))
			++n_tested;
		if (infection.tested_in_hospital(probabilities.at(1)))
			++n_tested_hsp;
		if (infection.false_negative_test_result(probabilities.at(2)))
			++n_fneg;
		if (infection.false_positive_test_result(probabilities.at(3)))
			++n_fpos;			
	}

	// Compute ratios and compare to expected
	double fr_tested = static_cast<double>(n_tested)/(static_cast<double>(n_tot));
	double fr_tested_hsp = static_cast<double>(n_tested_hsp)/(static_cast<double>(n_tot));
	double fr_fneg = static_cast<double>(n_fneg)/(static_cast<double>(n_tot));
	double fr_fpos = static_cast<double>(n_fpos)/(static_cast<double>(n_tot));

	if (!float_equality<double>(fr_tested, probabilities.at(0), 0.01)){
		std::cerr << "Wrong tested fraction\nExpected value: " << probabilities.at(0) 
				  << "\nComputed value: " << fr_tested << std::endl;
		return false;
	}
	if (!float_equality<double>(fr_tested_hsp, probabilities.at(1), 0.01)){
		std::cerr << "Wrong fraction of tested in a hospital\nExpected value: " << probabilities.at(1) 
				  << "\nComputed value: " << fr_tested_hsp << std::endl;
		return false;
	}
	if (!float_equality<double>(fr_fneg, probabilities.at(2), 0.01)){
		std::cerr << "Wrong fraction of tested false negative\nExpected value: " << probabilities.at(2) 
				  << "\nComputed value: " << fr_fneg << std::endl;
		return false;
	}
	if (!float_equality<double>(fr_fpos, probabilities.at(3), 0.01)){
		std::cerr << "Wrong fraction of tested false positive\nExpected value: " << probabilities.at(3) 
				  << "\nComputed value: " << fr_fpos << std::endl;
		return false;
	}

	return true;
}

/// \brief Verification of functionality for generating random IDs
bool check_random_ID(Infection& infection)
{
	// Increase index of a place/agent with a randomly
	// generated ID - at the end all the places should
	// have associated values larger than 0

	std::vector<int> houses = {0, 0, 0};
	std::vector<int> hospitals = {0, 0, 0, 0};
	std::vector<int> agents = {0, 0, 0, 0, 0};
	
	int n_hs = houses.size();
	int n_hsp = hospitals.size();
	int n_ag = agents.size();
	
	int n_tot = 1000;
	for (int i=0; i<n_tot; ++i){
		// If ID is past bounds, the following will crash because of .at()
		++houses.at(infection.get_random_household_ID(n_hs)-1);
		++hospitals.at(infection.get_random_hospital_ID(n_hsp)-1);
		++agents.at(infection.get_random_agent_ID(n_ag)-1);
	}

	if (std::find(houses.begin(), houses.end(), 0) != houses.end()){
		std::cerr << "Not all houses have IDs" << std::endl;
		return false;
	}
	if (std::find(hospitals.begin(), hospitals.end(), 0) != hospitals.end()){
		std::cerr << "Not all hospitals have IDs" << std::endl;
		return false;
	}
	if (std::find(agents.begin(), agents.end(), 0) != agents.end()){
		std::cerr << "Not all agents have IDs" << std::endl;
		return false;
	}

	return true;
}

/// \brief Verification of testing wait time functionality
bool check_testing_wait_distribution(Infection& infection)
{
	double t0 = 200.5, t_test = 0.0;
	int n_tot = 1e6;
	for (int i=0; i<n_tot; ++i){
		t_test = infection.wait_time_for_test(0.0);
		if (t_test < 1){
			std::cerr << "Minimum wait time should be 1.0" << std::endl;
			return false;
		}

		t_test = infection.wait_time_for_test(t0);
		if (t_test < t0){
			std::cerr << "Minimum wait time should be " << t0 << std::endl;
			return false;
		}
	}
	return true;
}
