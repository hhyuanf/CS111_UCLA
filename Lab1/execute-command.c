// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <error.h>
#include<unistd.h> // pipe()
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>
#include"alloc.h"


void
execute_command (command_t c, int time_travel);

void execute_and_command(command_t c, int time_travel)
{
	execute_command(c->u.command[0], time_travel);
	if(c->u.command[0]->status == 0)
	{
		execute_command(c->u.command[1], time_travel);
		c->status = c->u.command[1]->status;
	}
	else
	{
		c->status = c->u.command[0]->status;
	}
}

void execute_or_command(command_t c, int time_travel)
{
	execute_command(c->u.command[0], time_travel);
	if(c->u.command[0]->status != 0)
	{
		execute_command(c->u.command[1], time_travel);
		c->status = c->u.command[1]->status;
	}
	else
	{
		c->status = c->u.command[0]->status;
	}
}

void execute_pipe_command(command_t c, int time_travel)
{
	int fd[2];
	int status_temp;
	pid_t pid_left;
	pid_t pid_right;

	if(pipe(fd) == -1)
	{
		error(1, 0, "Fail to create pipeline!");
	}

	pid_left = fork();
	if(pid_left == -1)
	{
		error(1, 0, "Fail to fork pid_left!");
	}
	else if(pid_left == 0)// left child process
	{
		if(close(fd[0]) == -1)
		{
			error(1, 0, "Fail to close(fd[0])!");
		}
		if(dup2(fd[1], 1) == -1)
		{
			error(1, 0, "Fail to dup2(fd[1], 1)!");
		}
		execute_command(c->u.command[0], time_travel);
		exit(c->u.command[0]->status);
	}
	else // parent process
	{
		pid_right = fork();
		if(pid_right == -1)
		{
			error(1, 0, "Fail to fork pid_right!");
		}
		else if(pid_right == 0)// right child process
		{
			if(close(fd[1]) == -1)
			{
				error(1, 0, "Fail to close(fd[1]!");
			}
			if(dup2(fd[0], 0) == -1)
			{
				error(1, 0, "Fail to dup2(fd[0], 0)!");
			}
			execute_command(c->u.command[1], time_travel);
			exit(c->u.command[1]->status);
		}
		else // parent process
		{
			close(fd[0]);
			close(fd[1]);
			pid_t pid_temp;
			pid_temp = wait(&status_temp);
			if(pid_temp == pid_left)
			{
				waitpid(pid_right, &status_temp, 0);
			}
			else
			{
				waitpid(pid_left, &status_temp, 0);
			}
			c->status = status_temp;
		}
	}
}

void execute_sequence_command(command_t c, int time_travel)
{
	if(c->u.command[0] != 0)
	{
		execute_command(c->u.command[0], time_travel);
		c->status = c->u.command[0]->status;
	}
	if(c->u.command[1] != 0)
	{
		execute_command(c->u.command[1], time_travel);
		c->status = c->u.command[1]->status;
	}
}

void execute_subshell_command(command_t c, int time_travel)
{
	pid_t pid_temp;
	int status_temp;
	pid_temp = fork();
	if(pid_temp == -1)
	{
		error(0, 1, "Can't fork!");
	}
	else if(pid_temp == 0)
	{
		int fd_input;
		int fd_output;
		if(c->input != 0)
		{
			fd_input = open(c->input, O_RDONLY);
			dup2(fd_input, 0);
		}
		if(c->output != 0)
		{
			fd_output = open(c->output, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
			dup2(fd_output, 1);
		}
		execute_command(c->u.subshell_command, time_travel);
		exit(c->u.subshell_command->status);
	}
	else
	{
		wait(&status_temp);
		c->status = status_temp;
	}
}

void execute_simple_command(command_t c, int time_travel)
{
	int status_temp1;
	int status_temp2;
	pid_t pid_temp;


	pid_temp = fork();
	if(pid_temp == -1)
	{
		error(0, 1, "Can't fork!");
	}
	else if(pid_temp == 0)
	{
		int fd_input;
		int fd_output;
		if(c->input != 0)
		{
			fd_input = open(c->input, O_RDONLY);
			dup2(fd_input, 0);
		}
		if(c->output != 0)
		{
			fd_output = open(c->output, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
			dup2(fd_output, 1);
		}


		status_temp1 = 0;
		execvp(c->u.word[0], c->u.word);
		status_temp1 = -1;
		error(0, 1, "Command not found!");
		exit(status_temp1);
	}
	else
	{
		wait(&status_temp2);
		c->status = status_temp2;
	}
}



int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int time_travel)
{
	switch(c->type)
	{
		case AND_COMMAND:
		{
			execute_and_command(c, time_travel);
			break;
		}
		case OR_COMMAND:
		{
			execute_or_command(c, time_travel);
			break;
		}
		case PIPE_COMMAND:
		{
			execute_pipe_command(c, time_travel);
			break;
		}
		case SEQUENCE_COMMAND:
		{
			execute_sequence_command(c, time_travel);
			break;
		}
		case SUBSHELL_COMMAND:
		{
			execute_subshell_command(c, time_travel);
			break;
		}
		case SIMPLE_COMMAND:
		{
			execute_simple_command(c, time_travel);
			break;
		}
		default:
		{
			error (1, 0, "Undefined command type!");
			break;
		}
	}
}



void get_constrains( command_t cmd_root, char **o_constrain, char **i_constrain, int *i_ind, int *o_ind ){
	switch(cmd_root->type)
	{
		case AND_COMMAND:
		case OR_COMMAND:
		case SEQUENCE_COMMAND:
		case PIPE_COMMAND:
		{
			if( cmd_root->u.command[0] !=0 ){
				get_constrains( cmd_root->u.command[0], o_constrain, i_constrain, i_ind, o_ind );
			}
			if( cmd_root->u.command[1] !=0 ){
				get_constrains( cmd_root->u.command[1], o_constrain, i_constrain, i_ind, o_ind );
			}
			break;
		}
		case SIMPLE_COMMAND:
		{
			if( cmd_root->output !=0 ){
				o_constrain[*o_ind] = (char*) checked_malloc( (strlen(cmd_root->output)+1) * sizeof(char) );
				strcpy(o_constrain[*o_ind], cmd_root->output);
				++(*o_ind);
			}
			if( cmd_root->input !=0 ){
				i_constrain[*i_ind] = (char*) checked_malloc( (strlen(cmd_root->input)+1) * sizeof(char) );
				strcpy(i_constrain[*i_ind], cmd_root->input);
				++(*i_ind);
			}
			char **ptr = cmd_root->u.word;
			while(*ptr!=0){
				i_constrain[*i_ind] = (char*) checked_malloc( (strlen(*ptr)+1) * sizeof(char) );
				strcpy(i_constrain[*i_ind], *ptr);
				++(*i_ind);
				++ptr;
			}
			break;
		}
		case SUBSHELL_COMMAND:
		{
			if( cmd_root->u.subshell_command != 0)
			{
				get_constrains( cmd_root->u.subshell_command, o_constrain, i_constrain, i_ind, o_ind );
			}
			break;
		}
		default:
		{
			error(0, 1, "Unknown command type in timetravel exe!");
		}
	}
}

int check_constrain(char **o_constrain, int o_size, char **i_constrain, int i_size)
{
	int i = 0;
	int j = 0;
	while(i<o_size)
	{
		while(j<i_size)
		{
			if( strcmp(o_constrain[i], i_constrain[j]) == 0 )
			{
				return 1;
			}
			j++;
		}
		i++;
	}
	return 0;
}

cmd_stream_list_t get_ptr(cmd_stream_list_t ptr, int position)
{
	int i=0;
	while( i < position )
	{
		ptr = ptr->next;
		i++;
	}
	return ptr;
}

void execute_tcmd(cmd_stream_list_t firstCmdUnit, int time_travel, int size, int *constrain_graph, int *W, int *status )
{
	int ind = 0;
	int all_run = 1;

	while( ind < size )
	{
		if( (get_ptr(firstCmdUnit, ind))->running == 0 )
		{
			all_run = 0;
			break;
		}
		ind++;
	}



	if(all_run == 0)
	{
		int ind = 0;
		pid_t pid_temp;
		while( ind < size)
		{
			if( *(W+ind) == 0 &&  (get_ptr(firstCmdUnit, ind))->running == 0 )
			{
				pid_temp = fork();
				if( pid_temp == -1 )
				{
					error(0, 1, "Error creating child process in time travel exec!");
				}
				else if(pid_temp == 0)//child process
				{
					execute_command ((get_ptr(firstCmdUnit, ind))->cmd_unit, time_travel);
					exit((get_ptr(firstCmdUnit, ind))->cmd_unit->status);
				}
				else
				{
					(get_ptr(firstCmdUnit, ind))->running = 1;
					(get_ptr(firstCmdUnit, ind))->pid = pid_temp;
				}

			}
			ind++;
		}


		int temp = 0;
		temp = *status;
		pid_temp = wait(status);
		if( temp != 0 )
		{
			*status = temp;
		}

		int row = 0;
		int col = 0;
		while(row<size)
		{
			col = 0;
			while( col < size )
			{
				if((get_ptr(firstCmdUnit, col))->pid == pid_temp && *(constrain_graph + size*row+ col) == 1)
				{
					*(constrain_graph + size*row+ col) = 0;
					(*(W+row))--;
				}
				col++;
			}
			row++;
		}
		execute_tcmd(firstCmdUnit, time_travel, size, constrain_graph, W, status );
	}
}

int execute_timetravel_cmd(cmd_stream_list_t firstCmdUnit, int time_travel, int size)
{
	cmd_stream_list_t ptr = firstCmdUnit;
	cmd_stream_list_t ptr1;
	cmd_stream_list_t ptr2;
	int buffer_size = 100;
	int i_ind = 0;
	int o_ind = 0;
	int status = 0;
	while(ptr!=0)
	{
		i_ind = 0;
		o_ind = 0;
		ptr->input_constrain = (char**) checked_malloc( buffer_size * sizeof(char*));
		ptr->output_constrain = (char**) checked_malloc( buffer_size * sizeof(char*));
		get_constrains( ptr->cmd_unit, ptr->output_constrain, ptr->input_constrain, &i_ind, &o_ind );
		ptr->i_size = i_ind;
		ptr->o_size = o_ind;
		ptr = ptr->next;
	}
	int *constrain_graph = (int *) checked_malloc( size * size * sizeof(int) );
	int *W = (int *) checked_malloc(size * sizeof(int));
	int row = 0;
	int col = 0;
	ptr = firstCmdUnit;
	while( row < size )
	{
		*(W+row) = 0;
		col = 0;
		while( col < size )
		{
			if(col != row)
			{
				ptr1 = get_ptr(ptr, col);
				ptr2 = get_ptr(ptr, row);
				if( check_constrain( ptr1->output_constrain, ptr1->o_size, ptr2->input_constrain, ptr2->i_size) == 1
						|| check_constrain( ptr1->output_constrain, ptr1->o_size, ptr2->output_constrain, ptr2->o_size) == 1)
				{
					*(constrain_graph + row*size + col ) = 1;
					(*(W+row))++;
				}
				else
				{
					*(constrain_graph + row*size + col ) = 0;
				}
			}
			else
			{
				*(constrain_graph + row*size + col ) = 0;
			}
			col++;
		}
		row++;
	}
	execute_tcmd( firstCmdUnit, time_travel, size, constrain_graph, W, &status);
	return status;
}


