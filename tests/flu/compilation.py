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
src_files = path + 'flu.cpp'
src_files += ' ' +path + 'testing.cpp'
src_files += ' ' + path + 'utils.cpp'
src_files += ' ' + path + 'io_operations/FileHandler.cpp'
src_files += ' ' + path + 'io_operations/load_parameters.cpp'
tst_files = '../common/test_utils.cpp'

#
# Tests
#

# Initialization and construction 
# Basic functionality 
exe_name = 'flu_test'
# Files needed only for this build
spec_files = 'flu_functionality_test.cpp '
compile_com = ' '.join([cx, std, opt, '-o', exe_name, spec_files, tst_files, src_files])
subprocess.call([compile_com], shell=True)

