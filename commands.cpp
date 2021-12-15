//		commands.cpp
//this started off as a c file but I wanted to be able to dynamicaly allocate
//********************************************
#include "commands.h"
#include <iostream>
#include <algorithm>

using namespace std;
int curr_jid = 1;

//********************************************

vector<job>::iterator find_job(int jid,vector<job> jobs){
	vector<job>::iterator it = jobs.begin();
	while(it!=jobs.end()){
		if(it->jid == jid){
			return it;
		}
		it++;
	}
	return it;
}

bool is_digits(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

int does_job_exist(int jid, vector<job> jobs){
	if(jobs.empty()) return 0;
	vector<job>::iterator it = jobs.begin();
	while(it!=jobs.end()){
		if(it->jid == jid) return 1;
		it++;
	}
	return 0;
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(),
		 ending.length(), ending));
    }
	return false;
}


int print_curr_directory (char *pwd){
	if (getcwd(pwd, MAX_LINE_SIZE)!=NULL){
		printf("smash>%s\n", getcwd(pwd, MAX_LINE_SIZE));
	}
	else{
		return 1;
	}
	return 0;
}



int jobs_status_update(){
	if(jobs.empty()) return 0;
	vector<job>::iterator it = jobs.begin(); 
	int *status_p;
	while (it != jobs.end()){
		pid_t changed_proc = waitpid(it->pid, status_p, 
		WNOHANG| WUNTRACED| WCONTINUED);
		if(changed_proc==(-1)){
			return -1;
		}
		if(changed_proc!=0){
			if(WIFEXITED(*status_p) || WIFSIGNALED(*status_p)){
				it = jobs.erase(it);
				continue;
			}
			else if(WIFSTOPPED(*status_p)){
				it->stopped = true;
			}
			else if(WIFCONTINUED(*status_p)){
				it->stopped = false;	
			}
		}
		it++;
	}
	return 0;
}
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************


int ExeCmd(char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char delimiters[4] = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	int command_p = 0;
	static int hist_pointer = 0;
	int oldest_command = (hist_pointer/(HIST_SIZE+1))%(HIST_SIZE+1);
	//the above line makes sure the oldest command is updated in case there are
	//more than 50 commands
	static char cmd_hist[HIST_SIZE][MAX_LINE_SIZE] = {'\0'};
	string delimitered_command = NULL;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL){ 
			num_arg++; 
			strcpy(cmd_hist[hist_pointer%(HIST_SIZE+1)]+command_p,args[i]);
			//this can be handeled a lot smoother by just copying the cmdString
			//outside the for loop
			command_p += strlen(args[i]);
			delimitered_command.append(args[i]);
		}
 
	}
	//after updating the cmd history array:
	hist_pointer += MAX_LINE_SIZE;
	static char prev_dir[MAX_LINE_SIZE] ;
	if(hist_pointer==0){// want strcpy to be called only the first time this 
	//function is called
		strcpy(prev_dir,getcwd(pwd, MAX_LINE_SIZE));
	}
	
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) //changes directory
	//should check if the first arg is what should be passed to this function
	{
		if(!strcmp(args[1],"-")){
			if(chdir(prev_dir)==0){//move to prev dir
				if (print_curr_directory(pwd)!=0){
					perror("smash error:>");
				}
			}
			else{
				perror("smash error:>");
			}
		}
		else if (chdir(args[1])==0){
			strcpy(prev_dir,getcwd(pwd, MAX_LINE_SIZE));
		}
		else{
			perror("smash error:>");
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) //this prints the current working directory
	{	
		if(print_curr_directory(pwd)!=0){
			perror("smash error:>");
		}
		return 0;
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		//pid_t pid 
		if(num_arg != 0 || !jobs_status_update()){
			perror("smash error:>"); 
		}
		if(jobs.empty()) return 0;
		vector<job>::iterator it = jobs.begin();
		for(int i=1 ; it!=jobs.end(); it++){
			cout<<'['<<i<<"] "<<it->command<<": "<<it->pid<<' '<<it->time
			<<" secs";
			if(it->stopped){
				cout<<" (Stopped)";
			}
			cout<<endl;
		}
		
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		pid_t pid = getpid();//this function is always successful based on
		//Linux Programmer's Manual
		//https://man7.org/linux/man-pages/man2/getpid.2.html
		printf ("smash pid is %d\n", pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		int curr_jid;
		try{
			if(!jobs_status_update()||num_arg>1||jobs.empty()){
				throw 1;
			}
		}
		catch(int x){
			switch (x)
			{
			case (1):
				perror("smash error:>");
				break;
			default:
				break;
			}
		}
		switch (num_arg)
		{
		case 0:
			curr_jid = jobs.back().jid;
			break;
		case 1:
			curr_jid = atoi(args[1]);
		default:
			break;
		}
		if(does_job_exist(curr_jid, jobs)){
			vector<job>::iterator it = find_job(curr_jid, jobs);
			cout<<it->command<<endl;
			if(it->stopped){//if it is stopped continue in the foreground
				if(kill(it->pid, SIGCONT) == -1){//if stopped continue
					perror("smash error:>");
				}
			}
			pid_t status;
			pid_t waitpid_res = waitpid(it->pid, &status, WSTOPPED);
			if(waitpid_res==-1){
				perror("smash error:>");
			}
			else if(waitpid_res>0 && WIFEXITED(status)){
				jobs.erase(it);
			}
		}
		else{
			perror("smash error:> no such job");
		}
		return 0;
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		int curr_jid;
		try{
			if(!jobs_status_update()||num_arg>1||jobs.empty()){
				throw 1;
			}
		}
		catch(int x){
			switch (x)
			{
			case (1):
				perror("smash error:>");
				break;
			default:
				break;
			}
		}
		switch (num_arg)
		{
		case 0:
			curr_jid = jobs.back().jid;
			break;
		case 1:
			curr_jid = atoi(args[1]);
		default:
			break;
		}
		if(does_job_exist(curr_jid, jobs)){
			vector<job>::iterator it = find_job(curr_jid, jobs);
			cout<<it->command<<endl;
			if(it->stopped){//if it is stopped continue in the foreground
				if(kill(it->pid, SIGCONT) == -1){//if stopped continue
					perror("smash error:>");
				}
			}
			else{
				perror("smash error:> job already running");
			}
		}
		else{
			perror("smash error:> job doesn't exist");
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		//signal()
		
		try{
			if(!jobs_status_update()||num_arg>1||jobs.empty()){
				throw 1;
			}
		}
		catch(int x){
			switch (x)
			{
			case (1):
				perror("smash error:>");
				break;
			default:
				break;
			}
		}
		switch (num_arg)
		{
		case 0:
			exit(EXIT_SUCCESS);
			break;
		case 1:
			if(!strcmp(args[1],"kill")){
				vector<job>::iterator it = jobs.begin();
				for(int i=1 ; it!=jobs.end(); it++){
					cout<<'['<<i<<"] "<<it->command<<": "<<"sending SIGTERM...";
					if(kill(it->pid, SIGTERM)==-1){
						perror("smash error:>error sending SIGTERM signal");
						exit(EXIT_FAILURE);
					}
					else{
						pid_t waitpid_res = 0;
						time_t start_time = time(NULL);
						while(waitpid_res==0 &&difftime(time(nullptr)
						,start_time) < 5){
							waitpid_res = waitpid(it->pid, NULL, WNOHANG);
						}
						switch(waitpid_res){
							case(0):
								cout<<" (5 sec passed) Sending SIGKILL...";
								if(kill(it->pid, SIGKILL)==-1){
									perror("smash error:>error sending SIGTERM signal");
									exit(EXIT_FAILURE);
								}
								else{
									cout<< "Done."<<endl;
								}
								break;
							case(-1):
								perror("smash error:> waitpid invalid result");
								break;
							default:
								cout<<"Done."<<endl;
						}
					}
				}
				exit(EXIT_SUCCESS);
			}
			else{
				cout << "smash error:>" << "invalid argument"<< endl;
			}
		default:
			break;
		}

	} 
	/*************************************************/
    else if (!strcmp(cmd, "diff"))
    {
        //signal()
        if( num_arg != 2){
            illegal_cmd = true;
        }
        else {
            int file1, file2;

            if ((file1 = open(args[1], O_RDONLY)) == -1) {
                fprintf(stderr, "smash error: > ");
                perror("file does not exist");
            }

            if ((file2 = open(args[2], O_RDONLY)) == -1) {
                fprintf(stderr, "smash error: > ");
                perror("file does not exist");
            }

            FILE *fp1 = fopen(args[1], "r");
            FILE *fp2 = fopen(args[2], "r");

            char ch1 = getc(fp1);
            char ch2 = getc(fp2);

            while (ch1 != EOF && ch2 != EOF) {
                if (ch1 != ch2) {
                    cout << "1" << endl;
                }

                ch1 = getc(fp1);
                ch2 = getc(fp2);
            }
            cout << "0" << endl;
        }

    }
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	//here args[1] is the signum and args[2] is the pid to kill
	{
		if(!is_digits(args[1])) perror("smash error:>");
		else if(!does_job_exist(std::atoi(args[2]),jobs)){//using atoi instead
		// of stoi
		//for none-C++ C support.)
			cout<< "smash error:> kill" << args[2] << " - job does not exist"
			<<endl;
			return 1;
		}
		else{
			vector<job>::iterator job_it = find_job(std::atoi(args[2]),jobs);
			if(job_it == jobs.end()) perror ("smash error:>");
			else{
				//try sending a kill signal
				if(kill(job_it->pid, atoi(args[1])) == -1){
					cout<< "smash error:> kill" << args[2] <<
					" - cannot send signal"<<endl;
					return 1;
				}
				else{
					return 0;
				}
			}
		}

   		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
   		for (int i = 0; i < HIST_SIZE; i++)
		   {
			   for (int j = 0; j < MAX_LINE_SIZE; j++)
			   {
				   cout << (cmd_hist[(oldest_command+i)%(HIST_SIZE+1)][j]) <<
					endl;
			   }
			   printf("\n");
		   }
		   
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	char delimiters[4] = " \t\n";  
	string delimitered_command = NULL;
	
	for(int i=0;i<MAX_ARG;i++){
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL){ 
			delimitered_command.append(args[i]);
		}
	}
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
					
					/* 
					your code
					*/
        	case 0 :
                	// Child Process
               		setpgrp();
					
			        // Add your code here (execute an external command)
					
					/* 
					your code
					*/
			
			default:
					// Add your code here
					if(!hasEnding(delimitered_command,bg_char)){
						jobs.push_back({pID,static_cast<int>(time(nullptr)),
						false, args[0], curr_jid++});
					}
					break;
					/* 
					your code
					*/
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
// int ExeComp(char* lineSize)
// {
// 	char ExtCmd[MAX_LINE_SIZE+2];
// 	char *args[MAX_ARG];
//     if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) ||
// 	 (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || 
// 	 (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || 
// 	 (strstr(lineSize, "|&")))
//     {
// 		// Add your code here (execute a complicated command)
					
// 		/* 
// 		your code
// 		*/
// 	} 
// 	return -1;
// }
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs)
{

	char* Command;
	char delimiters[4] = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)
					
		/* 
		your code
		*/
		
	}
	return -1;
}
