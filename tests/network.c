/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */
/*
 * Network support
 */

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
 * Configure LibBSD.
 */
#define RTEMS_BSD_CONFIG_DOMAIN_PAGE_MBUFS_SIZE (32 * 1024 * 1024)
#define RTEMS_BSD_CONFIG_BSP_CONFIG
#define RTEMS_BSD_CONFIG_INIT

#include <machine/rtems-bsd-config.h>

#include <rtems/bsd/iface.h>

static bool wait_for_iface_up(const char* iface)
{
  int wait_for_secs = 30;
  bool state = false;
  while (wait_for_secs > 0) {
    --wait_for_secs;
    if (rtems_bsd_iface_link_state(iface, &state) == 0) {
      if (state) {
        return true;
      }
    }
    sleep(1);
  }
  if (!state) {
    printf("error: network interfae not up: %s\n", iface);
    return false;
  }
  return true;
}

static bool network_config(
  const char* hostname, const char* iface, const char* macaddr,
  const char* ipaddr, const char* netmask, const char* gateway,
  const char* ifconfig_options) {
  const bool dhcp = ipaddr == NULL || netmask == NULL;
  FILE* rc_conf;
  struct stat sb;
  int r;
  if (ifconfig_options == NULL) {
    ifconfig_options = "";
  }
  r = stat("/etc", &sb);
  if (r != 0) {
    if (errno != ENOENT) {
      printf("error: checking etc: %s\n", strerror(errno));
      return false;
    }
    r = mkdir(
      "/etc", S_IFDIR | S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH);
    if (r != 0) {
      printf("error: mkdir etc: %s\n", strerror(errno));
      return false;
    }
  }
  rc_conf = fopen("/etc/rc.conf", "w");
  if (rc_conf == NULL)
  {
    printf("error: cannot create /etc/rc.conf\n");
    return false;
  }
  fprintf(rc_conf, "#\n# LibBSD Configuration\n#\n");
  fprintf(rc_conf, "hostname=\"%s\"\n", hostname);
  if (dhcp) {
    fprintf(rc_conf, "ifconfig_%s=\"DHCP %s\"\n", iface, ifconfig_options);
  } else {
    fprintf(rc_conf, "ifconfig_%s=\"inet %s netmask %s %s\"\n",
            iface, ipaddr, netmask, ifconfig_options);
  }
  if (macaddr != NULL) {
    fprintf(rc_conf, "ifconfig_%s_alias0=\"ether %s\"\n", iface, macaddr);
  }
  if (!dhcp && gateway != NULL) {
    fprintf(rc_conf, "defaultrouter=\"%s\"\n", gateway);
  }
  if (dhcp) {
    fprintf(rc_conf, "dhcpcd_priority=\"%d\"\n", 200);
    fprintf(rc_conf, "dhcpcd_options=\"--nobackground --timeout 10\"\n");
  }
  fclose(rc_conf);
  return true;
}

bool network_init(
  const char* hostname, const char* iface, const char* macaddr,
  const char* ipaddr, const char* netmask, const char* gateway,
  const char* ifconfig_options) {
  const int timeout = 30;
  const bool trace = false;
  int r;
  bool ok =
    network_config(
      hostname, iface, macaddr, ipaddr, netmask, gateway, ifconfig_options);
  if (!ok) {
    return false;
  }
  rtems_bsd_initialize();
  r = rtems_bsd_run_etc_rc_conf(timeout, trace);
  if (r < 0) {
    printf("error: network /etc/rc.conf failed: %s\n", strerror(errno));
    return false;
  }
  ok = wait_for_iface_up(iface);
  if (!ok) {
    return false;
  }
  return true;
}
