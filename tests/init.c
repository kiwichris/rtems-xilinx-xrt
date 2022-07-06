/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */
/*
 * Application with networking initalisation
 */

#include <inttypes.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/fatal.h>

#include <bsp.h>

/*
 * Provide this fatal error handler as the default seems to crash.
 *
 * The default handler on Versal needs to be fixed but I do not have
 * the time.
 */
#define TEST_FATAL_ERROR_HANDLER 1

#if TEST_FATAL_ERROR_HANDLER
#include <bsp/bootcard.h>

static rtems_interrupt_lock lock = RTEMS_INTERRUPT_LOCK_INITIALIZER("ferror");
static void fatal_extension(
  rtems_fatal_source source, bool is_internal, rtems_fatal_code code)
{
  rtems_interrupt_lock_context lcontext;
  rtems_interrupt_lock_acquire(&lock, &lcontext);
  printk("FATAL: %s (%d/%" PRIxLEAST32 ")\n", rtems_fatal_source_text(source), source, (int) code);
  if (source == RTEMS_FATAL_SOURCE_EXCEPTION || source == RTEMS_FATAL_SOURCE_HEAP) {
    bsp_fatal_extension(source, is_internal, code);
  }
  printk("Resetting ....\n");
  bsp_reset();
}
#define CONFIGURE_INITIAL_EXTENSIONS { .fatal = fatal_extension }
#endif

#ifdef CONFIGURE_MICROSECONDS_PER_TICK
 #undef CONFIGURE_MICROSECONDS_PER_TICK
#endif

#define CONFIGURE_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(1)

#define CONFIGURE_MAXIMUM_DRIVERS                  10
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_INIT_TASK_INITIAL_MODES          RTEMS_DEFAULT_MODES
#define CONFIGURE_INIT_TASK_ATTRIBUTES             RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_STACK_SIZE             (64 * 1024)
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_MEMORY_OVERHEAD                  2048
#define CONFIGURE_UNLIMITED_OBJECTS                1
#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE        (20)
#define CONFIGURE_MAXIMUM_POSIX_KEYS               (200)
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS          (2)

#define CONFIGURE_STACK_CHECKER_ENABLED

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS         200
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_FILESYSTEM_IMFS
#define CONFIGURE_FILESYSTEM_NFS
#define CONFIGURE_FILESYSTEM_RFS

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_SWAPOUT_TASK_PRIORITY            100
#define CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY   100
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE          (8 * 1024 * 1024)
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS      0
#define CONFIGURE_BDBUF_MAX_WRITE_BLOCKS           512

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL

#define CONFIGURE_SHELL_NO_COMMAND_BLKSYNC
#define CONFIGURE_SHELL_NO_COMMAND_BLKSTATS
#define CONFIGURE_SHELL_NO_COMMAND_FDISK

#define CONFIGURE_SHELL_COMMANDS_ALL_NETWORKING

#include <bsp/irq-info.h>
#include <rtems/netcmds-config.h>

#define CONFIGURE_SHELL_USER_COMMANDS \
  &bsp_interrupt_shell_command, \
  &rtems_shell_ARP_Command, \
  &rtems_shell_HOSTNAME_Command, \
  &rtems_shell_PING_Command, \
  &rtems_shell_ROUTE_Command, \
  &rtems_shell_NETSTAT_Command, \
  &rtems_shell_IFCONFIG_Command, \
  &rtems_shell_SYSCTL_Command

#include <rtems/shellconfig.h>
