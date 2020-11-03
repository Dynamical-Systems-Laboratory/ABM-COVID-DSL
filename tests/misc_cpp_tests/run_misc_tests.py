import subprocess

import sys
py_path = '../../scripts/'
sys.path.insert(0, py_path)

import utils as ut
from colors import *

#
# Compile and run all the tests from misc cpp category
#

# Compile
subprocess.call(['python3.6 compilation.py'], shell=True)

# ABM_IO class
ut.msg('ABM_IO class', CYAN)
subprocess.call(['./abm_io_tests'], shell=True)

# FileHandler class
ut.msg('FileHandler class', CYAN)
subprocess.call(['./file_hdl_tests'], shell=True)

# Utils tests
ut.msg('Utils', CYAN)
subprocess.call(['./utils_tests'], shell=True)

# LoadParameters class
ut.msg('LoadParameters class', CYAN)
subprocess.call(['./ld_params_tests'], shell=True)
