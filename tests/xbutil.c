/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */
/*
 * Simple command handler for the XRT xbutil command.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/shell.h>

#include <xrt.h>

#include <rtems/pm/pm.h>
#include <rtems/zocl/zocl.h>
#include <rtems/shell.h>

#include "debugger.h"
#include "network.h"

extern int xbutil_main(int argc, char** arg);

#ifndef TEST_HOSTNAME
#define TEST_HOSTNAME "xrt"
#endif

#ifndef TEST_IFACE
#define TEST_IFACE "cgem0"
#endif

#ifndef TEST_MACADDR
#define TEST_MACADDR "06:00:00:10:00:01"
#endif

#ifndef TEST_IPADDR
#define TEST_IPADDR NULL
#endif

#ifndef TEST_NETMASK
#define TEST_NETMASK NULL
#endif

#ifndef TEST_GATEWAY
#define TEST_GATEWAY NULL
#endif

#ifndef TEST_IFCONFIG_OPTS
#define TEST_IFCONFIG_OPTS "rxcsum txcsum"
#endif

#ifndef TEST_DB_CMD
#define TEST_DB_CMD true
#endif

#ifndef TEST_DB_START
#define TEST_DB_START true
#endif

#ifndef TEST_DB_WAIT
#define TEST_DB_WAIT false
#endif

static void pm_init(void) {
  rtems_pm_cmd_register();
}

static void xrt_init(void) {
  xclDeviceHandle handle;
  struct stat sb;
  rtems_status_code sc;
  int r;
  r = stat("/dev", &sb);
  if (r != 0) {
    if (errno != ENOENT) {
      printf("error: checking /dev: %s\n", strerror(errno));
      return;
    }
    r = mkdir(
      "/dev", S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH);
    if (r != 0) {
      printf("error: mkdir /dev: %s\n", strerror(errno));
      return;
    }
  }
  r = stat("/dev/dri", &sb);
  if (r != 0) {
    if (errno != ENOENT) {
      printf("error: checking /dev/drm: %s\n", strerror(errno));
      return;
    }
    r = mkdir(
      "/dev/dri", S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH);
    if (r != 0) {
      printf("error: mkdir /dev/dri: %s\n", strerror(errno));
      return;
    }
  }
  r = rtems_zocl_register("/dev/dri/renderD128");
  if (r != 0) {
    printf("error: xcl zocl: driver register failed: %s\n", strerror(errno));
    return;
  }
  handle = xclOpen(0, "", XCL_INFO);
  if (handle == NULL) {
    printf("error: xcl open: failure\n");
  }
  xclClose(handle);
  rtems_shell_add_cmd(
    "xbutil2", "xilinx",
    "The Xilinx (R) Board Utility (xbutil) is a standalone command line utility",
    xbutil_main);
  sc = rtems_shell_init("xrt", 8 * 1024 * 1024, 1, "/dev/console", 0, 1, NULL);
  if (sc != RTEMS_SUCCESSFUL) {
    printf("shell: starting %s (%d)", rtems_status_text (sc), sc);
  }
}

static void nfs_init(void) {
  struct stat sb;
  char* args[6];
  int   r;
  r = stat("/net", &sb);
  if (r != 0) {
    if (errno != ENOENT) {
      printf("error: checking /net: %s\n", strerror(errno));
      return;
    }
    r = mkdir(
      "/net", S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH);
    if (r != 0) {
      printf("error: mkdir /dev: %s\n", strerror(errno));
      return;
    }
  }
  args[0] = "mount";
  args[1] = "-t";
  args[2] = "nfs";
  args[3] = "10.10.5.6:/opt/src";
  args[4] = "/net";
  args[5] = NULL;
  rtems_shell_execute_cmd(args[0], 5, args);
}

int main(int argc, char* argv[]) {
  bool ok;
  rtems_shell_init_environment();
  ok = network_init(
    TEST_HOSTNAME, TEST_IFACE, TEST_MACADDR, TEST_IPADDR, TEST_NETMASK,
    TEST_GATEWAY, TEST_IFCONFIG_OPTS);
  if (ok) {
    debugger_init(TEST_DB_CMD, TEST_DB_START, TEST_DB_WAIT);
  }
  nfs_init();
  pm_init();
  xrt_init();
  return 0;
}
