# Copyright 2021 Chris Johns (chrisj@rtems.org)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

from __future__ import print_function

import component

data = {
    #
    # RTEMS platform
    #
    'rtems': {
        'base': 'boost/rtems',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    #
    # Submodules
    #
    'algorithm': {
        'base': 'boost/algorithm',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'any': {
        'base': 'boost/any',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'array': {
        'base': 'boost/array',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'assert': {
        'base': 'boost/assert',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'bind': {
        'base': 'boost/bind',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'concept_check': {
        'base': 'boost/concept_check',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'config': {
        'base': 'boost/config',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'container': {
        'base': 'boost/container',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
        'sources': [
            'src/alloc_lib.c',
            'src/dlmalloc.cpp',
            'src/global_resource.cpp',
            'src/monotonic_buffer_resource.cpp',
            'src/pool_resource.cpp',
            'src/synchronized_pool_resource.cpp',
            'src/unsynchronized_pool_resource.cpp',
        ]
    },
    'container_hash': {
        'base': 'boost/container_hash',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'core': {
        'base': 'boost/core',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'detail': {
        'base': 'boost/detail',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
        'sources': []
    },
    'filesystem': {
        'base': 'boost/filesystem',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
        'sources': [
            'src/codecvt_error_category.cpp',
            'src/exception.cpp',
            'src/operations.cpp',
            'src/directory.cpp',
            'src/path.cpp',
            'src/path_traits.cpp',
            'src/portability.cpp',
            'src/unique_path.cpp',
            'src/utf8_codecvt_facet.cpp',
        ]
    },
    'foreach': {
        'base': 'boost/foreach',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'format': {
        'base': 'boost/format',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'function': {
        'base': 'boost/function',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'integer': {
        'base': 'boost/integer',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'intrusive': {
        'base': 'boost/intrusive',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'io': {
        'base': 'boost/io',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
        'sources': []
    },
    'iterator': {
        'base': 'boost/iterator',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'iterator': {
        'base': 'boost/iterator',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'lexical_cast': {
        'base': 'boost/lexical_cast',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'move': {
        'base': 'boost/move',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'mpl': {
        'base': 'boost/mpl',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'multi_index': {
        'base': 'boost/multi_index',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'numeric_conversion': {
        'base': 'boost/numeric_conversion',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'optional': {
        'base': 'boost/optional',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'predef': {
        'base': 'boost/predef',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'preprocessor': {
        'base': 'boost/preprocessor',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'program_options': {
        'base': 'boost/program_options',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
        'sources': [
            'src/cmdline.cpp',
            'src/config_file.cpp',
            'src/convert.cpp',
            'src/options_description.cpp',
            'src/parsers.cpp',
            'src/positional_options.cpp',
            'src/split.cpp',
            'src/utf8_codecvt_facet.cpp',
            'src/value_semantic.cpp',
            'src/variables_map.cpp',
            'src/winmain.cpp',
        ]
    },
    'property_tree': {
        'base': 'boost/property_tree',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'range': {
        'base': 'boost/range',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'serialization': {
        'base': 'boost/serialization',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
        'sources': [
            'src/archive_exception.cpp',
            'src/basic_archive.cpp',
            'src/basic_iarchive.cpp',
            'src/basic_iserializer.cpp',
            'src/basic_oarchive.cpp',
            'src/basic_oserializer.cpp',
            'src/basic_pointer_iserializer.cpp',
            'src/basic_pointer_oserializer.cpp',
            'src/basic_serializer_map.cpp',
            'src/basic_text_iprimitive.cpp',
            'src/basic_text_oprimitive.cpp',
            'src/basic_xml_archive.cpp',
            'src/binary_iarchive.cpp',
            'src/binary_oarchive.cpp',
            'src/codecvt_null.cpp',
            'src/extended_type_info.cpp',
            'src/extended_type_info_no_rtti.cpp',
            'src/extended_type_info_typeid.cpp',
            'src/polymorphic_binary_iarchive.cpp',
            'src/polymorphic_binary_oarchive.cpp',
            'src/polymorphic_iarchive.cpp',
            'src/polymorphic_oarchive.cpp',
            'src/polymorphic_text_iarchive.cpp',
            'src/polymorphic_text_oarchive.cpp',
            'src/polymorphic_xml_iarchive.cpp',
            'src/polymorphic_xml_oarchive.cpp',
            'src/stl_port.cpp',
            'src/text_iarchive.cpp',
            'src/text_oarchive.cpp',
            'src/utf8_codecvt_facet.cpp',
            'src/void_cast.cpp',
            'src/xml_archive_exception.cpp',
            'src/xml_grammar.cpp',
            'src/xml_iarchive.cpp',
            'src/xml_oarchive.cpp',
        ]
    },
    'smart_ptr': {
        'base': 'boost/smart_ptr',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'spirit': {
        'base': 'boost/spirit',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'static_assert': {
        'base': 'boost/static_assert',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'system': {
        'base': 'boost/system',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'throw_exception': {
        'base': 'boost/throw_exception',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'tokenizer': {
        'base': 'boost/tokenizer',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'tuple': {
        'base': 'boost/tuple',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'type_index': {
        'base': 'boost/type_index',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'type_traits': {
        'base': 'boost/type_traits',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'utility': {
        'base': 'boost/utility',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
    'uuid': {
        'base': 'boost/uuid',
        'headers': {
            'boost': [
                'include/boost'
            ],
        },
    },
}

class build(component.build):
    def __init__(self):
        super(build, self).__init__(data)

    def configure(self, conf, arch_bsp):
        conf.msg('Boost components', str(len(data)), 'YELLOW')

    def prep(self, bld, config):
        config['defines'] += [
            'BOOST_PLATFORM_CONFIG="boost/config/platform/rtems.hpp"',
        ]
        self.preinstall(bld, 'include', config)
