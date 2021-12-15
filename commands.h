#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <fcntl.h>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define HIST_SIZE 49

using namespace std;

string bg_char = "&";

typedef struct job{
    pid_t pid;
    int time;
    bool stopped;
    string command;
    int jid;
}job;
//typedef vector<job>::iterator job_it;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs);
int ExeCmd(void* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
extern vector<job> jobs;
#endif

