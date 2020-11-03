#include <algorithm>
#include "../common/test_utils.h"
#include "../../include/rng.h"

/***************************************************** 
 *
 * Test suite for functionality of the RNG class
 *
 *****************************************************/

// Tests
bool uniform_real_test(double, double);
bool uniform_int_test(double, double);
bool gamma_test(double, double, double);
bool lognormal_test(double, double, double);
bool weibull_test(double, double, double);
bool random_shuffle_test();

int main()
{
	double gamma_shape = 0.7696, gamma_scale = 3.4192, gamma_mean = 2.6314;
	double logn_meanx = 2.6696, logn_stx = 0.4760, logn_mean = 16.1652;
	double wb_shape = 1.6726, wb_scale = 10.1237, wb_mean = 9.0433;

	test_pass(uniform_real_test(0.0, 1.0), "Uniform real distribution");
	test_pass(uniform_int_test(0, 10), "Uniform int distribution");
	test_pass(gamma_test(gamma_shape, gamma_scale, gamma_mean), "Gamma distribution");
	test_pass(lognormal_test(logn_meanx, logn_stx, logn_mean), "Lognormal distribution");
	test_pass(weibull_test(wb_shape, wb_scale, wb_mean), "Weibull distribution");
	test_pass(random_shuffle_test(), "Random shuffling");
}

/// Test if the uniform distribution generation is correct
bool uniform_real_test(double a, double b)
{
	RNG rng;
	std::vector<double> rnum;

	for (int i=0; i<100000; ++i)
		rnum.push_back(rng.get_random(a, b));
	
	// Compare mean value
	double exp_mean = 0.5*(a + b);
	double rng_mean = std::accumulate(rnum.begin(), rnum.end(), 0.0)/static_cast<double>(rnum.size());
	
	if (!float_equality<double>(exp_mean, rng_mean, 0.01)){
		std::cout << exp_mean << " " << rng_mean << std::endl;
		return false;
	}
	
	return true;
}

/// Test if the uniform integer distribution generation is correct
bool uniform_int_test(double a, double b)
{
	RNG rng;
	std::vector<int> rnum;

	for (int i=0; i<1000000; ++i)
		rnum.push_back(rng.get_random_int(a, b));
	
	// Compare mean value
	int exp_mean = (a + b)/2;
	int rng_mean = std::accumulate(rnum.begin(), rnum.end(), 0)/rnum.size();
	
	if (exp_mean != rng_mean){
		std::cout << exp_mean << " " << rng_mean << std::endl;
		return false;
	}
	
	return true;
}

/** 
 * \brief Test if the gamma distribution generation is correct
 * \details Checks the mean and saves the generated data
 */
bool gamma_test(double shape, double scale, double exp_mean)
{
	RNG rng;
	std::vector<double> rnum;
	std::string fname_out("test_data/gamma_values.txt");

	std::ofstream fout(fname_out);

	for (int i=0; i<100000; ++i){
		rnum.push_back(rng.get_random_gamma(shape, scale));
		fout << rnum[i] << "\n";
	}

	// Compare mean and standard deviation values  
	double rng_mean = std::accumulate(rnum.begin(), rnum.end(), 0.0)/static_cast<double>(rnum.size());
	
	if (!float_equality<double>(exp_mean, rng_mean, 0.01)){
		std::cout << exp_mean << " " << rng_mean << std::endl;
		return false;
	}

	return true;
}

/** 
 * \brief Test if the lognormal distribution generation is correct
 * \details Checks the mean and saves the generated data
 */
bool lognormal_test(double meanx, double stx, double exp_mean)
{
	RNG rng;
	std::vector<double> rnum;
	std::string fname_out("test_data/lognormal_values.txt");

	std::ofstream fout(fname_out);

	for (int i=0; i<100000; ++i){
		rnum.push_back(rng.get_random_lognormal(meanx, stx));
		fout << rnum[i] << "\n";
	}

	// Compare mean and standard deviation values  
	double rng_mean = std::accumulate(rnum.begin(), rnum.end(), 0.0)/static_cast<double>(rnum.size());
	
	if (!float_equality<double>(exp_mean, rng_mean, 0.01)){
		std::cout << exp_mean << " " << rng_mean << std::endl;
		return false;
	}

	return true;
}

/** 
 * \brief Test if the Weibull distribution generation is correct
 * \details Checks the mean and saves the generated data
 */
bool weibull_test(double shape, double scale, double exp_mean)
{
	RNG rng;
	std::vector<double> rnum;
	std::string fname_out("test_data/weibull_values.txt");

	std::ofstream fout(fname_out);

	for (int i=0; i<100000; ++i){
		rnum.push_back(rng.get_random_weibull(shape, scale));
		fout << rnum[i] << "\n";
	}

	// Compare mean and standard deviation values  
	double rng_mean = std::accumulate(rnum.begin(), rnum.end(), 0.0)/static_cast<double>(rnum.size());
	
	if (!float_equality<double>(exp_mean, rng_mean, 0.01)){
		std::cout << exp_mean << " " << rng_mean << std::endl;
		return false;
	}

	return true;
}

/// Test of random shuffling functionality
bool random_shuffle_test()
{
	RNG rng;
	std::vector<int> v2s = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<int> v_orig(v2s);
	rng.vector_shuffle(v2s);
	return !(v2s == v_orig);
}
