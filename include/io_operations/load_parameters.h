#ifndef LOAD_PARAMETERS_H
#define LOAD_PARAMETERS_H

#include "FileHandler.h"
#include "../common.h"

/***************************************************** 
 * class: LoadParameters 
 * 
 * Class for reading and storing parameters from file 
 * 
 *****************************************************/

class LoadParameters
{
public:
		
	LoadParameters() = default; 

	/**
	 * \brief Read parameters from file, store the as a map
	 * \details In the file parameter name can be multiple words
	 * 		but it needs to be in a separate line with a // comment
	 * 		type as the first element of the line; it is followed
	 * 		by a numeric value in the next line. 
	 *
	 * @param infile - input file with parameters
	 */
	std::map<std::string, double> load_parameter_map(const std::string infile);
	
	/** 
	 * \brief Read age-dependent distributions as map, store the as a map
	 * \details Keys are age ranges as strings, in a specific form like "40-43"
	 *
	 * @param infile - input file with parameters
	 */
	std::map<std::string, double> load_age_dependent(const std::string infile);

};
#endif
