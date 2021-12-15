/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#include <vector>
#include <iostream>

using namespace std;
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
vector<job> jobs = {}; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE]; 
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	   

	
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */

     struct sigaction new_sigaction = {
             {0}
     };
	
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */

    new_sigaction.sa_handler = &new_signal_handler;
    new_sigaction.sa_flags = SA_RESTART;

	try
	{
		if(sigaction(SIGINT, &new_sigaction, NULL)==-1){
			throw 1;
		}
		if(sigaction(SIGTSTP, &new_sigaction, NULL)==-1){
			throw 2;
		}
	}
	catch(int x)
	{
		switch (x)
		{
		case 1:
			perror("Error handeling SIGINT");
			break;
		case 2:
			perror("Error handeling SIGTSTP");
			break;
		}
	}

	/************************************/

	/************************************/
	// Init globals 


	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
			cout << "smash > ";
			fgets(lineSize, MAX_LINE_SIZE, stdin);
			strcpy(cmdString, lineSize);    	
			cmdString[strlen(lineSize)-1]='\0';
			//			// perform a complicated Command
			//if(!ExeComp(lineSize)) continue;
						// background command	
			if(!BgCmd(lineSize, &jobs)) continue; 
						// built in commands
			ExeCmd(&jobs, lineSize, cmdString);
			
			/* initialize for next line read*/
			lineSize[0]='\0';
			cmdString[0]='\0';
	}
    return 0;
}

