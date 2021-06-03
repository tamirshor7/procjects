#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <unistd.h>

using namespace std;

void ctrlZHandler(int sig_num)
{
    if (signal(SIGTSTP , ctrlZHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-Z handler");
        return;
    }
	std::cout << "smash: got ctrl-Z" << std::endl;
    SmallShell& smash = SmallShell::getInstance();
    if (!smash.foreground_command)
        return;
    smash.foreground_command->in_foreground = false;
    smash.jobs_list.addJob(smash.foreground_command, smash.foreground_command->pid, true);
//    if(sig_num == SIGTSTP)
//        smash.jobs_list.getJobById(smash.foreground_command->job_id)->is_stopped = true;
    if (kill(smash.foreground_command->pid, sig_num) == -1)
    {
        perror("smash error: kill failed");
        return;
    }
    std::cout << "smash: process " << smash.foreground_command->pid << " was stopped" << std::endl;
    smash.foreground_command = NULL;
}

void ctrlCHandler(int sig_num)
{
    if (signal(SIGINT , ctrlCHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-C handler");
        return;
    }
    std::cout << "smash: got ctrl-C" << std::endl;
    SmallShell& smash = SmallShell::getInstance();
    if (!smash.foreground_command)
        return;
    if (kill(smash.foreground_command->pid, sig_num) == -1)
    {
        perror("smash error: kill failed");
        return;
    }
    std::cout << "smash: process " << smash.foreground_command->pid << " was killed" << std::endl;
    smash.foreground_command = NULL;
}

void alarmHandler(int sig_num)
{
    SmallShell& smash = SmallShell::getInstance();
    cout << "smash: got an alarm" << endl;
    smash.jobs_list.removeFinishedJobs(); //in case the timed out command is a background command (in jobs list) that finished before the alarm
    smash.timed_commands.clear();
    auto timed_out_command = smash.timed_commands.timedOutCommand();
    if (timed_out_command)
    {
        cout << "smash: " << timed_out_command->cmd_line << " timed out!" << endl;
        if (kill(timed_out_command->pid, SIGKILL) == -1)
        {
            perror("smash error: kill failed");
            return;
        }
        smash.timed_commands.commands.erase(timed_out_command->pid);
    }
    int next_alarm = smash.timed_commands.nextAlarm();
    //cout << "next alarm in " << next_alarm << " seconds\n";
    if (next_alarm < 0)
        return;
    alarm(next_alarm);
}
