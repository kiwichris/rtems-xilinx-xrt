/*
 * Copyright 2022 Chris Johns (chrisj@rtems.org)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
 */
/*
 * Stubs until aacrh64 support added to RTEMS
 */

#include <stdlib.h>

void*
dlopen (const char* name, int mode)
{
  return NULL;
}

int
dlclose (void* handle)
{
  return 0;
}

void*
dlsym (void* handle, const char *symbol)
{
  return NULL;
}

const char*
dlerror (void)
{
  return "not supported";
}
