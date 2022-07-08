# Copyright 2021 Chris Johns (chrisj@rtems.org)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

from __future__ import print_function

import component

data = {
    'rtems': {
        'base': 'rtems',
        'headers': {
            'drv': [
                'zocl/zocl.h',
            ],
        },
        'includes': [
            '..',
            '../xrt/src/runtime_src/core/edge/include',
        ],
        'cflags': ['-Wall'],
        'sources': [
            'zocl/zocl.c',
            'zocl/zocl-report.c',
            'zocl/zocl-requests.c',
            'zocl/zocl-xclbin.c',
        ],
        'install': {
            'rtems/zocl': [
                'zocl/zocl.h',
            ]
        }
     },
    'smc': {
        'base': 'rtems',
        'includes': [
            '.',
        ],
        'sources': [
            'smc/smccc.c',
            'smc/smccc_arm64.S',
        ],
        'install': {
            'smc': [
                'smc/smccc.h',
            ]
        }
     },
    'pm': {
        'base': 'rtems',
        'includes': [
            '.',
        ],
        'cflags': ['-Wall'],
        'sources': [
            'pm/pm.c',
            'pm/pm-image.c',
            'pm/pm-shell.c',
        ],
        'install': {
            'rtems/pm': [
                'pm/pm.h',
                'pm/pm-image.h',
            ]
        }
     },
}

class build(component.build):
    def __init__(self):
        super(build, self).__init__(data)
