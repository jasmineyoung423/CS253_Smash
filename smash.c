#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "smash.h"
#include "commands.c"
#include "history.h"
#include "history.c"


#define MAXLINE 4096

struct history* hist;
int keepRunning = 1;
int gotIt = 0;

int main(void)
{
	signal(SIGINT, signalHandler);
	hist = init_history(10);
	char bfr[MAXLINE]; // holds the line from stdin
	while(keepRunning)
	{
		fputs("$ ", stderr);
		while(fgets(bfr,MAXLINE,stdin) != NULL)
		{
			if(gotIt)
			{
				fputs("^C",stderr);
				fputs("\n$ ",stderr);
				gotIt = 0;
			}
			else
			{
				bfr[strlen(bfr) -1] = '\0'; // replace newline with null
				executeCommand(bfr);
				fputs("$ ", stderr);
			}
		}
	}
	return 0;
}


void signalHandler(int sigNumber)
{
	signal(SIGINT, SIG_IGN);
	gotIt = 1;
	signal(SIGINT, signalHandler);
}









































