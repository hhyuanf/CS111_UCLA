/*****************************************************************************
 * schedos-3
 *
 *   See schedos-1.
 *
 *****************************************************************************/

#define PRINTCHAR	('3' | 0x0900)
#ifndef USER_PRIORITY
#define USER_PRIORITY	3
#endif

#ifndef USER_SHARE
#define USER_SHARE	3
#endif

#include "schedos-1.c"
