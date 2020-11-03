#
# Script for converting collected NR data into 
#	the format for comparison with the simulations
#

def clean_and_save(file1, file2, cx, cy, ech):
	''' Remove rows with character ech from 
			the data in file1 column cy and corresponding
			rows in cx, then save to file2. Column numbering
			starts with 0. '''

	with open(file1, 'r') as fin, open(file2, 'w') as fout:
		# Skip the header
		next(fin)
		for line in fin:
			temp = line.strip().split()
			if temp[cy] == ech:
				continue
			else:
				fout.write((' ').join([temp[cx], temp[cy], '\n']))

# Input
data_file = 'input_data/New_Rochelle_covid_data.txt' 
no_entry_mark = '?'

# Number of active cases
clean_and_save(data_file, 'output/real_active_w_time.txt', 1, 2, no_entry_mark)
# Total number of cases 
clean_and_save(data_file, 'output/real_tot_cases_w_time.txt', 1, 3, no_entry_mark)
# Number of deaths in the county
clean_and_save(data_file, 'output/real_tot_deaths_county_w_time.txt', 1, 4, no_entry_mark)
