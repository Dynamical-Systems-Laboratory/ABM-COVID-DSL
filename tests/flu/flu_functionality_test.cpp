#include "../../include/flu.h"
#include "../common/test_utils.h"

/***************************************************** 
 *
 * Test suite for Flu class 
 * 
 *****************************************************/

// Tests
bool flu_generation();
bool flu_transitions();
bool flu_testing();

int main()
{
	test_pass(flu_generation(), "Creation of flu agents");
	test_pass(flu_transitions(), "Transitions of flu agents");
	test_pass(flu_testing(), "Testing of flu agents");
}

/// Checks if correctness of creating agents with flu
bool flu_generation()
{
	int n_tot = 1000;
	double fr_flu = 0.3;
	std::vector<int> flu_agents = {};

	Flu flu;
	flu.set_fraction(fr_flu);

	// To simulate adding every second
	// since susceptible population doesn't include 
	// all agents 
	for (int i=1; i<=n_tot*2; ++i){
		if (i%2 == 0){
			flu.add_susceptible_agent(i);
		}
	}
	flu_agents = flu.generate_flu();

	// Test if correct fraction
 	int n_flu = flu_agents.size();
	double fr_generated = static_cast<double>(n_flu)/static_cast<double>(n_tot);
	if (!float_equality<double>(fr_generated, fr_flu, 1e-5)){
		std::cerr << "Wrong fraction of agents with flu" << std::endl;			
		return false;
	}

	// Test if all are unique
	auto it = std::unique(flu_agents.begin(), flu_agents.end());
	bool was_unique = (it == flu_agents.end());
	
	if (!was_unique){
		std::cerr << "IDs of agents with flu are not unique" << std::endl;
		return false;
	}

	return true;
}

/// Verifies correctness of adding and removing agent types
bool flu_transitions()
{
	int n_tot = 100;
	double fr_flu = 0.1;
	int old_flu = 0, new_flu = 0, swap_ind = 1;
	std::vector<int> flu_agents = {};

	Flu flu;
	flu.set_fraction(fr_flu);

	for (int i=1; i<=n_tot; ++i){
		flu.add_susceptible_agent(i);
	}
	flu_agents = flu.generate_flu();

	// Test if swapping is working
	old_flu = flu_agents.at(swap_ind);
	new_flu = flu.swap_flu_agent(old_flu);

	// Now check if old_flu is no more in the flu vector
	// or in the susceptible vector
	const std::vector<int>& susceptible_agents_swap = flu.get_susceptible_IDs();
	const std::vector<int>& flu_agents_swap = flu.get_flu_IDs();

	if ((std::find(susceptible_agents_swap.begin(), susceptible_agents_swap.end(), 
									old_flu)) != susceptible_agents_swap.end()){
		std::cerr << "Former agent with flu still in the available susceptible group" << std::endl;
		return false;
	}

	if ((std::find(flu_agents_swap.begin(), flu_agents_swap.end(), 
									old_flu)) != flu_agents_swap.end()){
		std::cerr << "Former agent with flu still in the flu group" << std::endl;
		return false;
	}

	// And if new_flu not in susceptible anymore and in the flu
	if ((std::find(susceptible_agents_swap.begin(), susceptible_agents_swap.end(), 
									new_flu)) != susceptible_agents_swap.end()){
		std::cerr << "New agent with flu still in the available susceptible group" << std::endl;
		return false;
	}

	if ((std::find(flu_agents_swap.begin(), flu_agents_swap.end(), 
									new_flu)) == flu_agents_swap.end()){
		std::cerr << "New agent with flu not in the flu group" << std::endl;
		return false;
	}

	return true;
}

/// Verifies if testing practices are correctly implemented
bool flu_testing()
{
	int n_tot = 1000000;
	int n_tested = 0, n_fp = 0;
	double fr_tested = 0.637, fr_fp = 0.2;
	double fr_t_comp = 0.0, fr_fp_comp = 0.0;
	double t0 = 0.0, neg = 0.78, fneg = 0.1;
	double sy_ini = 0.65, exp_ini = 0.72;
	std::vector<int> flu_agents = {};

	Testing testing;
	testing.initialize_testing(t0, neg, fneg, fr_fp, sy_ini, exp_ini);

	Flu flu;
	flu.set_fraction_tested_false_positive(fr_fp);
	// Test if proper fraction will get tested
	for (int i=0; i<n_tot; ++i){
		if (flu.getting_tested(testing)){
			++n_tested;
		}
		if (flu.tested_false_positive()){
			++n_fp;
		}
	}

	fr_t_comp = static_cast<double>(n_tested)/static_cast<double>(n_tot);
	fr_fp_comp = static_cast<double>(n_fp)/static_cast<double>(n_tot);

	if (!float_equality<double>(fr_tested, fr_t_comp, 1e-3)){
		std::cerr << "Wrong flu fraction tested" << std::endl;			
		return false;
	}
	if (!float_equality<double>(fr_fp, fr_fp_comp, 1e-3)){
		std::cerr << "Wrong false positive fraction" << std::endl;
		return false;
	}

	return true;
}
