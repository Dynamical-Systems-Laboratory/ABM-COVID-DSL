import subprocess, glob, os

#
# Input 
#

# Path to the main directory
path = '../../src/'
# Compiler options
cx = 'g++'
std = '-std=c++11'
opt = '-O0'
# Common source files
src_files = path + 'agent.cpp' 
src_files += ' ' + path + 'utils.cpp'
src_files += ' ' + path + 'io_operations/FileHandler.cpp'
tst_files = '../common/test_utils.cpp'

#
# Tests
#

# Test 1
# Functionality of Agent class
# Name of the executable
exe_name = 'agent_test'
# Files needed only for this build
spec_files = 'agent_test.cpp '
compile_com = ' '.join([cx, std, opt, '-o', exe_name, spec_files, tst_files, src_files])
subprocess.call([compile_com], shell=True)

# Test 2
# Agent class - additional states
# Name of the executable
exe_name = 'agent_states_test'
# Files needed only for this build
spec_files = 'agent_states_test.cpp '
compile_com = ' '.join([cx, std, opt, '-o', exe_name, spec_files, tst_files, src_files])
subprocess.call([compile_com], shell=True)


