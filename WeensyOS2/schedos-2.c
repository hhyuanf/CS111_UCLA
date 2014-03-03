/*****************************************************************************
 * schedos-2
 *
 *   See schedos-1.
 *
 *****************************************************************************/

#define PRINTCHAR	('2' | 0x0A00)
#ifndef USER_PRIORITY
#define USER_PRIORITY	2
#endif

#ifndef USER_SHARE
#define USER_SHARE	2
#endif

#include "schedos-1.c"
