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

#ifndef RTEMS_ZOCL_ZOCL_PRIVATE_H
#define RTEMS_ZOCL_ZOCL_PRIVATE_H

#include <stdbool.h>
#include <inttypes.h>

#include <xclbin.h>
#include <xrt_xclbin.h>

#include <zynq_ioctl.h>

#include <rtems.h>
#include <rtems/imfs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE_IN_K(_ks) ((_ks) * 1024)

#ifndef ZOCL_MAX_SLOTS
#define ZOCL_MAX_SLOTS 4
#endif

#define MAX_CU_NUM  128
#define MAX_APT_NUM (2 * MAX_CU_NUM)

#define CU_SIZE SIZE_IN_K(64)

struct addr_aperture {
  void* addr;
  size_t size;
  uint32_t prop;
  int cu_idx;
  uint32_t slot_idx;
};

struct cu_subdev {
  unsigned int cu_num;
  unsigned int irq[MAX_CU_NUM];
   struct addr_aperture apertures[MAX_CU_NUM];
   unsigned int num_apts;
   //struct mutex lock;
};

struct aie_metadata {
  size_t size;
  void *data;
};

typedef struct {
  struct ip_layout* ip;
  struct debug_ip_layout* debug_ip;
  struct aie_metadata aie_data;
  struct connectivity* connectivity;
  struct mem_topology* topology;
} zocl_slot_sections;

typedef struct {
  int refs;
  int slot_idx;
  uuid_t uuid;
  zocl_slot_sections sections;
} zocl_slot;

typedef struct {
  rtems_mutex lock;
  int num_pr_slot;
  zocl_slot slots[ZOCL_MAX_SLOTS];
  struct cu_subdev cu_subdevs;
} zocl_dev;

static inline zocl_dev* zocl_get(rtems_libio_t *iop) {
  return IMFS_generic_get_context_by_iop(iop);
}

void zocl_xclbin_report(const struct axlf *axlf);

int zocl_request(zocl_dev* zocl, struct drm_zocl_request* req);

int zocl_load_axlf(zocl_dev* zocl, struct drm_zocl_axlf* axlf_obj);

int zocl_get_sect(
  enum axlf_section_kind kind, const struct axlf *axlf, void* base,
  uint64_t* size);
int zocl_get_slot_sections(
  const struct axlf* axlf_obj, zocl_slot_sections* sections);
int zocl_slot_sections_alloc(
  const struct axlf* axlf, zocl_slot_sections* sections);
void zocl_slot_sections_free(zocl_slot_sections* sections);

#ifdef __cplusplus
}
#endif

#endif /* RTEMS_ZOCL_ZOCL_PRIVATE_H */
