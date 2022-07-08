# Copyright 2022 Chris Johns (chrisj@rtems.org)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

from __future__ import print_function

# See README.waf for building instructions.

try:
    import rtems_waf.rtems as rtems
except:
    print('error: no rtems_waf git submodule; run git submodule init')
    import sys
    sys.exit(1)

rtems_version = "6"

config = {
    'rtems' : {
        'version': rtems_version
    },
    'defines': [],
    'includes': ['include'],
    'comflags': ['-g', '-O'],
    'cflags': [],
    'cxxflags': ['-std=c++20']
}

import xrt
import boost
import embeddedsw
import mesadrm
import rtems_zocl

components = [
    xrt.build(),
    boost.build(),
    embeddedsw.build(),
    mesadrm.build(),
    rtems_zocl.build(),
]

def init(ctx):
    rtems.init(ctx, version=rtems_version, long_commands=True)
    for c in components:
        c.init(ctx)

def bsp_configure(conf, arch_bsp):
    for c in components:
        c.configure(conf, arch_bsp)

def options(opt):
    rtems.options(opt)
    for c in components:
        c.options(opt)

def configure(conf):
    rtems.configure(conf, bsp_configure=bsp_configure)

def build(bld):
    rtems.build(bld)
    for c in components:
        c.prep(bld, config)
    bld.add_group()
    targets = [t  for c in components for t in c.build(bld, config)]
    if len(targets) != len(set(targets)):
        import collections
        bld.fatal('target name repeated: %r' %
                  ([t for t,c in collections.Counter(targets).items() if c >  1]))
    lib_path = '${PREFIX}/' + \
        rtems.arch_bsp_lib_path(bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP)
    bld.stlib(target='xrt',
              features='c cxx cstlib',
              use=targets,
              install_path=lib_path)
    bld.recurse('tests')

def rebuild(ctx):
    import waflib.Options
    waflib.Options.commands.extend(['clean', 'build'])
