import numpy as np

# Compute average simulation time
all_times = []
with open('output/simulation.log', 'r') as fin:
	for line in fin:
		if '[ms]' in line:
			temp = line.strip().split()[-1].split('[')
			all_times.append(float(temp[-2]));

print('Number of realizations: ', len(all_times))
print('Time, s: ', '{:.2f}'.format(sum(all_times)/len(all_times)/1e3), ' +/- ', '{:.2f}'.format(np.std(all_times)/1e3))

