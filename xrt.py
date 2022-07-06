# Copyright 2021 Chris Johns (chrisj@rtems.org)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

from __future__ import print_function

import os.path

import rtems_waf.rtems as rtems
import component

class build(component.build):

    def version(self, ctx):
        major = 0
        minor = 0
        try:
            lines = open('xrt/src/CMakeLists.txt').readlines()
        except:
            ctx.fatal('XRT verison read failed')
        for line in lines:
            if 'XRT_VERSION_MAJOR' in line:
                ls = line[:-1].split()
                if len(ls) == 2:
                    num = ls[1][:-1]
                    if num.isdigit():
                        major = int(num)
            elif 'XRT_VERSION_MINOR' in line:
                ls = line[:-1].split()
                if len(ls) == 2:
                    num = ls[1][:-1]
                    if num.isdigit():
                        minor = int(num)
        return major, minor

    def configure(self, conf, arch_bsp):
        if rtems.arch(arch_bsp) != 'aarch64':
            conf.fatal('XRT only supports the aarch64 architecture')
        xrt_major, xrt_minor = self.version(conf)
        if xrt_major == 0 or xrt_minor == 0:
            conf.fatal('cannot find XRT major/minor version numbers')
        conf.env.XRT_MAJOR = xrt_major
        conf.env.XRT_MINOR = xrt_minor
        conf.env.XRT_VERSION = '%d.%d' % (xrt_major, xrt_minor)
        conf.msg('XTR Version', conf.env.XRT_VERSION, 'YELLOW')

    def build_version(self, bld):
        version_h_in = 'xrt/src/CMake/config/version.h.in'
        version_h = ['include/version.h', 'include/gen/version.h']
        if not os.path.exists(version_h_in):
            bld.fatal('version.h template not found')
        tags = [
            ('XRT_VERSION_STRING', bld.env.XRT_VERSION),
            ('XRT_BRANCH', 'n\/a'),
            ('XRT_HASH', 'n\/a'),
            ('XRT_HASH_DATE', 'n\/a'),
            ('XRT_DATE_RFC', 'n\/a'),
            ('XRT_DATE', 'n\/a'),
            ('XRT_MODIFIED_FILES', 'n\/a'),
            ('XRT_VERSION_MAJOR', bld.env.XRT_MAJOR),
            ('XRT_VERSION_MINOR', bld.env.XRT_MINOR)
        ]
        sed = 'sed '
        for tag in tags:
            sed += "-e 's/@%s@/%s/' " % (tag[0], tag[1])
        for v_h in version_h:
            bld(target=v_h,
                source=version_h_in,
                rule=sed + " < ${SRC} > ${TGT}",
                update_outputs=True)

    def core_common(self, bld, base, parent, config):
        sources = [
            'config_reader.cpp',
            'debug.cpp',
            'debug_ip.cpp',
            'device.cpp',
            'error.cpp',
            'info_*.cpp',
            'message.cpp',
            'module_loader.cpp',
            'query_requests.cpp',
            'scheduler.cpp',
            'sensor.cpp',
            'system.cpp',
            'thread.cpp',
            'time.cpp',
            'utils.cpp',
            'xclbin_parser.cpp',
            'xclbin_swemu.cpp',
            'api/*.cpp',
            'drv/xrt_xclbin.c'
        ]
        target, path = self.node(base, 'common', parent, 'common')
        config['includes'] += [os.path.join(path, 'drv', 'include')]
        self.objects(bld, path, target, sources, config)
        return [target]

    def core_edge_rtems(self, bld, base, parent, config, local):
        sources = [
            'user/aie_sys_parser.cpp',
            'user/device_rtems.cpp',
            'user/plugin/xdp/*.cpp',
            'user/shim.cpp',
            'user/system_rtems.cpp',
            'user/zynq_dev.cpp'
        ]
        target, path = self.node(base, '', parent, 'rtems')
        self.objects(bld, path, target, sources, config, local)
        return [target]

    def core_edge_common(self, bld, base, parent, config, local):
        config['defines'] += [
            'XRT_ENABLE_AIE=1',
            'XCLHAL_MAJOR_VER=1',
            'XCLHAL_MINOR_VER=0'
        ]
        sources = [
            'common/*.cpp',
            'user/aie/*.cpp',
            'user/aie/common_layer/*.cpp'
        ]
        target, path = self.node(base, '', parent, 'common')
        self.objects(bld, path, target, sources, config, local)
        return [target]

    def core_edge(self, bld, base, parent, config):
        local = {
        }
        targets = []
        node, path = self.node(base, 'edge', parent, 'edge')
        targets += self.core_edge_common(bld, path, node, config, local)
        targets += self.core_edge_rtems(bld, path, node, config, local)
        return targets

    def core_tools(self, bld, base, parent, config):
        local = {
            'defines': [
                'main_=xbutil_main_',
                'NO_BOOST_PROCESS=1',
            ],
            'includes': [
                base,
            ]
        }
        sources = [
            'common/*.cpp',
            'xbutil2/*.cpp'
        ]
        target, path = self.node(base, 'tools', parent, 'tools')
        self.objects(bld, path, target, sources, config, local)
        return [target]

    def core_prep(self, bld, base, parent, config):
        node, path = self.node(base, 'core', parent, 'core')
        config['includes'] += [os.path.join(path, 'include')]

    def core(self, bld, base, parent, config):
        targets = []
        node, path = self.node(base, 'core', parent, 'core')
        targets += self.core_common(bld, path, node, config)
        targets += self.core_edge(bld, path, node, config)
        targets += self.core_tools(bld, path, node, config)
        return targets

    def runtime_src_prep(self, bld, base, parent, config):
        node, path = self.node(base, 'runtime_src', parent, 'rt')
        self.core_prep(bld, path, node, config)

    def runtime_src(self, bld, base, parent, config):
        targets = []
        node, path = self.node(base, 'runtime_src', parent, 'rt')
        config['includes'] += [path, 'libdrm']
        targets += self.core(bld, path, node, config)
        return targets

    def prep(self, bld, config):
        self.build_version(bld)
        self.runtime_src_prep(bld, 'xrt/src', 'xrt', config)
        config['defines'] += [
            'XRT_VERSION_MAJOR="%d"' % (bld.env.XRT_MAJOR),
            '_GNU_SOURCE',
        ]

    def build(self, bld, config):
        return self.runtime_src(bld, 'xrt/src', 'xrt', config)
