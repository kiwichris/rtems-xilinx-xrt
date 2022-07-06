# Copyright 2021 Chris Johns (chrisj@rtems.org)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

from __future__ import print_function

import component

data = {
    'aiefal': {
        'base': 'embeddedsw/XilinxProcessorIPLib/drivers/aiefal',
        'headers': {
            'xaiefal': [
                'src/xaiefal.hpp',
            ],
            'xaiefal/common': [
                'src/common/*.hpp',
            ],
            'xaiefal/profile': [
                'src/profile/*.hpp',
            ],
            'xaiefal/rsc': [
                'src/rsc/*.hpp',
            ]
        },
        'sources': []
     },
    'xaiengine': {
        'base': 'embeddedsw/XilinxProcessorIPLib/drivers/aienginev2',
        'includes' : [
            'src/core',
            'src/device',
            'src/dma',
            'src/events',
            'src/io_backend/privilege',
            'src/interrupt',
            'src/locks',
            'src/npi',
            'src/pm',
            'src/rsc',
            'src/stream_switch',
        ],
        'headers': {
            '.': [
                'src/xaiengine.h',
            ],
            'xaiengine': [
                'src/common/xaie_helper.h',
                'src/core/xaie_core.h',
                'src/core/xaie_elfloader.h',
                'src/dma/xaie_dma.h',
                'src/events/xaie_events.h',
                'src/global/xaie_feature_config.h',
                'src/global/xaiegbl.h',
                'src/global/xaiegbl_defs.h',
                'src/global/xaiegbl_regdef.h',
                'src/interrupt/xaie_interrupt.h',
                'src/io_backend/ext/xlnx-ai-engine.h',
                'src/io_backend/xaie_io.h',
                'src/locks/xaie_locks.h',
                'src/memory/xaie_mem.h',
                'src/perfcnt/xaie_perfcnt.h',
                'src/pl/xaie_plif.h',
                'src/pm/xaie_clock.h',
                'src/pm/xaie_reset.h',
                'src/rsc/xaie_rsc.h',
                'src/stream_switch/xaie_ss.h',
                'src/timer/xaie_timer.h',
                'src/trace/xaie_trace.h',
            ]
        },
        'sources': [
            'src/common/xaie_helper.c',
            'src/core/xaie_core.c',
            'src/core/xaie_core_aie.c',
            'src/core/xaie_core_aieml.c',
            'src/core/xaie_elfloader.c',
            'src/device/xaie_device_aie.c',
            'src/device/xaie_device_aieml.c',
            'src/dma/xaie_dma.c',
            'src/dma/xaie_dma_aie.c',
            'src/dma/xaie_dma_aieml.c',
            'src/events/xaie_events.c',
            'src/global/xaiegbl.c',
            'src/global/xaiegbl_reginit.c',
            'src/global/xaiemlgbl_reginit.c',
            'src/interrupt/xaie_interrupt.c',
            'src/interrupt/xaie_interrupt_aie.c',
            'src/interrupt/xaie_interrupt_aieml.c',
            'src/io_backend/ext/xaie_baremetal.c',
            'src/io_backend/ext/xaie_cdo.c',
            'src/io_backend/ext/xaie_debug.c',
            'src/io_backend/ext/xaie_io_common.c',
            'src/io_backend/ext/xaie_linux.c',
            'src/io_backend/ext/xaie_metal.c',
            'src/io_backend/ext/xaie_sim.c',
            'src/io_backend/privilege/xaie_io_privilege.c',
            'src/io_backend/xaie_io.c',
            'src/locks/xaie_locks.c',
            'src/locks/xaie_locks_aie.c',
            'src/locks/xaie_locks_aieml.c',
            'src/memory/xaie_mem.c',
            'src/npi/xaie_npi.c',
            'src/npi/xaie_npi_aie.c',
            'src/npi/xaie_npi_aieml.c',
            'src/perfcnt/xaie_perfcnt.c',
            'src/pl/xaie_plif.c',
            'src/pm/xaie_clock.c',
            'src/pm/xaie_ecc.c',
            'src/pm/xaie_reset.c',
            'src/pm/xaie_reset_aie.c',
            'src/pm/xaie_reset_aieml.c',
            'src/pm/xaie_tilectrl.c',
            'src/rsc/xaie_rsc.c',
            'src/rsc/xaie_rsc_bcast.c',
            'src/rsc/xaie_rsc_events.c',
            'src/rsc/xaie_rsc_perfcnt.c',
            'src/rsc/xaie_rsc_trace.c',
            'src/stream_switch/xaie_ss.c',
            'src/stream_switch/xaie_ss_aie.c',
            'src/stream_switch/xaie_ss_aieml.c',
            'src/timer/xaie_timer.c',
            'src/trace/xaie_trace.c',
        ]
     }
}

class build(component.build):
    def __init__(self):
        super(build, self).__init__(data)

    def prep(self, bld, config):
        self.preinstall(bld, 'include', config)
        # https://github.com/Xilinx/XRT/issues/6610
        # Work around the issue
        config['includes'] += ['include/xaiengine']
