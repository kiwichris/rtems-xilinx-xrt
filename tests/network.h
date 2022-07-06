/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */
/*
 * Network support
 */

#ifndef NETWORK_H
#define NETWORK_H

/*
 * Network initialisation
 *
 * You must provide `hostname` and `iface`.
 *
 * If `macaddr` points to a string of format "00:11:22:33:44:55" the
 * interface will be configured with that MAC address.
 *
 * If `ipaddr` and `netmask` both point to strings the interface will
 * be given a static address configuration. If either are NULL the
 * interface will be configured for DHCP.
 *
 * If `gateway` points to a valid string and the interface has a
 * static configuration the default gateway will be configured. This
 * option is ignored for DHCP configurations.
 *
 * The argument `ifconfig_options` can optionally point to extra
 * ifconfig options such as "rxcsum txcsum". The string is appended to
 * the ifconfig options.
 */
bool network_init(
  const char* hostname, const char* iface, const char* macaddr,
  const char* ipaddr, const char* netmask, const char* gateway,
  const char* ifconfig_options);

#endif /* NETWORK_H */
