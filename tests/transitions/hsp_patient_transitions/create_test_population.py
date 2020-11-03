#
# Script for generating population files for 
#	running contributions tests
#

import random

# Input file with info on population
agents_in = 'test_data/NR_agents.txt' 
# Number of agents to create
n_agents = 80000
# Place numbers
n_houses = 29645 
n_works = 882 
n_hsp = 3 
n_rh = 5
n_schools = 68 

with open(agents_in, 'w') as fout:
	for i in range(1,n_agents):
		agent = ['0']*15

		# Age and location
		agent[2] = str(random.randint(0,110))
		agent[3] = str(random.random()*10)
		agent[4] = str(random.random()*10)

		# Residence
		if random.random() < 0.3:
			print('Creating a non-COVID hospital patient')
			agent[6] = '1'
			agent[13] = str(random.randint(1,n_hsp))
			if random.random() < 0.8:
				agent[14] = '1'
			fout.write((' ').join(agent)+'\n')
			continue
		if random.random() < 0.1:
			print('Creating a retirement home resident')
			agent[8] = '1'
			agent[5] = str(random.randint(1,n_rh))
			if random.random() < 0.8:
				agent[14] = '1'
			fout.write((' ').join(agent)+'\n')
			continue
		# Regular household, determine if infected	
		agent[5] = str(random.randint(1,n_houses))
		if random.random() < 0.8:
			agent[14] = '1'

		# Workplaces and schools
		# Hospital staff
		if random.random() < 0.2:
			print('Agent works in a hospital')
			agent[12] = '1' 
			agent[13] = str(random.randint(1,n_hsp))
			if random.random() < 0.25:
				print('\t\t Agent also goes to school')
				agent[0] = '1'
				agent[7]= str(random.randint(1,n_schools))
			fout.write((' ').join(agent)+'\n')
			continue
		# Retirement homes
		if random.random() < 0.3:
			print('Agent works in a retirement home')
			agent[1] = '1' 
			agent[9] = '1'
			agent[11] = str(random.randint(1,n_rh))
			if random.random() < 0.25:
				print('\t\t Agent also goes to school')
				agent[0] = '1'
				agent[7]= str(random.randint(1,n_schools))
			fout.write((' ').join(agent)+'\n')
			continue
		# Works at school
		if random.random() < 0.3:
			print('Agent works at school')
			agent[1] = '1' 
			agent[10] = '1'
			agent[11] = str(random.randint(1,n_schools))
			if random.random() < 0.25:
				print('\t\t Agent also goes to school')
				agent[0] = '1'
				agent[7]= str(random.randint(1,n_schools))
			fout.write((' ').join(agent)+'\n')
			continue
		# Works elsewhere
		if random.random() < 0.4:
			agent[1] = '1' 
			agent[11] = str(random.randint(1,n_schools))
			if random.random() < 0.25:
				print('\t\t Agent also goes to school')
				agent[0] = '1'
				agent[7]= str(random.randint(1,n_schools))
			fout.write((' ').join(agent)+'\n')
		else:
			if random.random() < 0.5:
				print('Agent goes to school')
				agent[0] = '1'
				agent[7]= str(random.randint(1,n_schools))
			fout.write((' ').join(agent)+'\n')

