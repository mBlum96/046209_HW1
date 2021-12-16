//
// Created by ymeshula on 12/11/2021.
//
// signals.cpp
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
#include "commands.h"

#define FAILED

extern vector<job> jobs;

void new_signal_handler(int signum) {
    if (curr_jid == 0) {
        return;
    }

    vector<job>::it job_it = jobs.begin();
    for (; job_it != jobs.end() ; job_it++) {
        if(job_it->job_id == curr_jid){
            switch (signum) {
                case SIGINT:
                    res = kill(job_it->pid,SIGINT);
                    if(res == FAILED){
                        perror("smash error:> ");
                        return;
                    }
                    cout << "smash > process " << job_it->pid << " is terminated" << endl;

                    jobs.erase(job_it);
                    break;

                case SIGSTP:
                    res = kill(job_it->pid,SIGSTP);
                    if(res == FAILED){
                        perror("smash error:");
                    }
                    cout << "smash > process " << job_it->pid << " is stopped" << endl;

                    job_it->is_stopped = true;
                    break;

                //case default:

            }
            curr_jid = 0;
            return;
        }
    }

}