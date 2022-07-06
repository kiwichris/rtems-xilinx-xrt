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

#include <stddef.h>
#include <string.h>

#include <rtems/zocl/zocl.h>

#include "zocl-private.h"
#include "zocl-trace.h"

int rtems_zocl_trace;

static zocl_dev* zocl_dev_init(void) {
  zocl_dev *zocl;
  int s;
  zocl = malloc(sizeof(*zocl));
  if (zocl == NULL) {
    return NULL;
  }
  memset(zocl, 0, sizeof(*zocl));
  rtems_mutex_init(&zocl->lock, "zocl");
  zocl->num_pr_slot = ZOCL_MAX_SLOTS;
  for (s = 0; s < zocl->num_pr_slot; ++s) {
    zocl->slots[s].slot_idx = -1;
  }
  return zocl;
}

static int zocl_version(struct drm_version* version) {
  version->version_major = 1;
  version->version_minor = 0;
  version->version_patchlevel = 0;
  strlcpy(version->name, "zocl", version->name_len);
  strlcpy(version->date, __DATE__, version->date_len);
  strlcpy(version->desc, "RTEMS ZOCL Versal (Contemporary Software)", version->desc_len);
  return 0;
}

static int zocl_aie_fd(zocl_dev* zocl, struct drm_zocl_aie_fd* aie_fd) {
  zocl_debug("zocl: aie-fd: partition=%" PRIu32 "\n", aie_fd->partition_id);
  /*
   * No need to jump through the hoops Linux does to get access to the AIE
   * memory. On RTEMS we have a 1:1 mapping and the AIE is mapped into cache
   * coherent memory. Return an invalif fd to catch any use of this.
   *
   * The fd is plac into the embeddedsw's aiengine2 Config data and use by a
   * Linux specific module to mmap the address into the user process's address
   * space.
   */
  aie_fd->fd = -1;
  return 0;
}

static int zocl_read_axlf(zocl_dev* zocl, struct drm_zocl_axlf* axlf_obj) {
  return zocl_load_axlf(zocl, axlf_obj);
}

static ssize_t zocl_read(
  rtems_libio_t *iop, void *buffer, size_t count) {
  //zocl_dev *zocl = IMFS_generic_get_context_by_iop(iop);
  zocl_debug("zocl: read\n");
  rtems_set_errno_and_return_minus_one(EIO);
}

static ssize_t zocl_write(
  rtems_libio_t *iop, const void *buffer, size_t count) {
  zocl_debug("zocl: write\n");
  rtems_set_errno_and_return_minus_one(EIO);
}

static int zocl_ioctl(
  rtems_libio_t *iop, ioctl_command_t command, void *arg) {
  zocl_dev *zocl = zocl_get(iop);
  int err = 0;

  switch (command) {
    case DRM_IOCTL_VERSION:
      zocl_debug("zocl: cmd: VERSION\n");
      err = zocl_version(arg);
      break;
    case DRM_IOCTL_ZOCL_CREATE_BO:
      zocl_debug("zocl: cmd: ZOCL_CREATE_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_USERPTR_BO:
      zocl_debug("zocl: cmd: ZOCL_USERPTR_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_GET_HOST_BO:
      zocl_debug("zocl: cmd: ZOCL_GET_HOST_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_MAP_BO:
      zocl_debug("zocl: cmd: ZOCL_MAP_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_SYNC_BO:
      zocl_debug("zocl: cmd: ZOCL_SYNC_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_INFO_BO:
      zocl_debug("zocl: cmd: ZOCL_INFO_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_PWRITE_BO:
      zocl_debug("zocl: cmd: ZOCL_PWRITE_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_PREAD_BO:
      zocl_debug("zocl: cmd: ZOCL_PREAD_BO\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_EXECBUF:
      zocl_debug("zocl: cmd: ZOCL_EXECBUF\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_READ_AXLF:
      zocl_debug("zocl: cmd: ZOCL_READ_AXLF\n");
      err = zocl_read_axlf(zocl, arg);
      break;
    case DRM_IOCTL_ZOCL_SK_GETCMD:
      zocl_debug("zocl: cmd: ZOCL_SK_GETCMD\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_SK_CREATE:
      zocl_debug("zocl: cmd: ZOCL_SK_CREATE\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_SK_REPORT:
      zocl_debug("zocl: cmd: ZOCL_SK_REPORT\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_INFO_CU:
      zocl_debug("zocl: cmd: ZOCL_INFO_CU\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_CTX:
      zocl_debug("zocl: cmd: ZOCL_CTX\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_ERROR_INJECT:
      zocl_debug("zocl: cmd: ZOCL_ERROR_INJECT\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_AIE_FD:
      zocl_debug("zocl: cmd: ZOCL_AIE_FD\n");
      err = zocl_aie_fd(zocl, arg);;
      break;
    case DRM_IOCTL_ZOCL_AIE_RESET:
      zocl_debug("zocl: cmd: ZOCL_AIE_RESET\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_AIE_GETCMD:
      zocl_debug("zocl: cmd: ZOCL_AIE_GETCMD\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_AIE_PUTCMD:
      zocl_debug("zocl: cmd: ZOCL_AIE_PUTCMD\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_AIE_FREQSCALE:
      zocl_debug("zocl: cmd: ZOCL_AIE_FREQSCALE\n");
      err = EIO;
      break;
    case DRM_IOCTL_ZOCL_REQUEST:
      zocl_debug("zocl: cmd: ZOCL_REQUEST\n");
      err = zocl_request(zocl, arg);
      break;
    default:
      zocl_debug("zocl: cmd: unknow: %08x\n", (int) command);
      err = EIO;
      break;
  }

  zocl_debug("zocl: err=%i\n", err);

  if (err != 0) {
    rtems_set_errno_and_return_minus_one(err);
  }
  return 0;
}

static const rtems_filesystem_file_handlers_r zocl_handler = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = zocl_read,
  .write_h = zocl_write,
  .ioctl_h = zocl_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static void zocl_node_destroy(IMFS_jnode_t *node) {
  zocl_dev *zocl;
  zocl = IMFS_generic_get_context_by_node(node);
  free(zocl);
  IMFS_node_destroy_default(node);
}

static const IMFS_node_control zocl_node_control = IMFS_GENERIC_INITIALIZER(
  &zocl_handler, IMFS_node_initialize_generic, zocl_node_destroy);

int rtems_zocl_register(const char* path) {
  zocl_dev* zocl = zocl_dev_init();
  int r;
  if (zocl == NULL) {
    errno = ENOMEM;
    return -1;
  }
  r = IMFS_make_generic_node(
    path, S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, &zocl_node_control, zocl);
  if (r != 0) {
    free(zocl);
    //(*bus->destroy)(bus);
  }
  rtems_zocl_trace = ZOCL_TRACE_DEBUG | ZOCL_TRACE_IOCTL;
  return r;
}
