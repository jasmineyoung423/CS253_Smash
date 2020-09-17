#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include <errno.h>

extern int errno;

static int count = 0;
static int callInit = 0;
static char charcommd[10][MAXLINE];


struct history* init_history(int capacity)
{
	struct history* hist = (struct history*) malloc(sizeof(struct history));
	hist->capacity = capacity;
	hist->offset = 0;
	hist->entries = (struct cmd**) malloc(capacity*sizeof(struct cmd*));
	for(int i = 0; i < capacity; i++)
	{
		hist->entries[i] = (struct cmd*) malloc(sizeof(struct cmd));
	}
	callInit = 1;
	return hist;
}

void add_history(struct history* hist, char* cmd, int exit)
{
	if(!callInit)
	{
		fputs("Cannot add command, History has not been created.\n",stderr);	
	}
	else
	{
		
		free(hist->entries[hist->offset]);
	
		struct cmd* comm = (struct cmd*) malloc(sizeof(struct cmd));
		int offset = hist->offset;
		char c;
		int i = 0;
		int exstatus = exit;
		while((c = cmd[i])!= '\0')
		{
			charcommd[offset][i] = c;
			i++;
		}
		while(i < MAXLINE)
		{
			charcommd[offset][i] = '\0';
			i++;
		}
		if(strncmp(cmd, "cd", 2) == 0)
		{
			exstatus = exit;
		}
		else if (strncmp(cmd, "history", 7)==0)
		{
			exstatus = 0;
		}
		else
		{
			exstatus = exit;
		}
		//fprintf(stderr, "address: %p\n", &comm);
		comm->cmd = charcommd[offset];
		comm->exitStatus = exstatus;
		hist->entries[offset] = comm;
		offset++;
		if(offset >= hist->capacity)
		{
			offset = offset%hist->capacity;
		}
		hist->offset = offset;
		count++;
	}
}

void print_history(struct history* hist)
{
	if(!callInit)
	{
		fputs("Cannot print commands, History has not been created.\n", stderr);
	}
	else
	{
		struct cmd* command;
		int start = 0;
		int num = count-10;
		if(num <= 0)
		{
			num = 1;
		}
		if(count >= hist->capacity)
		{
			start = hist->offset;
		}
		for(int i = 0; i < hist->capacity; i++)
		{
			if(start >= hist->capacity)
			{
				start = 0;
			}
			if(!(i >= count))
			{
				command = hist->entries[start];
				if(command->cmd!=NULL)
				{
					fprintf(stdout, "%d [%d] %s\n",num, command->exitStatus, command->cmd);
				}
			}
			start++;
			num++;
		}
	}
}


void clear_history(struct history* hist)
{
	if(!callInit)
	{
		fputs("Cannot clear commands, History has not been created.\n", stderr);
	}
	else
	{
		for(int i = 0; i < hist->capacity; i++)
		{
			free(hist->entries[i]);
		}
		free(hist->entries);
	}
}




