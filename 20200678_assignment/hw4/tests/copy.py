import os
import shutil

LIST = [
    'add_test01',
    'add_test02',
    'add_test05',
    'add_test09',
    'add_test10',
    'sub_test01',
    'c_test01',
    'div_test01',
    'div_test09',
    'd_test01',
    'f_test01',
    'mod_test01',
    'mod_test06',
    'mul_test01',
    'pow_test01',
    'pow_test02',
    'p_test01',
    'q_test01',
    'r_test01',
    'integration_test01',
    'integration_test02',
    'integration_test03',
    ]

for c in LIST:
    input = '../../input/%s.txt' % c
    stdout = '../../answer/%s.stdout' % c
    stderr = '../../answer/%s.stderr' % c

    files = [input, stdout, stderr]
    assert(all([os.path.exists(f) for f in files]))

    shutil.copy(input, './input/%s.txt' % c)
    shutil.copy(stdout, './answer/%s.stdout' % c)
    shutil.copy(stderr, './answer/%s.stderr' % c)
