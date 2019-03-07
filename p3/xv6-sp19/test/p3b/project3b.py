import sys, os, inspect

import toolspath
from testing import Xv6Test, Xv6Build

curdir = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
def get_description(name):
  cfile = os.path.join(curdir, 'ctests', name+'.c')
  with open(cfile, 'r') as f:
    desc = f.readline()
  desc = desc.strip()
  desc = desc[2:]
  if desc[-2:] == '*/':
    desc = desc[:-2]
  return desc.strip()

all_tests = []
build_test = Xv6Build
for testname in '''null null2 null3 null4 bounds
                   stack bounds2 bounds_str heap
                   stack2 bounds3 stack3 stack4 stack5
                   shmem_invalidpg
                   shmem_access_communication shmem_fork
                   shmem_access_exec2 
                   shmem_access_exec shmem_bound shmem_rw
                   shmem_access_persistent
                   '''.split():
  members = {
      'name': testname,
      'tester': 'ctests/' + testname + '.c',
      'description': get_description(testname),
      'timeout': 5 if testname != 'usertests' else 240,
      'point_value': 10 if testname in {
          'null', 'null2', 'bounds', 'stack', 'bounds_str', 'heap'
          } else {'bounds3': 3, 'stack3': 2}.get(testname, 5)
      }
  newclass = type(testname, (Xv6Test,), members)
  all_tests.append(newclass)
  setattr(sys.modules[__name__], testname, newclass)

from testing.runtests import main
main(build_test, all_tests)
