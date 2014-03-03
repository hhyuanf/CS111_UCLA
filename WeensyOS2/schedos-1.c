#include "schedos-app.h"
#include "x86sync.h"
#include "schedos-kern.h"


/*****************************************************************************
 * schedos-1
 *
 *   This tiny application prints red "1"s to the console.
 *   It yields the CPU to the kernel after each "1" using the sys_yield()
 *   system call.  This lets the kernel (schedos-kern.c) pick another
 *   application to run, if it wants.
 *
 *   The other schedos-* processes simply #include this file after defining
 *   PRINTCHAR appropriately.
 *
 *****************************************************************************/

#ifndef PRINTCHAR
#define PRINTCHAR	('1' | 0x0C00)
#endif

#ifndef USER_PRIORITY
#define USER_PRIORITY	1
#endif

#ifndef USER_SHARE
#define USER_SHARE	1
#endif



void
start(void)
{
	int i;
	sys_priority(USER_PRIORITY);
	sys_share(USER_SHARE);

	for (i = 0; i < RUNCOUNT; i++) {
		// Write characters to the console, yielding after each one.
#if SYN == 0
		*cursorpos++ = PRINTCHAR;
#endif
#if SYN == 1
		while (compare_and_swap(&mutex, 0, 1) != 0)
			continue;
		*cursorpos++ = PRINTCHAR;
		compare_and_swap(&mutex, 1, 0);
#endif
#if SYN == 2
		atomic_print(PRINTCHAR);
#endif
		sys_yield();

	}

	// Yield forever.
	while (1)
		sys_exit(0);
}
