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
src_files = path + 'infection.cpp' 
src_files += ' ' + path + 'agent.cpp'
src_files += ' ' + path + 'places/place.cpp'
src_files += ' ' + path + 'places/household.cpp'
src_files += ' ' + path + 'utils.cpp'
src_files += ' ' + path + 'io_operations/FileHandler.cpp'
tst_files = '../common/test_utils.cpp'

#
# Tests
#

# Test 1
# Functionality of Agent class
# Name of the executable
exe_name = 'inf_test'
# Files needed only for this build
spec_files = 'infection_test.cpp '
compile_com = ' '.join([cx, std, opt, '-o', exe_name, spec_files, tst_files, src_files])
subprocess.call([compile_com], shell=True)


