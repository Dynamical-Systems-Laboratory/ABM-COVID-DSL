#include "../include/testing.h"

/***************************************************** 
 * class: Testing
 * 
 * Attributes and functionality of infection testing 
 * 
 *****************************************************/

// Sets initial testing attributes
void Testing::initialize_testing(const double tst, const double neg, const double fneg, 
				const double fpos, const double sy_ini, const double exp_ini)
{
	start_testing = tst;
	negative_tests_fraction = neg;
	fraction_false_negative = fneg;
	fraction_false_positive = fpos;
	sy_fraction_to_get_tested = sy_ini;
	exposed_fraction_to_get_tested = exp_ini;
	next_testing_fractions.resize(2,0.0);
	set_flu_testing();
}

// Sets the vector of times vs. testing fractions
void Testing::set_time_varying(const std::vector<std::vector<double>> time_vec)
{
	for (const auto entry : time_vec){
		testing_change_times.push_back(entry);
	}
	// Next values
	std::vector<double> temp(3,0.0);
	temp = testing_change_times.front();
	time_of_next_change = temp.at(0);
	for (int i=1; i<3; ++i){
		next_testing_fractions.at(i-1) = temp.at(i);
	}
	// Remove the current 
	testing_change_times.pop_front();
}

void Testing::set_flu_testing()
{
	// Set testing-related probabilities
	flu_fraction_to_test = (fraction_false_positive + negative_tests_fraction)*sy_fraction_to_get_tested;
}

// Check if time to change values and change them if yes
bool Testing::check_switch_time(const double time)
{
	double tol = 1e-3;
	std::vector<double> temp(3,0.0);
	if (equal_floats<double>(time_of_next_change, time, tol)){
		// New probabilities/fractions
		exposed_fraction_to_get_tested = next_testing_fractions.at(0);
		sy_fraction_to_get_tested = next_testing_fractions.at(1);
		// Next values if exist
		if (!testing_change_times.empty()){
			temp = testing_change_times.front();
			time_of_next_change = temp.at(0);
			for (int i=1; i<3; ++i){
				next_testing_fractions.at(i-1) = temp.at(i);
			}
			set_flu_testing();
			testing_change_times.pop_front();
		}else{
			set_flu_testing();
		}
		return true;
	}
	return false;
}
