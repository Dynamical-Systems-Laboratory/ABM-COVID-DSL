#ifndef TESTING_H
#define TESTING_H

#include "common.h"
#include "utils.h"
#include <deque>

/***************************************************** 
 * class: Testing
 * 
 * Attributes and functionality of infection testing 
 * 
 *****************************************************/

class Testing{
public:
	
	//
	// Constructors
	//

	/**
	 * \brief Creates a Testing object with default attributes
	 */
	Testing() = default;

	//
	// Initialization
	//

	/**
	 *	\brief Sets initial testing attributes 
	 *
	 *	@param tst - time when testing starts
	 *	@param neg - fraction of negative tests
	 *	@param fneg - fraction of false negative tests
	 *	@param fpos - fraction of false positive tests
	 *	@param sy_ini - initial fraction of symptomatic agents to test
	 *	@param exp_ini - initial fraction of exposed agents to test
	 */
	void initialize_testing(const double tst, const double neg, const double fneg, 
			const double fpos, const double sy_ini, const double exp_ini);

	/**
	 * \brief Sets the vector of times vs. testing fractions
	 */
	void set_time_varying(const std::vector<std::vector<double>> time_vec);

	//
	// Time-varying testing
	//

	/// True if period of testing has started
	bool started(const double time) const { return time >= start_testing; }

	/** 
	 * \brief Check if time to change values and change them if yes
	 *  @param time - current simulation time 
	 *	@return True if values were changed
	 */
	bool check_switch_time(const double time);

	//
	// Getters
	//
	
	/// Probability symptomatic gets tested
	double get_sy_tested_prob() const { return sy_fraction_to_get_tested; }
	/// Probability exposed gets tested
	double get_exp_tested_prob() const { return exposed_fraction_to_get_tested; }
	/// Probability flu (non-covid symptomatic) gets tested
	double get_prob_flu_tested() const { return flu_fraction_to_test; } 

private:
	// Vector of times marking the time testing is supposed
	// to change value and corresponding values, i.e. 
	// fractions of exposed and symptomatic to be tested
	// time | fraction exposed tested | fraction Sy tested
	std::deque<std::vector<double>> testing_change_times = {};

	double start_testing = 0.0;
	double negative_tests_fraction = 0.0;
	double fraction_false_negative = 0.0;
	double fraction_false_positive = 0.0;

	double sy_fraction_to_get_tested = 0.0;
	double exposed_fraction_to_get_tested = 0.0;
	double flu_fraction_to_test = 0.0;

	double time_of_next_change = 0.0;
	std::vector<double> next_testing_fractions = {};

	// Calculates fraction of flu agents to get tested
	void set_flu_testing();

};

#endif
