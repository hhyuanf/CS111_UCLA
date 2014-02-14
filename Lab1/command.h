// UCLA CS 111 Lab 1 command interface

#include<sys/types.h>

typedef struct command *command_t;
typedef struct command_stream *command_stream_t;

/* Create a command stream from LABEL, GETBYTE, and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.  Use "time travel" if the integer flag is
   nonzero.  */
void execute_command (command_t, int);

/* Return the exit status of a command, which must have previously been executed.
   Wait for the command, if it is not already finished.  */
int command_status (command_t);



struct cmd_stream_list{
	command_t cmd_unit;
	struct cmd_stream_list *next;
	char **output_constrain;
	char **input_constrain;
	int i_size;
	int o_size;
	int running;
	int finished;
	pid_t pid;
};


typedef struct cmd_stream_list* cmd_stream_list_t;

int execute_timetravel_cmd(cmd_stream_list_t firstCmdUnit, int time_travel, int size);
