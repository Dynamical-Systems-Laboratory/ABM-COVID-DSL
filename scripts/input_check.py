# ------------------------------------------------------------------
#
#	Series of tests to check validity of ABM input
#
#	- Does not check for upper numeric limits since they are rather
#		high 	
#
# ------------------------------------------------------------------

import utils as ut

def load_data_for_checking(fname):
	''' Reads data from file and stores in the format ready 
			for verification '''
	
	# 
	# The format is inner list for each line, consisting 
	#	of strings representing each individual entry
	#
	# fname - name of the file to read from
	#

	output = []
	with open(fname, 'r') as fin:
		for line in fin:
			output.append(line.strip().split())
	
	return output

def test_place_IDs(data):
	''' Tests places IDs, assumes IDs 
			are first column and string format '''

	#
	# data - format as output by load_data_for_checking
	#

	# Minimum allowable ID value
	id_num = 1
	for line in data:
		if not (int(line[0]) == id_num):
			print('Place IDs not continuous')
			return False
		id_num += 1
	 
	return True

def check_numbers(data, num):
	''' Test if size of data equals expected '''
	
	#
	# data - format as output by load_data_for_checking
	# num - expected number of entries (lines)
	#

	if len(data) == num:
		return True
	else:
		print('Number of entries not equal expected')
		return False


def check_entry_number(data, num):
	''' Tests if number of entries for each object matches expected '''

	#
	# data - format as output by load_data_for_checking
	# num - expected number of entries per object (columns per line of input)
	#

	for line in data:
		if not (len(line) == num):
			print('Number of entries not equal expected')
			return False
		 
	return True

def test_place_properties(data, xlim, ylim):
	''' Checks correctness of place-specific properties 
			not covered by other tests'''

	#
	# data - format as output by load_data_for_checking
	# xlim, ylim - lists with min, max x and y values
	#

	for line in data:
		# Coordinates
		if (float(line[1]) < xlim[0]) or (float(line[1]) > xlim[1]):
			print('x coordinate of a place outside of the assigned range')
			return False
		if (float(line[2]) < ylim[0]) or (float(line[2]) > ylim[1]):
			print('y coordinate of a place outside of the assigned range')
			return False	
		 
	return True

def test_agent_properties(agent_data, houses_data, schools_data, workplaces_data):
	''' Checks correctness of agent-specfic properties
			not covered by other tests '''

	#
	# All data files have the format from load_data_for_checking
	#

	for line in agent_data:
		
		# If a student, check if school exists
		if bool(int(line[0])) == True:
			if not ((int(line[6]) > 0) and (int(line[6]) <= len(schools_data))):
				print('School ID in agent data out of range')
				return False
		else:
			# ID has to be 0
			if not (int(line[6]) == 0):
				print('Agent not a student but school ID not 0')
				return False
	
		# If works, check if workplace exist	
		if bool(int(line[1])) == True:
			if not ((int(line[7]) > 0) and (int(line[7]) <= len(workplaces_data))):
				print('Work ID in agent data out of range')
				return False
		else:
			# ID has to be 0
			if not (int(line[7]) == 0):
				print('Agent does not work but workplace ID not 0')
				return False
		
		# House ID needs to be in the range
		if not ((int(line[5]) > 0) and (int(line[5]) <= len(houses_data))):
			print('House ID in agent data out of range')
			return False

		# Agent infection status can either be 0 or 1
		if not ((int(line[8]) == 0) or (int(line[8]) == 1)):
			print('Agents infection status not 0 or 1')
			return False

		# Agent initial coordinates need to equal house coordinates
		hID = int(line[5]) 
		xh = float(houses_data[hID - 1][1]) 
		yh = float(houses_data[hID - 1][2])
		xa = float(line[3])
		ya = float(line[4])
		tol = 1.0e-5
	
		if not (ut.float_equality(xa, xh, tol) and ut.float_equality(ya, yh, tol)):
			print('Agent starting position does not match the household coordinates')
			return False
	
		# Check if age >= 0
		if not (int(line[2]) >= 0):
			print('Agents age is negative')
			return False

	return True

def check_agent_numbers(agent_data, ni, ns, nw):
	''' Check total number of agents that are infected, 
			students, or/and work '''

	#
	# agent_data - as from load_data_for_checking
	# n_infected, ns, nw - expected number of infected agents,
	# 	agents at schools, and at work 
	# 

	ni_cur = 0
	ns_cur = 0
	nw_cur = 0

	for line in agent_data:
		if int(line[0]) == 1:
			ns_cur += 1
		if int(line[1]) == 1:
			nw_cur += 1
		if int(line[8]) == 1:
			ni_cur += 1

	if not (ns_cur == ns):
		print('Number of students does not match expected')
		return False

	if not (nw_cur == nw):
		print('Number of working agents does not match expected')
		return False

	if not (ni_cur == ni):
		print('Number of infected agents does not match expected')
		return False

	return True

