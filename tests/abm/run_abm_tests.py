import subprocess

import sys
py_path = '../../scripts/'
sys.path.insert(0, py_path)

import utils as ut
from colors import *

#
# Compile and run all the abm class specific tests
#

# Compile
subprocess.call(['python3.6 compilation.py'], shell=True)

# Test suite 1
ut.msg('ABM interface - construction test', CYAN)
subprocess.call(['./con_test'], shell=True)

# Test suite 2
ut.msg('ABM interface - infection transmission test', CYAN)
subprocess.call(['./trans_inf_test'], shell=True)
