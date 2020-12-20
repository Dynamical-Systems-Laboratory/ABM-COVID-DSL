import numpy as np

Rd = []
with open('output/simulation.log', 'r') as fin, open('input_data/random_v_numbers_rhr.txt', 'w') as fout:
	for line in fin:
		#if 'casualities' in line:
		#	temp = line.strip().split()
		#	Rd.append(float(temp[-1]));
		if 'residents' in line:
			temp = line.strip().split()
			fout.write(temp[-1]+'\n')
			
#print(sum(Rd)/len(Rd))
#print(np.std(Rd))
