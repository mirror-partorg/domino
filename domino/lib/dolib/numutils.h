/*
  Copyright 1998-2003 Victor Wagner
  Copyright 2003 Alex Ott
  This file is released under the GPL.  Details can be
  found in the file COPYING accompanying this distribution.
*/
#ifndef NUMUTILS_H
#define NUMUTILS_H

#include <stdint.h>

/* numeric conversions */

int32_t  get_int32(void *buffer, int offset);
uint32_t get_uint32(void *buffer, int offset);
uint16_t get_uint16(void *buffer, int offset);
double   get_double(void *buffer, int offset);

#endif
