
# ------------------------------------------------------------------
#
#	Module for simple generation of a constraint but random
#		population/community 
#
# ------------------------------------------------------------------

import time as time
import random
import numpy as np

class Agents:
	''' Collection of agents that make the population '''
	
	def __init__(self, n, nh, ns, nw, ni):
		''' Initialize population with n agents in nh households
				ns and nw number of agents attending school 
				and/or work respectively; ni is number of initially
				infected '''		

		# List of dicts with input data for each agent
		self.agents = []

		# Total number of agents, infected agents, 
		# agents at schools, and agents at workplaces
		self.ntot = n 
		self.n_at_schools = ns
		self.n_at_works = nw
		self.n_infected = ni
		
	def distribute(self, houses, schools, workplaces):
		''' Assign agents to random places '''
		
		print('Assigning coordinates of the household')

		# Indices of agents that are students and workers
		# Using random.sample to get unique IDs
		studentIDs = random.sample(range(1, self.ntot+1), self.n_at_schools) 
		workerIDs = random.sample(range(1, self.ntot+1), self.n_at_works) 
		# Indices of agents that are infected
		infectedIDs = random.sample(range(1, self.ntot+1), self.n_infected)

		for i in range(1, self.ntot+1):
			# Assign ID (not related to simulation ID)
			atemp = {'ID': i}

			# Pick places
			atemp['hID'] = np.random.randint(1, houses.ntot + 1)

			# Pick schools/workplaces if student/worker
			if i in studentIDs:
				atemp['is_student'] = True
				atemp['sID'] = np.random.randint(1, schools.ntot + 1)
			else:
				atemp['is_student'] = False
				atemp['sID'] = 0	
	
			if i in workerIDs:
				atemp['works'] = True
				atemp['wID'] = np.random.randint(1, workplaces.ntot + 1)
			else:
				atemp['works'] = False
				atemp['wID'] = 0	
			 		
			# Assign coordinates
			atemp['x'] = houses.houses[atemp['hID']-1]['x']
			atemp['y'] = houses.houses[atemp['hID']-1]['y']

			# Determine if infected
			if i in infectedIDs:
				atemp['is_infected'] = True				
			else:
				atemp['is_infected'] = False
				
			# Assign age - Gaussian distribution
			atemp['age'] = int(round(max(0.0, np.random.normal(40.0, 18.0))))
			
			# Store and reset
			self.agents.append(atemp)
			atemp = {}

	def __repr__(self):
		''' String output for stdout or files '''
		
		atemp = []
		for agent in self.agents:
			temp = []

			if agent['is_student']:
				temp.append('1')
			else:
				temp.append('0')

			if agent['works']:
				temp.append('1')
			else:
				temp.append('0')

			temp.append(str(agent['age']))
			temp.append(str(agent['x']))
			temp.append(str(agent['y']))
			temp.append(str(agent['hID']))
			temp.append(str(agent['sID']))
			temp.append(str(agent['wID']))

			if agent['is_infected']:
				temp.append('1')
			else:
				temp.append('0')
			
			atemp.append((' ').join(temp))

		return ('\n').join(atemp)

class Places(object):
	''' Class for generation of places '''
	
	def __init__(self, n_tot, x_lim, y_lim):
		''' Generate n_tot places with random 
				location according to provided limits'''

		self.ntot = n_tot
		self.places = []

		for i in range(1, self.ntot + 1):
			# ID
			htemp = {'ID' : i}

			# Coordinates
			htemp['x'] = np.random.uniform(x_lim[0], x_lim[1], 1)[0]
			htemp['y'] = np.random.uniform(y_lim[0], y_lim[1], 1)[0]

			# Store and reset the temporary dict
			self.places.append(htemp) 
			htemp = {}
					
	def __repr__(self):
		''' String output for stdout or files '''
		
		temp = []
		for place in self.places:
			temp.append((' ').join([str(place['ID']), str(place['x']), str(place['y'])])) 

		return ('\n').join(temp)

class Houses(Places):
	''' Generate and store households '''

	def __init__(self, n_tot, x_lim, y_lim):
		super(Houses, self).__init__(n_tot, x_lim, y_lim)
		self.houses = self.places

class Schools(Places):
	''' Generate and store schools '''

	def __init__(self, n_tot, x_lim, y_lim):
		super(Schools, self).__init__(n_tot, x_lim, y_lim)
		self.schools = self.places

class Works(Places):
	''' Generate and store workplaces '''

	def __init__(self, n_tot, x_lim, y_lim):
		super(Works, self).__init__(n_tot, x_lim, y_lim)
		self.works = self.places

if __name__ == '__main__':

	# Quick demo how to run

	nh = 3
	ns = 5
	nw = 10
	
	n_agents = 20
	n_infected = 3	

	sp_lim_x = [10, 100]
	sp_lim_y = [200, 500]

	houses = Houses(nh, sp_lim_x, sp_lim_y)
	schools = Schools(ns, sp_lim_x, sp_lim_y)	
	works = Works(nw, sp_lim_x, sp_lim_y)
		
	agents = Agents(n_agents, nh, ns, nw, n_infected)
	agents.distribute(houses, schools, works)

	with open('schools_test_out.txt', 'w') as fout:
		fout.write(repr(schools))

	with open('houses_test_out.txt', 'w') as fout:
		fout.write(repr(houses))

	with open('agents_test_out.txt', 'w') as fout:
		fout.write(repr(agents))
	
