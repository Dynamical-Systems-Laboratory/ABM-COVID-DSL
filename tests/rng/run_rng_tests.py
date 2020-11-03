import subprocess

import sys
py_path = '../../scripts/'
sys.path.insert(0, py_path)

import utils as ut
from colors import *

#
# Compile and run all the RNG class specific tests
#

# Compile
subprocess.call(['python3.6 compilation.py'], shell=True)

# Test suite 1
ut.msg('RNG class functionality tests', CYAN)
subprocess.call(['./rng_test'], shell=True)


