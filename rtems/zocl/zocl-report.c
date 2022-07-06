/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2022 Contemporary Software
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdarg.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <rtems/zocl/zocl.h>

#include "zocl-private.h"
#include "zocl-trace.h"

static const char* mem_type_labels[] = {
  "MEM_DDR3",
  "MEM_DDR4",
  "MEM_DRAM",
  "MEM_STREAMING",
  "MEM_PREALLOCATED_GLOB",
  "MEM_ARE (Aurora)",
  "MEM_HBM",
  "MEM_BRAM",
  "MEM_URAM",
  "MEM_STREAMING_CONNECTION",
  "MEM_HOST",
  "MEM_PS_KERNEL",
};

static const char* ip_type_labels[] = {
  "IP_MB",
  "IP_KERNEL",
  "IP_DNASC",
  "IP_DDR4_CONTROLLER",
  "IP_MEM_DDR4",
  "IP_MEM_HBM",
  "IP_MEM_HBM_ECC",
  "IP_PS_KERNEL",
};

#define NUMOF(_a) (sizeof(_a) / sizeof(_a[0]))

#define LABEL(_type, _labels) _type < NUMOF(_labels) ? _labels[_type] : "invalid"

void zocl_xclbin_report(const struct axlf* axlf) {
  zocl_slot_sections secs;
  time_t time;
  char buf[64];
  int line_len;
  char last_char;
  int indent;
  int i;
  zocl_info("XCLBIN: @ %p\n", axlf);
  zocl_info(" m_length              : %" PRIu64 "\n", axlf->m_header.m_length);
  time = axlf->m_header.m_timeStamp;
  ctime_r(&time, buf);
  zocl_info(" m_timeStamp           : %s", buf);
  time = axlf->m_header.m_featureRomTimeStamp;
  ctime_r(&time, buf);
  zocl_info(" m_featureRomTimeStamp : %s", buf);
  zocl_info(" m_versionPatch        : %" PRIu16 "\n", axlf->m_header.m_versionPatch);
  zocl_info(" m_versionMajor        : %" PRIu8 "\n", axlf->m_header.m_versionMajor);
  zocl_info(" m_versionMinor        : %" PRIu8 "\n", axlf->m_header.m_versionMinor);
  zocl_info(" m_mode                : %" PRIx16 "\n", axlf->m_header.m_mode);
  zocl_info(" m_actionMask          : %" PRIx16 "\n", axlf->m_header.m_actionMask);
  uuid_unparse(axlf->m_header.uuid, buf);
  zocl_info(" m_uuid                : %s\n", buf);
  zocl_info(" m_debug_bin           : %s\n", axlf->m_header.m_debug_bin);
  zocl_info(" m_numSections         : %" PRIu32 "\n", axlf->m_header.m_numSections);
  for (i = 0; i < axlf->m_header.m_numSections; ++i) {
    const struct axlf_section_header* sect = &axlf->m_sections[i];
    zocl_info(
      "  %3d : %-25s %-9" PRIu64 " %-9" PRIu64 " %s\n", i,
      xrt_xclbin_kind_to_string(sect->m_sectionKind),
      sect->m_sectionOffset, sect->m_sectionSize, sect->m_sectionName);
  }
  i = zocl_get_slot_sections(axlf, &secs);
  if (i == 0) {
    /* topology */
    zocl_info(" topology : %" PRIu32 "\n", secs.topology->m_count);
    for (i = 0; i < secs.topology->m_count; ++i) {
      struct mem_data* mem = &secs.topology->m_mem_data[i];
      zocl_info(
        "  %3d : %-26s used=%s ", i,
        LABEL(mem->m_type, mem_type_labels), mem->m_used ? "used  " : "unused");
      switch (mem->m_type) {
        case MEM_DDR3:
        case MEM_DDR4:
        case MEM_DRAM:
          zocl_info("addr=%016" PRIx64 " size=%" PRIu64 "K ",
                    mem->m_base_address, mem->m_size);
          break;
        case MEM_STREAMING:
          zocl_info("route-id=%-4" PRIu64 " flow-id=%-4" PRIu64 " ",
                    mem->route_id, mem->flow_id);
          break;
        default:
          break;
      }
      zocl_info("%s\n", mem->m_tag);
    }
    /* ip-layout */
    zocl_info(" ip-layout : %" PRIu32 "\n", secs.ip->m_count);
    for (i = 0; i < secs.ip->m_count; ++i) {
      struct ip_data* ip_data = &secs.ip->m_ip_data[i];
      zocl_info(
        "  %3d : %-20s addr=0x%016" PRIx64 " ", i,
        LABEL(ip_data->m_type, ip_type_labels), ip_data->m_base_address);
      switch (ip_data->m_type) {
        case IP_KERNEL:
          zocl_info("prop=0x%08" PRIx32 " ", ip_data->properties);
          break;
        case IP_MEM_DDR4:
        case IP_MEM_HBM:
        case IP_MEM_HBM_ECC:
          zocl_info(
            "idx=%" PRIu16 " pc-idx=%" PRIu8 " ", ip_data->indices.m_index,
            ip_data->indices.m_pc_index);
          break;
        default:
          break;
      }
      zocl_info("%s\n", ip_data->m_name);
    }
    /* connectivity */
    zocl_info(" connectivity : %" PRIu32 "\n", secs.connectivity->m_count);
    for (i = 0; i < secs.connectivity->m_count; ++i) {
      struct connection* conn = &secs.connectivity->m_connection[i];
      zocl_info(
        "  %3d : arg-idx=%-4" PRIi32 " ip-idx=%-4" PRIi32 " mem-idx=%-4" PRIi32 "\n",
        i, conn->arg_index, conn->m_ip_layout_index, conn->mem_data_index);
    }
    /* aie metadata */
    zocl_info(" aie-metadata : size=%zu\n", secs.aie_data.size);
    indent = 2;
    last_char = '\n';
    for (i = 0; i < secs.aie_data.size; ++i) {
      const char* p = ((const char*) secs.aie_data.data) + i;
      bool print_lf = false;
      if (last_char == '\n') {
        line_len = zocl_info("%*c", indent, ' ');
      }
      switch (*p) {
        case '{':
          print_lf = true;
          ++indent;
          break;
        case '}':
          line_len = zocl_info("\n%*c", indent - 1, ' ');
          if (i < secs.aie_data.size - 1 && p[1] == '}') {
            --indent;
          }
          break;
        case ',':
          if (last_char == '}' || line_len > 70) {
            print_lf = true;
            if (last_char == '}') {
              --indent;
            }
          }
          break;
        default:
          break;
      }
      if (print_lf) {
        zocl_info("%c\n", *p);
        last_char = '\n';
      } else {
        ++line_len;
        zocl_info("%c", *p);
        last_char = *p;
      }
    }
  }

}
