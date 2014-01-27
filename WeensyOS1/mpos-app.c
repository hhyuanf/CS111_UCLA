#include "mpos-app.h"
#include "lib.h"
#define EXTRA_CREDIT_EXERCISE_5 0
#define EXTRA_CREDIT_EXERCISE_6 0

/*****************************************************************************
 * mpos-app
 *
 *   This application simply starts a child process and then waits for it
 *   to exit.  Both processes print messages to the screen.
 *
 *****************************************************************************/
#if (EXTRA_CREDIT_EXERCISE_5 == 0 && EXTRA_CREDIT_EXERCISE_6 == 0)
void run_child(void);

void
start(void)
{
	volatile int checker = 0; /* This variable checks that you correctly
				     gave the child process a new stack. */
	pid_t p;
	int status;

	app_printf("About to start a new process...\n");

	p = sys_fork();
	if (p == 0)
		run_child();
	else if (p > 0) {
		app_printf("Main process %d!\n", sys_getpid());
		do {
			status = sys_wait(p);
			app_printf("W");
		} while (status == WAIT_TRYAGAIN);
		app_printf("Child %d exited with status %d!\n", p, status);

		// Check whether the child process corrupted our stack.
		// (This check doesn't find all errors, but it helps.)
		if (checker != 0) {
			app_printf("Error: stack collision!\n");
			sys_exit(1);
		} else
			sys_exit(0);

	} else {
		app_printf("Error!\n");
		sys_exit(1);
	}
}

void
run_child(void)
{
	int i;
	volatile int checker = 1; /* This variable checks that you correctly
				     gave this process a new stack.
				     If the parent's 'checker' changed value
				     after the child ran, there's a problem! */

	app_printf("Child process %d!\n", sys_getpid());

	// Yield a couple times to help people test Exercise 3
	for (i = 0; i < 20; i++)
		sys_yield();
		

	sys_exit(1000);
}
#endif

#if (EXTRA_CREDIT_EXERCISE_5 == 1)
void start(void) {
        int x = 0;  /* note that local variable x lives on the stack */
        int *volatile px = &x;
        pid_t p = sys_fork();
        if (p == 0)
        {
		x = 1;
		*px = 1;
	}		
        else if (p > 0)
                sys_wait(p); // assume blocking implementation
        app_printf("%d", x);
        sys_exit(0);
}
#endif

#if (EXTRA_CREDIT_EXERCISE_6 == 1)
void start_function(void);

void
start(void)
{
	volatile int checker = 0; /* This variable checks that you correctly
				     gave the child process a new stack. */
	pid_t p;
	int status;

	app_printf("About to start a new process...\n");
	
	p = sys_newthread(&start_function);
	//app_printf("%d",p);
	if (p > 0) {
		app_printf("Main process %d!\n", sys_getpid());
		do {
			status = sys_wait(p);
			app_printf("W");
		} while (status == WAIT_TRYAGAIN);
		app_printf("Child %d exited with status %d!\n", p, status);

		// Check whether the child process corrupted our stack.
		// (This check doesn't find all errors, but it helps.)
		if (checker != 0) {
			app_printf("Error: stack collision!\n");
			sys_exit(1);
		} else
			sys_exit(0);

	} else {
		app_printf("Error!\n");
		sys_exit(1);
	}
}

void
start_function(void)
{
	int i;
	/*volatile int checker = 1; This variable checks that you correctly
				     gave this process a new stack.
				     If the parent's 'checker' changed value
				     after the child ran, there's a problem! */

	app_printf("Child process %d!\n", sys_getpid());

	// Yield a couple times to help people test Exercise 3
	for (i = 0; i < 20; i++)
		sys_yield();
		

	sys_exit(1000);
}
#endif


