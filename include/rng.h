#ifndef RNG_H
#define RNG_H

#include <random>

/***************************************************** 
 * class: RNG
 * 
 * Random number generator 
 * 
 *****************************************************/

class RNG
{
public:
    RNG() : gen(std::random_device()()) { } 

	/**
	 *	\brief Random number sampled from uniform distribution
	 *	@param dmin - minimum, inclusive
	 *	@param dmax - maximum, exclusive
	 */
    double get_random(const double dmin, const double dmax)
	{  
        std::uniform_real_distribution<double> dist(dmin, dmax);
        return dist(gen);
    }

	/**
	 *	\brief Random integer sampled from uniform distribution of ints
	 *	@param dmin - minimum, inclusive
	 *	@param dmax - maximum, inclusive
	 */
    int get_random_int(const int dmin, const int dmax)
	{  
        std::uniform_int_distribution<int> dist(dmin, dmax);
        return dist(gen);
    }

	/**
	 *	\brief Random number sampled from a gamma distribution
	 *	@param k - shape parameter 
	 *	@param theta - scale parameter 
	 */
    double get_random_gamma(const double k, const double theta)
	{  
        std::gamma_distribution<double> dist(k, theta);
        return dist(gen);
    }

	/**
	 *	\brief Random number sampled from a lognormal distribution
	 *	@param m - mean 
	 *	@param s - standard deviation 
	 */
    double get_random_lognormal(const double m, const double s)
	{  
        std::lognormal_distribution<double> dist(m, s);
        return dist(gen);
    }

	/**
	 *	\brief Random number sampled from a Weibull distribution
	 *	@param a - shape parameter
	 *	@param b - scale parameter 
	 */
    double get_random_weibull(const double a, const double b)
	{  
        std::weibull_distribution<double> dist(a, b);
        return dist(gen);
    }

	/// Performs in-place random shuffling of a vector
	void vector_shuffle(std::vector<int>& v)
	{ 
		std::shuffle(v.begin(), v.end(), gen);
	}

private:
    std::mt19937 gen;
};

#endif
