import subprocess

import sys
py_path = '../../../scripts/'
sys.path.insert(0, py_path)

import utils as ut
from colors import *

#
# Compile and run all the abm class specific tests
#

# Compile
subprocess.call(['python3.6 compilation.py'], shell=True)

ut.msg('FluTransitions class tests', CYAN)
subprocess.call(['./flu_tr_test'], shell=True)


