// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


enum token_type
{
	NULL_TOKEN,
	WORD_TOKEN,
	PIPELINE_TOKEN,
	AND_TOKEN,
	HALFAND_TOKEN,
	OR_TOKEN,
	LBKT_TOKEN,
	RBKT_TOKEN,
	IN_TOKEN,
	OUT_TOKEN,
	SEMICOLON_TOKEN,
	ENDCMD_TOKEN,
	HALFENDCMD_TOKEN,
};

// structure for token stream
struct command_stream
{
	enum token_type token;
	char *content;
};

// check a char is content of a word token
bool is_word_content(char next_byte)
{
	if( (next_byte >= 'a' && next_byte <='z') ||
				(next_byte >= 'A' && next_byte <='Z') ||
				(next_byte >= '0' && next_byte <='9') ||
				next_byte == '!' || next_byte == '%' ||
				next_byte == '+' || next_byte == ',' ||
				next_byte == '-' || next_byte == '.' ||
				next_byte == '/' || next_byte == ':' ||
				next_byte == '@' || next_byte == '^' ||
				next_byte == '_')
	{
		return true;
	}
	else
	{
		return false;
	}
}

// check both sides have parameter
bool check_twosides(command_stream_t gra_check, command_stream_t command_s)
{
	bool left_side = ( (gra_check-1)->token == WORD_TOKEN || (gra_check-1)->token == RBKT_TOKEN ) ||  ( (gra_check-1)->token == HALFENDCMD_TOKEN && ( (gra_check-2)->token == WORD_TOKEN || (gra_check-2)->token == RBKT_TOKEN ) );
	bool right_side = ( (gra_check+1)->token == WORD_TOKEN || (gra_check+1)->token == LBKT_TOKEN) || ( (gra_check+1)->token == HALFENDCMD_TOKEN && ( (gra_check+2)->token == WORD_TOKEN || (gra_check+2)->token == LBKT_TOKEN) );
	if( (gra_check+1)->token == ENDCMD_TOKEN || gra_check-1 < command_s )
	{
		return true;
	}
	else
	{
		return !(left_side && right_side);
	}
}

// check a char is supported
bool is_valid_char(char next_byte)
{
	if(  is_word_content(next_byte) || next_byte == ';' ||
			next_byte == '|' || next_byte == '&' ||
			next_byte == '(' || next_byte == ')' ||
			next_byte == '<' || next_byte == '>' ||
			next_byte == '\t' || next_byte == ' ' || next_byte == '\n')
		return true;
	else
		return false;
}

// set flags to ignore comments
void ignore_comments(char next_byte, bool *is_comments_t)
{
	if(next_byte == '#')
	{
		*is_comments_t = true;
	}
	else if(next_byte == '\n')
	{
		*is_comments_t = false;
	}
}

// resize the memory holding chars
void update_char(char** head, char* ptr, int *buffer_size, int buffer_inc, float lim)
{
	int temp = ptr - *head;
	if( temp >= lim * *buffer_size )
	{
		*head = (char *) checked_realloc( *head, (*buffer_size + buffer_inc) * sizeof(char));
		*buffer_size = *buffer_size + buffer_inc;
		ptr = *head + temp;
	}
}

// resize the memory holding tokens
void update_stream(struct command_stream** head,
		struct command_stream** ptr, int *buffer_size, int buffer_inc, float lim)
{

	int temp = *ptr - (*head) + 1;
	if( temp >= lim * (*buffer_size) )
	{
		*head = (struct command_stream *)
				checked_realloc( *head, (*buffer_size + buffer_inc) * sizeof(struct command_stream));
		*buffer_size = *buffer_size + buffer_inc;
		*ptr = *head + temp - 1;
	}
}


// resize the memory holding word content in a command structure
void update_word(char*** head, int *buffer_size, int buffer_inc, float lim)
{
	char **ptr = *head;
	int size = 0;
	while(*ptr!=0 && size<*buffer_size)
	{
		++ptr;
		++size;
	}
	if( size >= lim * *buffer_size )
	{
		*head = (char **) checked_realloc( *head, (*buffer_size + buffer_inc) * sizeof(char*));
		int j=*buffer_size;
		*buffer_size = *buffer_size + buffer_inc;
	    while(j<*buffer_size)
		{
		  *(*head+j)=0;
		  ++j;
		}
	}
}

// add a token to token stream
void add_token(char *word_buffer, struct command_stream **command_s,
		struct command_stream **command_s_ptr_ptr, int* stream_buffer_size, int stream_buffer_inc, float buffer_size_lim,
		enum token_type t_type, char **word_buffer_ptr_ptr)
{
	switch (t_type)
	{
		case WORD_TOKEN:
		{
			if(*word_buffer != '\0')
			{
				if( ( (*command_s_ptr_ptr-1)->token == ENDCMD_TOKEN || (*command_s_ptr_ptr-1)->token == HALFENDCMD_TOKEN ) &&
						( (*command_s_ptr_ptr-2)->token == AND_TOKEN || (*command_s_ptr_ptr-2)->token == OR_TOKEN
								|| (*command_s_ptr_ptr-2)->token == PIPELINE_TOKEN || (*command_s_ptr_ptr-2)->token == IN_TOKEN
								|| (*command_s_ptr_ptr-2)->token == OUT_TOKEN) )
				{
					(*command_s_ptr_ptr-1)->token = WORD_TOKEN;
					(*command_s_ptr_ptr-1)->content = (char *) checked_malloc((strlen(word_buffer)+1)*sizeof(char));
					strcpy((*command_s_ptr_ptr-1)->content, word_buffer);
					(*word_buffer_ptr_ptr) = word_buffer;
					*word_buffer='\0';
				}
				else
				{
					(*command_s_ptr_ptr)->token = WORD_TOKEN;
					(*command_s_ptr_ptr)->content = (char *) checked_malloc((strlen(word_buffer)+1)*sizeof(char));
					strcpy((*command_s_ptr_ptr)->content, word_buffer);
					(*word_buffer_ptr_ptr) = word_buffer;
					*word_buffer='\0';
					update_stream(command_s, command_s_ptr_ptr, stream_buffer_size, stream_buffer_inc, buffer_size_lim);
					++(*command_s_ptr_ptr);
				}
			}
			break;
		}
		case HALFENDCMD_TOKEN:
		{
			if((*command_s_ptr_ptr -1)->token == HALFENDCMD_TOKEN)
			{
				(*command_s_ptr_ptr -1)->token = ENDCMD_TOKEN;
			}
			else if((*command_s_ptr_ptr -1)->token == ENDCMD_TOKEN)
			{
			}
			else
			{
				(*command_s_ptr_ptr)->token = HALFENDCMD_TOKEN;
				update_stream(command_s, command_s_ptr_ptr, stream_buffer_size, stream_buffer_inc, buffer_size_lim);
				++(*command_s_ptr_ptr);
			}
			break;
		}
		case PIPELINE_TOKEN:
		{
			if((*command_s_ptr_ptr -1)->token == PIPELINE_TOKEN)
			{
				(*command_s_ptr_ptr-1)->token = OR_TOKEN;
			}
			else if((*command_s_ptr_ptr -1)->token == OR_TOKEN)
			{
				error (1, 0, "Undefined usage of '|'");
			}
			else
			{
				/*
				if( ( (*command_s_ptr_ptr-1)->token == ENDCMD_TOKEN || (*command_s_ptr_ptr-1)->token == HALFENDCMD_TOKEN) &&
						(*command_s_ptr_ptr-2)->token == WORD_TOKEN)
				{
					(*command_s_ptr_ptr-1)->token = PIPELINE_TOKEN;
				}
				else
				{*/
					(*command_s_ptr_ptr)->token = PIPELINE_TOKEN;
					update_stream(command_s, command_s_ptr_ptr, stream_buffer_size, stream_buffer_inc, buffer_size_lim);
					++(*command_s_ptr_ptr);
				//}
			}
			break;
		}
		case HALFAND_TOKEN:
		{
			if((*command_s_ptr_ptr - 1)->token == HALFAND_TOKEN)
			{
				(*command_s_ptr_ptr-1)->token = AND_TOKEN;
			}
			else if((*command_s_ptr_ptr -1)->token == AND_TOKEN)
			{
				error (1, 0, "Undefined usage of '&'");
			}
			else
			{
				/*if( ( (*command_s_ptr_ptr-1)->token == ENDCMD_TOKEN || (*command_s_ptr_ptr-1)->token == HALFENDCMD_TOKEN) &&
						(*command_s_ptr_ptr-2)->token == WORD_TOKEN)
				{
					(*command_s_ptr_ptr-1)->token = HALFAND_TOKEN;
				}
				else
				{*/
					(*command_s_ptr_ptr)->token = HALFAND_TOKEN;
					update_stream(command_s, command_s_ptr_ptr, stream_buffer_size, stream_buffer_inc, buffer_size_lim);
					++(*command_s_ptr_ptr);
				//}
			}
			break;
		}
		case SEMICOLON_TOKEN:
		case LBKT_TOKEN:
		case RBKT_TOKEN:
		{
			(*command_s_ptr_ptr)->token = t_type;
			update_stream(command_s, command_s_ptr_ptr, stream_buffer_size, stream_buffer_inc, buffer_size_lim);
			++(*command_s_ptr_ptr);
			break;
		}
		case IN_TOKEN:
		case OUT_TOKEN:
		{
			if( ( (*command_s_ptr_ptr-1)->token == ENDCMD_TOKEN || (*command_s_ptr_ptr-1)->token == HALFENDCMD_TOKEN) &&
					(*command_s_ptr_ptr-2)->token == WORD_TOKEN)
			{
				(*command_s_ptr_ptr-1)->token = t_type;
			}
			else
			{
				(*command_s_ptr_ptr)->token = t_type;
				update_stream(command_s, command_s_ptr_ptr, stream_buffer_size, stream_buffer_inc, buffer_size_lim);
				++(*command_s_ptr_ptr);
			}
			break;
		}
		default:
		{
			error (1, 0, "Token undefined!");
		}
	}
}

// convert file stream into token stream and check its grammar
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
	char next_byte = get_next_byte(get_next_byte_argument);
	bool *is_comments_t;
	bool is_comments = false;
	is_comments_t = & is_comments;
	int stream_buffer_size = 200;
	int stream_buffer_inc = 200;
	int word_buffer_size = 100;
	int word_buffer_inc = 100;
	float buffer_size_lim = 0.8;
	struct command_stream *command_s =(struct command_stream *)
			checked_malloc( stream_buffer_size * sizeof(struct command_stream));
	struct command_stream *command_s_ptr = command_s;
	char *word_buffer = (char *) checked_malloc( word_buffer_size * sizeof(char));
	*word_buffer = '\0';
	char *word_buffer_ptr = word_buffer;
	// generate token stream
	while( next_byte != -1)
	{
		ignore_comments(next_byte, is_comments_t);
		if( is_comments == false )
		{
			if(is_valid_char(next_byte))
			{

				if(is_word_content(next_byte))
				{
					*word_buffer_ptr = next_byte;
					update_char(&word_buffer, word_buffer_ptr, &word_buffer_size, word_buffer_inc, buffer_size_lim);
					++word_buffer_ptr;
					*word_buffer_ptr = '\0';
				}
				else
				{
					switch(next_byte)
					{
						case ';':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, SEMICOLON_TOKEN, &word_buffer_ptr);
							break;
						}
						case ' ':
						case '\t':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							break;
						}
						case '\n':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, HALFENDCMD_TOKEN, &word_buffer_ptr);
							break;
						}
						case '|':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, PIPELINE_TOKEN, &word_buffer_ptr);
							break;
						}
						case  '&':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, HALFAND_TOKEN, &word_buffer_ptr);
							break;
						}
						case '(':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, LBKT_TOKEN, &word_buffer_ptr);
							break;
						}
						case ')':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, RBKT_TOKEN, &word_buffer_ptr);
							break;
						}
						case '<':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s,&command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, IN_TOKEN, &word_buffer_ptr);
							break;
						}
						case '>':
						{
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, WORD_TOKEN, &word_buffer_ptr);
							add_token(word_buffer, &command_s, &command_s_ptr,
									&stream_buffer_size, stream_buffer_inc, buffer_size_lim, OUT_TOKEN, &word_buffer_ptr);
							break;
						}
						default:
						{
							break;
						}
					}
				}
			}
			else
			{
				error (1, 0, "unsupported character!");
			}
		}
		next_byte = get_next_byte(get_next_byte_argument);
	}
	// check grammar of token stream
	struct command_stream *gra_check = command_s;
	int lbkt = 0;
	int rbkt = 0;
	// discard the first newline token
	while(command_s->token==HALFENDCMD_TOKEN || command_s->token==ENDCMD_TOKEN)
	{
		command_s++;
	}
	if((command_s_ptr-1)->token==HALFENDCMD_TOKEN)
	{
		(command_s_ptr-1)->token=ENDCMD_TOKEN;
	}
	while(gra_check < command_s_ptr)
	{
		switch( gra_check->token )
		{
			case LBKT_TOKEN:
			{
				lbkt++;
				break;
			}
			case RBKT_TOKEN:
			{
				rbkt++;
				break;
			}
			case AND_TOKEN:
			{
				if( check_twosides(gra_check, command_s) )
				{
					error (1, 0, "incomplete command of '&&'!");
				}
				if((gra_check-1)->token !=WORD_TOKEN && (gra_check-1)->token !=RBKT_TOKEN)
				{
					error (1, 0, "undefined syntax of '&&'!");
				}
				break;
			}
			case OR_TOKEN:
			{
				if( check_twosides(gra_check, command_s) )
				{
					error (1, 0, "incomplete command of '||'!");
				}
				if((gra_check-1)->token !=WORD_TOKEN && (gra_check-1)->token !=RBKT_TOKEN)
				{
					error (1, 0, "undefined syntax of '||'!");
				}
				break;
			}
			case PIPELINE_TOKEN:
			{
				if( check_twosides(gra_check, command_s) )
				{
					error (1, 0, "incomplete command of '|'!");
				}
				if((gra_check-1)->token !=WORD_TOKEN && (gra_check-1)->token !=RBKT_TOKEN)
				{
					error (1, 0, "undefined syntax of '|'!");
				}
				break;
			}
			case IN_TOKEN:
			{
				if ( check_twosides(gra_check, command_s) )
				{
					error (1, 0, "incomplete command of '>'!");
				}
				break;
			}
			case OUT_TOKEN:
			{
				if( check_twosides(gra_check, command_s) )
				{
					error (1, 0, "incomplete command of '<'!");
				}
				break;
			}
			case HALFAND_TOKEN:
			{
				error (1, 0, "undefined syntax of '&'!");
				break;
			}
			case HALFENDCMD_TOKEN:
			{
				gra_check->token = ENDCMD_TOKEN;
				break;
			}
			case SEMICOLON_TOKEN:
			{
				if((gra_check-1)->token !=WORD_TOKEN && (gra_check-1)->token !=RBKT_TOKEN)
				{
					error (1, 0, "undefined syntax of ';'!");
				}
				break;
			}
			default:
			{
				break;
			}
		}
		gra_check++;
	}
	if( lbkt > rbkt )
	{
		error (1, 0, "missing ')'");
	}
	if( lbkt < rbkt )
	{
		error (1, 0, "missing '('");
	}
	free(word_buffer);
	return command_s;
}

// find the start of command
command_stream_t find_stream_start(command_stream_t s)
{
	command_stream_t ptr = s;
	while(ptr->token==NULL_TOKEN)
	{
		++ptr;
	}
	return ptr;
}

// find the end of command
command_stream_t find_stream_end(command_stream_t s)
{
	command_stream_t ptr = s;
	while(ptr->token!=ENDCMD_TOKEN)
	{
		++ptr;
	}
	return ptr;
}

// set all processed command tokens to NULL_TOKEN
void update_read_stream(command_stream_t stream_start_ptr, command_stream_t stream_end_ptr)
{
	if((stream_end_ptr+1)->token==NULL_TOKEN)
	{
		(stream_end_ptr+1)->token = ENDCMD_TOKEN;
	}
	while(stream_start_ptr <= stream_end_ptr )
	{
		stream_start_ptr->token = NULL_TOKEN;
		++stream_start_ptr;
	}
}

// get the next position for storing word content in command structure
char **word_position(command_t parse_command)
{
	char **ptr = parse_command->u.word;
	while(*ptr!=0)
	{
		++ptr;
	}
	return ptr;
}

// set all char* pointer to 0
void initialize_word(command_t s, int wd_buffer_size)
{
	char **ptr = s->u.word;
	int i = 0;
	while( i< wd_buffer_size )
	{
		*(ptr+i) = 0;
		++i;
	}
}

// allocate memory for command structure
void generate_new_node(command_t *s, int wd_buffer_size, int node_type)
{

	(*s) = (command_t) checked_malloc(sizeof(struct command));
	(*s)->input=0;
	(*s)->output=0;
	if(node_type == 1)
	{
		(*s)->u.word = (char **) checked_malloc( wd_buffer_size * sizeof(char *) );
		initialize_word(*s, wd_buffer_size);
	}
	else if(node_type == 0)
	{
		(*s)->u.command[0]=0;
		(*s)->u.command[1]=0;
	}
	else
	{
		(*s)->u.subshell_command = 0;
	}
}

// convert token stream into command structure
command_t
read_command_stream (command_stream_t s)
{
	int wd_buffer_size = 8;
	int wd_buffer_inc = 8;
	float wd_buffer_lim = 0.8;
	int is_subshell = 0;
	command_t command_root = 0;
	command_t subshell_root = 0;
	generate_new_node(&command_root, wd_buffer_size, 0);
	command_t parse_command = command_root;
	command_stream_t stream_start_ptr = find_stream_start(s);
	command_stream_t stream_end_ptr = find_stream_end(s);
	command_stream_t stream_ptr = stream_start_ptr;
	if(stream_start_ptr == stream_end_ptr)
	{
		return 0;
	}

	while( stream_ptr <= stream_end_ptr )
	{
		switch(stream_ptr->token)
		{
		case WORD_TOKEN:
		{
			if(parse_command->type != SIMPLE_COMMAND)
			{
				free(parse_command);
				generate_new_node(&parse_command, wd_buffer_size, 1);
			}
			parse_command->type = SIMPLE_COMMAND;
			char **pos = word_position(parse_command);
			*pos = (char *) checked_malloc( (strlen(stream_ptr->content)+1)*sizeof(char) );
			strcpy(*pos, stream_ptr->content);
			update_word( &((parse_command)->u.word), &wd_buffer_size, wd_buffer_inc, wd_buffer_lim);
			break;
		}
		case PIPELINE_TOKEN:
		{
			command_t pipeline_node = 0;
			generate_new_node(&pipeline_node, wd_buffer_size, 0);
			pipeline_node->type = PIPE_COMMAND;
			if(is_subshell == 0 )
			{
				if(command_root->type == AND_COMMAND || command_root->type == OR_COMMAND || command_root->type == SEQUENCE_COMMAND)
				{
					pipeline_node->u.command[0] = command_root->u.command[1];
					command_root->u.command[1] = pipeline_node;
					generate_new_node(&(pipeline_node->u.command[1]), wd_buffer_size, 0);
					parse_command = pipeline_node->u.command[1];
				}
				else
				{
					command_t command_root_old = command_root;
					pipeline_node->u.command[0] = command_root_old;
					generate_new_node(&(pipeline_node->u.command[1]), wd_buffer_size, 0);
					parse_command = pipeline_node->u.command[1];
					command_root = pipeline_node;
				}
			}
			else
			{
				if(subshell_root->u.subshell_command->type == AND_COMMAND || subshell_root->u.subshell_command->type == OR_COMMAND
						|| subshell_root->u.subshell_command->type == SEQUENCE_COMMAND)
				{
					pipeline_node->u.command[0] = subshell_root->u.subshell_command->u.command[1];
					subshell_root->u.subshell_command->u.command[1] = pipeline_node;
					generate_new_node(&(pipeline_node->u.command[1]), wd_buffer_size, 0);
					parse_command = pipeline_node->u.command[1];
				}
				else
				{
					command_t command_root_old = subshell_root->u.subshell_command;
					pipeline_node->u.command[0] = command_root_old;
					generate_new_node(&(pipeline_node->u.command[1]), wd_buffer_size, 0);
					parse_command = pipeline_node->u.command[1];
					subshell_root->u.subshell_command = pipeline_node;
				}
			}

			break;
		}
		/*
		case AND_TOKEN:
		case OR_TOKEN:
		case SEMICOLON_TOKEN:// semicolon higher than AND and OR
		{
			command_t command_root_temp = 0;
			generate_new_node(&command_root_temp, wd_buffer_size, 0);
			switch(stream_ptr->token)
			{
				case AND_TOKEN:
				{
					command_root_temp->type = AND_COMMAND;
					break;
				}
				case OR_TOKEN:
				{
					command_root_temp->type = OR_COMMAND;
					break;
				}
				case SEMICOLON_TOKEN:
				{
					command_root_temp->type = SEQUENCE_COMMAND;
					break;
				}
				default:
				{
					break;
				}
			}
			if(is_subshell == 1)
			{
				command_root_temp->u.command[0] = subshell_root->u.subshell_command;
				generate_new_node(&(command_root_temp->u.command[1]), wd_buffer_size, 0);
				parse_command = command_root_temp->u.command[1];
				subshell_root->u.subshell_command = command_root_temp;
			}
			else
			{
				command_root_temp->u.command[0] = command_root;
				generate_new_node(&(command_root_temp->u.command[1]), wd_buffer_size, 0);
				parse_command = command_root_temp->u.command[1];
				command_root = command_root_temp;
			}
			break;
		}
		*/
		case AND_TOKEN:
		case OR_TOKEN:
		{
			command_t new_node = 0;
			generate_new_node(&new_node, wd_buffer_size, 0);
			switch(stream_ptr->token)
			{
				case AND_TOKEN:
				{
					new_node->type = AND_COMMAND;
					break;
				}
				case OR_TOKEN:
				{
					new_node->type = OR_COMMAND;
					break;
				}
				default:
				{
					break;
				}
			}
			if(is_subshell == 0 )
			{
				if(command_root->type == SEQUENCE_COMMAND)
				{
					new_node->u.command[0] = command_root->u.command[1];
					command_root->u.command[1] = new_node;
					generate_new_node(&(new_node->u.command[1]), wd_buffer_size, 0);
					parse_command = new_node->u.command[1];
				}
				else
				{
					command_t command_root_old = command_root;
					new_node->u.command[0] = command_root_old;
					generate_new_node(&(new_node->u.command[1]), wd_buffer_size, 0);
					parse_command = new_node->u.command[1];
					command_root = new_node;
				}
			}
			else
			{
				if(subshell_root->u.subshell_command->type == SEQUENCE_COMMAND)
				{
					new_node->u.command[0] = subshell_root->u.subshell_command->u.command[1];
					subshell_root->u.subshell_command->u.command[1] = new_node;
					generate_new_node(&(new_node->u.command[1]), wd_buffer_size, 0);
					parse_command = new_node->u.command[1];
				}
				else
				{
					command_t command_root_old = subshell_root->u.subshell_command;
					new_node->u.command[0] = command_root_old;
					generate_new_node(&(new_node->u.command[1]), wd_buffer_size, 0);
					parse_command = new_node->u.command[1];
					subshell_root->u.subshell_command = new_node;
				}
			}
			break;
		}
		case SEMICOLON_TOKEN:// semicolon higher than AND and OR
		{
			command_t command_root_temp = 0;
			generate_new_node(&command_root_temp, wd_buffer_size, 0);

			command_root_temp->type = SEQUENCE_COMMAND;

			if(is_subshell == 1)
			{
				command_root_temp->u.command[0] = subshell_root->u.subshell_command;
				generate_new_node(&(command_root_temp->u.command[1]), wd_buffer_size, 0);
				parse_command = command_root_temp->u.command[1];
				subshell_root->u.subshell_command = command_root_temp;
			}
			else
			{
				command_root_temp->u.command[0] = command_root;
				generate_new_node(&(command_root_temp->u.command[1]), wd_buffer_size, 0);
				parse_command = command_root_temp->u.command[1];
				command_root = command_root_temp;
			}
			break;
		}

		case IN_TOKEN:
		{
			if((stream_ptr-1)->token == RBKT_TOKEN)
			{
				subshell_root->input = (char*) checked_malloc((strlen((stream_ptr+1)->content)+1)*sizeof(char));
				strcpy(subshell_root->input,(stream_ptr+1)->content);
				++stream_ptr;
			}
			else
			{
				parse_command->input = (char*) checked_malloc((strlen((stream_ptr+1)->content)+1)*sizeof(char));
				strcpy(parse_command->input,(stream_ptr+1)->content);
				++stream_ptr;
			}
			break;
		}
		case OUT_TOKEN:
		{
			if((stream_ptr-1)->token == RBKT_TOKEN)
			{
				subshell_root->output = (char*) checked_malloc((strlen((stream_ptr+1)->content)+1)*sizeof(char));
				strcpy(subshell_root->output,(stream_ptr+1)->content);
				++stream_ptr;
			}
			else
			{
				parse_command->output = (char*) checked_malloc((strlen((stream_ptr+1)->content)+1)*sizeof(char));
				strcpy(parse_command->output,(stream_ptr+1)->content);
				++stream_ptr;
			}
			break;
		}
		case LBKT_TOKEN:
		{
			is_subshell = 1;
			parse_command->type = SUBSHELL_COMMAND;
			subshell_root = parse_command;
			generate_new_node(&(parse_command->u.subshell_command), wd_buffer_size, 2);
			parse_command = parse_command->u.subshell_command;
			break;
		}
		case RBKT_TOKEN:
		{
			is_subshell = 0;
			break;
		}
		default:
		{
			break;
		}
		}
		++stream_ptr;
	}
	update_read_stream(stream_start_ptr, stream_end_ptr);
	return command_root;
}


