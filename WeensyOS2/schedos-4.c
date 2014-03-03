/*****************************************************************************
 * schedos-4
 *
 *   See schedos-1.
 *
 *****************************************************************************/

#define PRINTCHAR	('4' | 0x0E00)
#ifndef USER_PRIORITY
#define USER_PRIORITY	4
#endif

#ifndef USER_SHARE
#define USER_SHARE	4
#endif

#include "schedos-1.c"
