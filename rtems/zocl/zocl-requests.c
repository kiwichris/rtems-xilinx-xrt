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

/*
 * Request handler.
 */
typedef int (*zocl_req_handler)(zocl_dev* zocl, struct drm_zocl_request* req);
typedef struct {
  const char* req;
  zocl_req_handler handler;
} zocl_req_handlers;

static uint32_t zocl_req_length(const struct drm_zocl_request* req) {
  return req->data_size - req->data_level;
}

static char* zocl_req_data(const struct drm_zocl_request* req) {
  return &req->data[req->data_level];
}

static size_t zocl_req_write(
  struct drm_zocl_request* req, const void* data, size_t size) {
  uint32_t len = zocl_req_length(req);
  if (len > 0) {
    len = size < len ? size : len;
    memcpy(zocl_req_data(req), data, len);
    req->data_level += len;
  }
  return len;
}

static int zocl_req_print(struct drm_zocl_request* req, const char* format, ...) {
  uint32_t len = zocl_req_length(req);
  if (len > 0) {
    va_list ap;
    va_start(ap, format);
    len = vsnprintf(zocl_req_data(req), len, format, ap);
    req->data_level += len;
  }
  return len;
}

static int zocl_xclbinid(zocl_dev* zocl, struct drm_zocl_request* req) {
  int s;
  for (s = 0; s < zocl->num_pr_slot; ++s) {
    zocl_slot* slot = &zocl->slots[s];
    if (slot->slot_idx >= 0) {
      char buf[32];
      uint32_t len;
      uuid_unparse(slot->uuid, buf);
      len = zocl_req_print(req, "%d %s\n", slot->slot_idx, buf);
      if (len == 0) {
        return EFBIG;
      }
    }
  }
  return 0;
}

static int zocl_kds_custat_raw(zocl_dev* zocl, struct drm_zocl_request* req) {
  return 0;
}

static zocl_req_handlers req_handlers[] = {
  { "xclbinid", zocl_xclbinid },
  { "kds_custat_raw", zocl_kds_custat_raw },
};

#define ZOCL_REQ_NUMOF (sizeof(req_handlers) / sizeof(req_handlers[0]))

int zocl_request(zocl_dev* zocl, struct drm_zocl_request* req) {
  int h;
  memset(req->data, 0, req->data_size);
  for (h = 0; h < ZOCL_REQ_NUMOF; ++h) {
    if (strcmp(req_handlers[h].req, req->req_type) == 0) {
      return req_handlers[h].handler(zocl, req);
    }
  }
  zocl_info("zocl: request: invalid request: %s\n", req->req_type);
  return EINVAL;
}
