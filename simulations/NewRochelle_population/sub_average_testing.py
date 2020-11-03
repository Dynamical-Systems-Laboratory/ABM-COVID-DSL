#!/usr/local/bin/python3.6
# Substitute average testing value from optimization

import sys

if len(sys.argv) != 2:
	sys.stderr.write('Usage: python3.X %s sy_fraction\n' % sys.argv[0])
	raise SystemExit(1)

sy_fraction = sys.argv[1]

parameters = []
with open('input_data/infection_parameters.txt','r') as fin:
	parameters = fin.readlines()

for ind,param in enumerate(parameters):
	if 'average fraction to get tested' in param:
		parameters[ind+1] = str(sy_fraction) + '\n'
		break

with open('input_data/infection_parameters.txt','w') as fout:
	fout.writelines(parameters)
