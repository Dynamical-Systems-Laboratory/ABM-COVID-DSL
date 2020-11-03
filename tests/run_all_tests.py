import subprocess, os

import sys
py_path = '../scripts/'
sys.path.insert(0, py_path)

import utils as ut
from colors import *

#
# Compile and run all the tests 
#

# Bar delimiting the test suites
nSim = 10

print('\n'*2)

# Testing utilities
ut.msg('- '*nSim + 'TESTING UTILS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('common/')
subprocess.call(['make && ./tu_test'], shell=True)
os.chdir('../')

# Misc
print('\n'*2)
ut.msg('- '*nSim + 'MISCELANEOUS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('misc_cpp_tests/')
subprocess.call(['python3.6 run_misc_tests.py'], shell=True)
os.chdir('../')

# Random number generator class
print('\n'*2)
ut.msg('- '*nSim + 'RNG CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('rng/')
subprocess.call(['python3.6 run_rng_tests.py'], shell=True)
os.chdir('../')

# Infection class
print('\n'*2)
ut.msg('- '*nSim + 'INFECTION CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('infection/')
subprocess.call(['python3.6 run_infection_tests.py'], shell=True)
os.chdir('../')

# Places class - base and derived
print('\n'*2)
ut.msg('- '*nSim + 'PLACE CLASS TESTS (BASE AND DERIVED)' + ' -'*nSim, REVERSE+RED)
os.chdir('places/')
subprocess.call(['python3.6 run_places_tests.py'], shell=True)
os.chdir('../')

# Agent class
print('\n'*2)
ut.msg('- '*nSim + 'AGENT CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('agent/')
subprocess.call(['python3.6 run_agent_tests.py'], shell=True)
os.chdir('../')

# Testing class
print('\n'*2)
ut.msg('- '*nSim + 'TESTING CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('testing_class/')
subprocess.call(['python3.6 run_testing_class_tests.py'], shell=True)
os.chdir('../')

# Flu class
print('\n'*2)
ut.msg('- '*nSim + 'FLU CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('flu/')
subprocess.call(['python3.6 run_flu_tests.py'], shell=True)
os.chdir('../')

# Contributions class  
print('\n'*2)
ut.msg('- '*nSim + 'CONTRIBUTIONS CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('contributions/')
subprocess.call(['python3.6 run_contributions_tests.py'], shell=True)
os.chdir('../')

# Transitions classes 
print('\n'*2)
ut.msg('- '*nSim + 'REGULAR_TRANSITIONS CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('transitions/regular_transitions/')
subprocess.call(['python3.6 run_regular_transitions_tests.py'], shell=True)
os.chdir('../../')

print('\n'*2)
ut.msg('- '*nSim + 'HSP_EMPLOYEE_TRANSITIONS CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('transitions/hsp_employee_transitions/')
subprocess.call(['python3.6 run_hsp_employee_transitions_tests.py'], shell=True)
os.chdir('../../')

print('\n'*2)
ut.msg('- '*nSim + 'HSP_PATIENT_TRANSITIONS CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('transitions/hsp_patient_transitions/')
subprocess.call(['python3.6 run_hsp_patient_transitions_tests.py'], shell=True)
os.chdir('../../')

print('\n'*2)
ut.msg('- '*nSim + 'FLU_TRANSITIONS CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('transitions/flu_transitions/')
subprocess.call(['python3.6 run_flu_transitions_tests.py'], shell=True)
os.chdir('../../')

# ABM interface tests
print('\n'*2)
ut.msg('- '*nSim + 'ABM CLASS TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('abm/')
subprocess.call(['python3.6 run_abm_tests.py'], shell=True)
os.chdir('../')

# Integration tests
print('\n'*2)
ut.msg('- '*nSim + 'INTEGRATION TESTS' + ' -'*nSim, REVERSE+RED)
os.chdir('integration_tests/')
subprocess.call(['python3.6 run_integration_tests.py'], shell=True)
os.chdir('../')

print('\n')
