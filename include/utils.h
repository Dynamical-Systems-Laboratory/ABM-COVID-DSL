#ifndef UTILS_H
#define UTILS_H

#include "common.h"

/**
 * \brief Convert a string to all lower case
 * \details From https://en.cppreference.com/w/cpp/string/byte/tolower
 *
 * @param [in] s - string which will be converted to all lower case
 */
std::string str_to_lower(std::string s); 

/**
 * \brief Compare num1 and num2 as inexact values
 * \details For floating point compoarisons, i.e. floats, doubles
 * Current version based on 
 * 		https://stackoverflow.com/a/15012792/2763915
 * @param num1 - first number
 * @param num2 - second number
 * @param tol - relative tolerance for comparison
 * @return true if equal within the tolerance
 */
template <typename T>
bool equal_floats(T, T, T);

//
// Implementations
//

// Compares two floating point numbers for approximate equality
template <typename T>
bool equal_floats(T num1, T num2, T tol)
{
	T max_num_one = std::max({1.0, std::fabs(num1) , std::fabs(num2)});
    return std::fabs(num1 - num2) <= tol*max_num_one;
}


#endif
