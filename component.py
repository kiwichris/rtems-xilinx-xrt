# Copyright 2021 Chris Johns (chrisj@rtems.org)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

from __future__ import print_function

import os.path
import re
import shutil

import rtems_waf.rtems as rtems

class build(object):
    def __init__(self, data=None):
        self.data = data

    def make_sources(self, ctx, base, sources):
        expanded_sources = []
        for src in sources:
            expanded_sources += ctx.path.ant_glob(os.path.join(base, src))
        return expanded_sources

    def node(self, base, path, parent, name):
        return parent + '-' + name, os.path.join(base, path)

    def objects(self, bld, base, target, sources, config, local = None):
        defines = config['defines']
        includes = config['includes']
        cflags = config['cflags'] + config['comflags']
        cxxflags = config['cxxflags'] + config['comflags']
        if local is not None:
            if 'defines' in local:
                defines += local['defines']
            if 'includes' in local:
                includes += local['includes']
            if 'cflags' in local:
                cflags += local['cflags']
            if 'cxxflags' in local:
                cxxflags += local['cxxflags']
        bld.objects(target=target,
                    features='asm c cxx',
                    source=self.make_sources(bld, base, sources),
                    defines=defines,
                    includes=includes,
                    cflags=cflags,
                    cxxflags=cxxflags)

    def init(self, ctx):
        pass

    def configure(self, conf, arch_bsp):
        pass

    def options(self, opt):
        pass

    def preinstall(self, bld, include, config):
        def rule(task):
            if len(task.inputs) != len(task.outputs):
                raise IndexError('task input and output lengths are not the same')
            for i in range(0, len(task.inputs)):
                shutil.copy2(task.inputs[i].abspath(), task.outputs[i].abspath())
            return False
        inc_node = bld.path.find_or_declare(include)
        for pkg in self.data:
            package = self.data[pkg]
            base_node = bld.path.find_node(package['base'])
            if 'headers' in package:
                for group in package['headers']:
                    if group == '.':
                        grp_node = inc_node
                    else:
                        grp_node = inc_node.find_or_declare(group)
                    for hdrs in package['headers'][group]:
                        inode = base_node.find_node(hdrs)
                        inode_isdir = inode is not None and inode.isdir()
                        leader = None
                        if inode_isdir:
                            headers = inode.ant_glob('**/*')
                            leader = re.compile(r'.*' + grp_node.bldpath().replace('/', '\/') + '\/')
                        else:
                            headers = base_node.ant_glob(hdrs)
                        if len(headers) == 0:
                            bld.fatal('%s header(s) not found: %s' % (pkg, hdrs))
                        targets = []
                        for header in headers:
                            if inode_isdir:
                                name = leader.split(header.abspath())[1]
                            else:
                                name = header.path_from(header.parent)
                            targets += [grp_node.find_or_declare(name)]
                        grp_node.mkdir()
                        bld(target=targets,
                            source=headers,
                            rule=rule)

    def prep(self, bld, config):
        pass

    def build(self, bld, config):
        targets = []
        for grp in self.data:
            group = self.data[grp]
            gnode = bld.path.find_node(group['base'])
            if 'sources' in group and len(group['sources']) != 0:
                local = {}
                if 'defines' in group:
                    local['defines'] = group['defines']
                if 'includes' in group:
                    local['includes'] = [gnode.find_node(inc) for inc in group['includes']]
                if 'cflags' in group:
                    local['cflags'] = group['cflags']
                if 'cxxflags' in group:
                    local['cxxflags'] = group['cxxflags']
                self.objects(bld, group['base'], grp, group['sources'], config, local)
                targets += [grp]
            if 'install' in group and len(group['install']) != 0:
                for install_point in group['install']:
                    install_to = os.path.join(
                        '${PREFIX}',
                        rtems.arch_bsp_include_path(
                            bld.env.RTEMS_VERSION, bld.env.RTEMS_ARCH_BSP),
                        install_point)
                    files = [gnode.find_node(f) for f in group['install'][install_point]]
                    bld.install_files(install_to, files)
        return targets
