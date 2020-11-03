import subprocess, glob, os

#
# Input 
#

# Compiler options
cx = 'g++'
std = '-std=c++11'
opt = '-O0'
tst_files = '../common/test_utils.cpp'

#
# Tests
#

# Test 1
# Correctness of generated random distribution 
# Name of the executable
exe_name = 'rng_test'
# Files needed only for this build
spec_files = 'rng_tests.cpp '
compile_com = ' '.join([cx, std, opt, '-o', exe_name, spec_files, tst_files])
subprocess.call([compile_com], shell=True)


