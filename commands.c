#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "smash.h"
#include "history.h"
#include "errno.h"

extern int errno;

extern struct history* hist;

char histStr[4096];
int j;

void executeCommand(char* str)
{
	char c;
	int i = 0;
	int ex = 0;
	while((c = str[i])!= '\0')
	{
		histStr[i] = c;
		i++;
	}
	while(i < MAXLINE)
	{
		histStr[i] = '\0';
		i++;
	}
	char* tokens = strtok(str," "); // tokenizer
	i = 0; // index to send to external

	if(strncmp(tokens,"exit",5) == 0)
	{
		clear_history(hist);
		free(hist);
		exit(0);
	}
	else if(strncmp(tokens, "cd", 3) == 0)
	{
		char* dir; // temp var to hold original input
		char* temp; // temp var to hold extra data if any
		if((dir = strtok(NULL, " ")) == NULL)
		{
			fputs("Error: Invalid command. Enter a single valid directory.\n", stderr);
			ex = 1;
		}
		else if((temp = strtok(NULL, " ")) != NULL)
		{
			fputs("Error: Invalid command. Enter a single valid directory.\n", stderr);
			ex = 1;
		}
		else
		{
			if(chdir(dir) == -1)
			{
				perror(dir);
				ex = errno;
			}
			else
			{
				fputs(dir, stderr);
				fputs("\n", stderr);
				ex = 0;
			}
		}
		dir = NULL;
		add_history(hist,histStr,ex);
	}
	else if(strncmp(tokens, "history", 8)==0)
	{
		ex = 0;
		add_history(hist, histStr,ex);
		print_history(hist);
	}
	else
	{
		char* args[1026];
		int numPipes = 0;
		int hasIn = 0;
		int hasOut = 0;
		char* inFile = NULL;
		char* outFile = NULL;
		while(tokens != NULL)
		{
			if(strncmp(tokens, "<", 1)==0)
			{
				hasIn++;
				inFile = strtok(NULL, " ");
			}
			else if(strncmp(tokens, ">", 1)==0)
			{
				hasOut++;
				outFile = strtok(NULL, " ");
			}
			else
			{
				args[i] = tokens;
				if(strncmp(tokens,"|",1)==0)
				{
					numPipes++;
				}
				i++;
			}
			tokens = strtok(NULL, " ");
		}

		args[i] = NULL;

		ex = executeExternalCommand(i, args, numPipes, hasIn, hasOut, inFile, outFile);
		add_history(hist,histStr,ex); 
	}

}

int executeExternalCommand(int argc, char* argv[], int numPipes, int hasIn, int hasOut, char* iFile, char* oFile)
{
	int exStatus = 0;
	int pid;
	char* args[1026];
	j = 0;
	int inDupe = dup(0);
	int outDupe = dup(1);
	int inFile;
	int outFile;
	
	if(hasIn)
	{
		inFile = open(iFile, O_RDONLY);
	}
	else
	{
		inFile = dup(inDupe);
	}

	for(int i=0; i <= numPipes; i++)
	{
		getArgs(args, argc, argv, numPipes);

		char* file = args[0];

		dup2(inFile,0);
		close(inFile);

		if(i == numPipes)
		{
			if(hasOut)
			{
				outFile = open(oFile, O_WRONLY);
			}
			else
			{
				outFile = dup(outDupe);
			}
		}
		else
		{
			int fd[2];
			int piper = pipe(fd);
			
			if(piper < 0)
			{
				fputs("Pipe failed, try again\n", stderr);
			}
			
			outFile = fd[1];
			inFile = fd[0];
		}

		dup2(outFile,1);
		close(outFile);

		pid = fork();

		if(pid < 0)
		{
			fputs("Fork failed, try again\n", stderr);
		}

		if(pid == 0)
		{
			exStatus = execvp(file,args);
			if(exStatus == -1)
			{
				fputs("Invalid Command or invalid arguments, try again\n", stderr);
			}

			exStatus = errno;
			exit(exStatus);
		}
	}
	
	dup2(inDupe,0);
	dup2(outDupe,1);
	close(inDupe);
	close(outDupe);

	if(pid > 0)
	{
		wait(&exStatus);
	}

	return exStatus;
}



void getArgs(char* args[], int argc, char* argv[], int numPipes)
{
	int i = 0;
	while(j < argc)
	{
		if(strncmp(argv[j], "|", 1) == 0)
		{
			j++;
			break;
		}
		else
		{
			args[i] = argv[j];
			i++;
			j++;
		}
	}
}


