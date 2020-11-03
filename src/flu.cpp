#include "../include/flu.h"

/****************************************************                                
 * class: Flu 
 * 
 * Manages the susceptible agents that have symptoms                                 
 * from an infection other than COVID                                                
 * 
 *****************************************************/

// Remove a susceptible agent 
void Flu::remove_susceptible_agent(const int index)
{
	std::vector<int> new_agent_IDs = {};
	std::remove_copy(susceptible_agent_IDs.begin(), susceptible_agent_IDs.end(),
					std::back_insert_iterator<std::vector<int>>(new_agent_IDs), index);
	std::swap(new_agent_IDs, susceptible_agent_IDs);	
}

// Remove a flu agent 
void Flu::remove_flu_agent(const int index)
{
	std::vector<int> new_agent_IDs = {};
	std::remove_copy(flu_agent_IDs.begin(), flu_agent_IDs.end(),
					std::back_insert_iterator<std::vector<int>>(new_agent_IDs), index);
	std::swap(new_agent_IDs, flu_agent_IDs);	
}

// Remove recovered from flu, add new chosen randomly
int Flu::swap_flu_agent(const int index)
{
	remove_flu_agent(index);
	int n_susceptible = susceptible_agent_IDs.size();
	if (n_susceptible == 0){
		std::cout << "No susceptible left for modeling flu - returning"
				  << std::endl;
		return -1;
	}
	// Pick from available susceptible
	int ind = rng.get_random_int(0, n_susceptible - 1);
	int agent_ind = susceptible_agent_IDs.at(ind);
	remove_susceptible_agent(agent_ind);
	flu_agent_IDs.push_back(agent_ind);
	// Actual agent ID
	return agent_ind;
}

// Create initial flu population
std::vector<int> Flu::generate_flu()
{
	int n_flu = nc_sy_frac*susceptible_agent_IDs.size();
	for (int i=0; i<n_flu; ++i){
		int ind = rng.get_random_int(0, susceptible_agent_IDs.size() - 1);
		int agent_ind = susceptible_agent_IDs.at(ind);
		remove_susceptible_agent(agent_ind);
		flu_agent_IDs.push_back(agent_ind);
	}
	return flu_agent_IDs;
}


