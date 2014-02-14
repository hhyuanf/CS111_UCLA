// UCLA CS 111 Lab 1 main program

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdio.h>
#include "command.h"
#include "alloc.h"

static char const *program_name;
static char const *script_name;

static void
usage (void)
{
  error (1, 0, "usage: %s [-pt] SCRIPT-FILE", program_name);
}

static int
get_next_byte (void *stream)
{
  return getc (stream);
}



int
main (int argc, char **argv)
//main ()
{
  /*int argc=3;
  char *argv[3];
  argv[0]="timetrash";
  argv[1]="-t";
  argv[2]="mytest";*/

  int command_number = 1;
  int print_tree = 0;
  int time_travel = 0;
  program_name = argv[0];


  for (;;)
    switch (getopt (argc, argv, "pt"))
      {
      case 'p': print_tree = 1; break;
      case 't': time_travel = 1; break;
      default: usage (); break;
      case -1: goto options_exhausted;
      }
 options_exhausted:;

  // There must be exactly one file argument.
  if (optind != argc - 1)
    usage ();

  script_name = argv[optind];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream);

  command_t last_command = NULL;
  command_t command;

  if( time_travel == 0 ){
	  while ((command = read_command_stream (command_stream)))
		{
		  if (print_tree)
		{
		  printf ("# %d\n", command_number++);
		  print_command (command);
		}
		  else
		{
		  last_command = command;
		  execute_command (command, time_travel);
		}
		}
	  return print_tree || !last_command ? 0 : command_status (last_command);
  }
  else{
	  cmd_stream_list_t firstCmdUnit = 0;
	  int firstIte = 1;
	  int cid = 0;
	  int status = 0;
	  cmd_stream_list_t current_ptr = 0;
	  while ((command = read_command_stream (command_stream)))
	  {
		  if( firstIte == 1){
			  firstCmdUnit = ( cmd_stream_list_t )checked_malloc(sizeof(struct cmd_stream_list));
			  firstCmdUnit->cmd_unit = command;
			  current_ptr = firstCmdUnit;
			  firstIte = 0;
			  cid++;
		  }
		  else{
			  current_ptr->next = ( cmd_stream_list_t )checked_malloc(sizeof(struct cmd_stream_list));
			  current_ptr = current_ptr->next;
			  current_ptr->cmd_unit = command;
			  cid++;
		  }
	  }
	  status = execute_timetravel_cmd(firstCmdUnit, time_travel, cid);
	  return status;
  }
}
