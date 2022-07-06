# Copyright 2021 Chris Johns (chrisj@rtems.org)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

from __future__ import print_function

import component

data = {
    'drm': {
        'base': 'mesa-drm',
        'headers': {
            'libdrm': [
                'include/drm/drm.h',
                'include/drm/drm_mode.h',
            ],
        },
        'sources': []
     },
}

class build(component.build):
    def __init__(self):
        super(build, self).__init__(data)

    def prep(self, bld, config):
        self.preinstall(bld, 'include', config)
