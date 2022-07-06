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

#define sizeof_section(sect, data) \
({ \
        size_t ret; \
        size_t data_size; \
        data_size = sect->m_count * sizeof(typeof(sect->data)); \
        ret = (sect) ? offsetof(typeof(*(sect)), data) + data_size : 0; \
        (ret); \
})

int zocl_get_sect(
  enum axlf_section_kind kind, const struct axlf *axlf, void* base, uint64_t* size) {
  void** base_ = NULL;
  uint64_t offset;
  uint64_t size_;
  int r;
  if (base != NULL) {
    base_ = (void**) base;
  }
  r = xrt_xclbin_section_info(axlf, kind, &offset, &size_);
  if (r < 0) {
    *base_ = NULL;
    if (size != NULL) {
      *size = 0;
    }
    return 0 - r;
  }
  if (base_ != NULL) {
    *base_ = ((uint8_t*) axlf) + offset;
  }
  if (size != NULL) {
    *size = size_;
  }
  return 0;
}

int zocl_get_slot_sections(
  const struct axlf* axlf, zocl_slot_sections* sections) {
  uint64_t size = 0;
  int r;
  memset(sections, 0, sizeof(*sections));
  r = zocl_get_sect(IP_LAYOUT, axlf, &sections->ip, &size);
  if (r != 0) {
    zocl_info("zocl: get-slot-sect: IP_LAYOUT not found\n");
    return r;
  }
  if (sizeof_section(sections->ip, m_ip_data) != size) {
    zocl_info("zocl: get-slot-sect: invalid IP_LAYOUT size\n");
    return EINVAL;
  }
  r = zocl_get_sect(DEBUG_IP_LAYOUT, axlf, &sections->debug_ip, &size);
  if (r != 0) {
    zocl_info("zocl: get-slot-sect: DEBUG_IP_LAYOUT not found\n");
    return r;
  }
  if (sizeof_section(sections->debug_ip, m_debug_ip_data) != size) {
    zocl_info("zocl: get-slot-sect: invalid DEBUG_IP_LAYOUT size\n");
    return EINVAL;
  }
  r = zocl_get_sect(AIE_METADATA, axlf, &sections->aie_data.data, &size);
  if (r != 0) {
    zocl_info("zocl: get-slot-sect: AIE_METADATA not found\n");
    return r;
  }
  sections->aie_data.size = size;
  r = zocl_get_sect(CONNECTIVITY, axlf, &sections->connectivity, &size);
  if (r != 0) {
    zocl_info("zocl: get-slot-sect: DEBUG_IP_LAYOUT not found\n");
    return r;
  }
  if (sizeof_section(sections->connectivity, m_connection) != size) {
    zocl_info("zocl: get-slot-sect: invalid CONNECTIVITY size\n");
    return EINVAL;
  }
  r = zocl_get_sect(MEM_TOPOLOGY, axlf, &sections->topology, &size);
  if (r != 0) {
    zocl_info("zocl: get-slot-sect: MEM_TOPOLOGY not found\n");
    return r;
  }
  if (sizeof_section(sections->topology, m_mem_data) != size) {
    zocl_info("zocl: get-slot-sect: invalid MEM_TOPOLOGY size\n");
    return EINVAL;
  }
  return 0;
}

int zocl_slot_sections_alloc(
  const struct axlf* axlf, zocl_slot_sections* sections) {
  zocl_slot_sections secs;
  void* mem;
  size_t size;
  int r;

  memset(sections, 0, sizeof(*sections));

  r = zocl_get_slot_sections(axlf, &secs);
  if (r != 0) {
    return r;
  }

  size =
    sizeof_section(secs.ip, m_ip_data) +
    sizeof_section(secs.debug_ip, m_debug_ip_data) +
    secs.aie_data.size +
    sizeof_section(secs.connectivity, m_connection) +
    sizeof_section(secs.topology, m_mem_data);

  mem = calloc(1, size);
  if (mem == NULL) {
    zocl_info("zocl: slot-sect-alloc: no memory\n");
    return ENOMEM;
  }

  size = sizeof_section(secs.ip, m_ip_data);
  sections->ip = mem;
  memcpy(sections->ip, secs.ip, size);
  mem += size;

  size = sizeof_section(secs.debug_ip, m_debug_ip_data);
  sections->debug_ip = mem;
  memcpy(sections->debug_ip, secs.debug_ip, size);
  mem += size;

  size = secs.aie_data.size;
  sections->aie_data.data = mem;
  memcpy(sections->aie_data.data, secs.aie_data.data, size);
  mem += size;

  size = sizeof_section(secs.connectivity, m_connection);
  sections->connectivity = mem;
  memcpy(sections->connectivity, secs.connectivity, size);
  mem += size;

  size = sizeof_section(secs.topology, m_mem_data);
  sections->topology = mem;
  memcpy(sections->topology, secs.topology, size);
  mem += size;

  return 0;
}

void zocl_slot_sections_free(zocl_slot_sections* sections) {
  free(sections->ip);
}

static struct addr_aperture* zocl_next_free_apt_index(zocl_dev* zocl) {
  int a;
  for (a = 0; a < MAX_CU_NUM; ++a) {
    if (zocl->cu_subdevs.apertures[a].addr == NULL) {
      return &zocl->cu_subdevs.apertures[a];
    }
  }
  return NULL;
}

static int zocl_update_apertures(zocl_dev* zocl, zocl_slot* slot) {
  int total = 0;
  int i;
  /*
   * xclbin doesn't contain IP size, hardcoding size for now
   */
  if (slot->sections.ip != NULL) {
    total += slot->sections.ip->m_count;
  }
  if (slot->sections.debug_ip != NULL) {
    total += slot->sections.debug_ip->m_count;
  }
  if (total == 0) {
    return 0;
  }
  /* If this happened, the xclbin is super bad */
  if (total < 0 || total > MAX_APT_NUM) {
    zocl_info("zocl: update-aperture: invalid number of apertures\n");
    return EINVAL;
  }
  if (slot->sections.ip != NULL) {
    for (i = 0; i < slot->sections.ip->m_count; ++i) {
      struct ip_data* ip = &slot->sections.ip->m_ip_data[i];
      struct addr_aperture* apt = zocl_next_free_apt_index(zocl);
      if (apt == NULL) {
        zocl_info("zocl: update-apt: no apertures free for ip\n");
        return ENOSPC;
      }
      apt->slot_idx = slot->slot_idx;
      apt->addr = (void*) ip->m_base_address;
      apt->size = CU_SIZE;
      apt->prop = ip->properties;
      apt->cu_idx = -1;
    }
  }
  if (slot->sections.debug_ip != NULL) {
    for (i = 0; i < slot->sections.debug_ip->m_count; ++i) {
      struct debug_ip_data* dip = &slot->sections.debug_ip->m_debug_ip_data[i];
      struct addr_aperture* apt = zocl_next_free_apt_index(zocl);
      if (apt == NULL) {
        zocl_info("zocl: update-apt: no apertures free for debug-ip\n");
        return ENOSPC;
      }
      apt->slot_idx = slot->slot_idx;
      apt->addr = (void*) dip->m_base_address;
      if (dip->m_type == AXI_MONITOR_FIFO_LITE ||
          dip->m_type == AXI_MONITOR_FIFO_FULL) {
        apt->size = SIZE_IN_K(8);
      } else {
        apt->size = SIZE_IN_K(64);
      }
    }
  }
  return 0;
}

int zocl_load_axlf(zocl_dev* zocl, struct drm_zocl_axlf* axlf_obj) {
  struct axlf* axlf;
  int slot_id = axlf_obj->za_slot_id;
  zocl_slot* slot = NULL;
  int sections;
  bool axlf_same;
  int r;

  if (slot_id > zocl->num_pr_slot) {
    zocl_info("zocl: load-axlf: slot out of range: %d\n", slot_id);
    return EINVAL;
  }

  slot = &zocl->slots[slot_id];

  slot->slot_idx = slot_id;

  axlf = axlf_obj->za_xclbin_ptr;

  if (memcmp(&axlf->m_magic, "xclbin2", 8) != 0) {
    zocl_info("zocl: load-axlf: xclbin magic is invalid\n");
    return EINVAL;
  }

  zocl_xclbin_report(axlf);

  if (axlf->m_header.m_mode != XCLBIN_FLAT) {
    zocl_info("zocl: load-axlf: invalid xclbin mode: %d\n", axlf->m_header.m_mode);
    return EINVAL;
  }

  axlf_same = uuid_compare(slot->uuid, axlf->m_header.uuid) == 0;

  if (axlf_same) {
    zocl_info("zocl: load-axlf: xclbin is already loaded\n");
    return EINVAL;
  }

  uuid_copy(slot->uuid, axlf->m_header.uuid);

  sections = axlf->m_header.m_numSections - 1;

  /*
   * Returns NULL is there are no AIE resources
   */
  r = zocl_get_sect(AIE_RESOURCES, axlf, NULL, NULL);
  if (r != 0) {
    return r;
  }

  /*
   * @todo If the same AXLF see if not forced and then if only AIE and
   *       load that.
   */
  r = zocl_slot_sections_alloc(axlf, &slot->sections);
  if (r != 0) {
    return r;
  }

  r = zocl_update_apertures(zocl, slot);
  if (r != 0) {
    zocl_slot_sections_free(&slot->sections);
    return r;
  }

  if (axlf_obj->za_ksize > 0) {
  }

  return EIO;
}
