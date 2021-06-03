#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <signal.h>
#include <sys/wait.h>
#include <iomanip>
#include <fcntl.h>
#include <assert.h>
#include "Commands.h"
#include <list>

#define WHITESPACE " "
#define PATH_MAX 1024
using namespace std;
#define UNLIMITED_ARGS -1

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

///*******************************************************   PARSING FUNCTIONS   *******************************************************///


string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{//get rid of spaces around s (not inside)
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}


void removeAmp(string& str)
{// remove last ampersand sign of a string
    int last_char_pos = str.find_last_not_of(WHITESPACE);
    if (last_char_pos != (int)string::npos && str[last_char_pos] == '&')
        str = str.substr(0,last_char_pos);
}

void removeAllAmps(string& str)
{//remove all ampersand signs from a string
    int first_amp = str.find_first_of("&");
    while(first_amp != string::npos)
    {
        str.replace(first_amp,1,"");
        first_amp = str.find_first_of("&");
    }
}
pair<int,int>  BuiltInCommand::parseArgs(string cmd_line, int arg_num)
{

    assert(cmd_line.find_first_not_of(WHITESPACE) != string::npos); //string shouldn't be empty - at least command descriptor should be included

    //ignore ampersand
    removeAmp(cmd_line);

    //set input to only include args (dispose of command descriptor), with no spaces on start of string
    pair<int,int> ans = pair<int,int>(!arg_num,0);
    //set input to only include args (dispose of command descriptor), with no spaces on start of string
    cmd_line = _trim(cmd_line);
    int args_start = cmd_line.find_first_of(WHITESPACE);
    if(args_start != (int)string::npos)
    {
        cmd_line = cmd_line.substr(args_start);
        cmd_line = _trim(cmd_line);

    }
    else
    {
        return ans;
    }
    if(cmd_line.empty())
        return ans;

    //from here on, at least one argument exists
    int args_count = 0;
    int start_runner = 0;
    int end_runner = cmd_line.find_first_of(WHITESPACE);

    while(end_runner != (int)string::npos)
    {
        if(args_count<arg_num || arg_num == UNLIMITED_ARGS)
            args.push_back(cmd_line.substr(start_runner,end_runner-start_runner));
        args_count++;
        start_runner = cmd_line.find_first_not_of(WHITESPACE,end_runner);
        assert(start_runner != (int)string::npos); //cmd_line shouldn't end with space because of trim activation
        end_runner = cmd_line.find_first_of(WHITESPACE,start_runner);
    }

    //handle addition of last arg
    if(args_count<arg_num || arg_num==UNLIMITED_ARGS)
        args.push_back(cmd_line.substr(start_runner));
    args_count++;

    ans.first = arg_num == (signed)args.size() || arg_num == UNLIMITED_ARGS;
    ans.second = args_count;
    return ans;

}

pair<string, string> parseRedirectionPipe(const char* cmd_line, bool* append_or_std_err)
{//split pipe/redirection cmd to command1 and command2/path. flag states whether to append in redirection mode,
    // and where to output to in pipe mode. This function relies on the fact that only up to one redirection/pipe symbol
    // would appear in a single command.

    pair<string,string> split;

    string input = string(cmd_line);
    int delim_idx = input.find_first_of(">|");
    assert(delim_idx != (int)string::npos);
    assert(input.size()>= delim_idx+1); //the first '>' or '|' isn't the last character in the string

    if(input[delim_idx+1] == '>' || input[delim_idx+1] == '&') //case of "<<", rather than "<". no need to increment delim_idx
    {
        *append_or_std_err = true;
    }

    split.first = input.substr(0,delim_idx);
    int path_start_idx = input.find_first_not_of(" >&|",delim_idx);
    assert (path_start_idx != (int)string::npos); //according to piazza the case of a redirection with no args would not be tested
    assert(input.find_last_not_of(WHITESPACE,path_start_idx) != string::npos); //at least first indx in search is not space
    split.second = input.substr(path_start_idx);
    removeAllAmps(split.first);
    removeAllAmps(split.second);
    split.first  = _trim(split.first);
    split.second = _trim(split.second);
    return split;
}

///*******************************************************   COMMANDS EXECUTION   *******************************************************///


Command * SmallShell::CreateCommand(const char* cmd_line)
{
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if(cmd_s.find_first_of(">") != string::npos)
    {
        return new RedirectionCommand(cmd_line, this);
    }
    if(cmd_s.find_first_of("|") != string::npos)
    {
        return new PipeCommand(cmd_line);
    }
    if(firstWord.compare("cat") == 0)
    {
        return new CatCommand(cmd_line);
    }
    if (firstWord.compare("pwd") == 0 || !firstWord.compare("pwd&"))
    {
        return new GetCurrDirCommand(cmd_line);
    }
    if (firstWord.compare("cd") == 0 || !firstWord.compare("cd&"))
    {
        return new ChangeDirCommand(cmd_line, pwd);
    }
    if (firstWord.compare("chprompt") == 0)
    {
        return new ChangePromptCommand(cmd_line);
    }
    if (firstWord.compare("showpid") == 0 || firstWord.compare("showpid&") == 0)
    {
        return new ShowPidCommand(cmd_line);
    }
    if (firstWord.compare("jobs") == 0 || firstWord.compare("jobs&") == 0)
    {
        SmallShell& smash = SmallShell::getInstance();
        return new JobsCommand(cmd_line, &(smash.jobs_list));
    }
    if (firstWord.compare("quit") == 0)
    {
        SmallShell& smash = SmallShell::getInstance();
        QuitCommand(cmd_line, &(smash.jobs_list)).execute();
    }
    if (firstWord.compare("kill") == 0)
    {
        SmallShell& smash = SmallShell::getInstance();
        return new KillCommand(cmd_line, &(smash.jobs_list));
    }
    if (firstWord.compare("fg") == 0)
    {
        SmallShell& smash = SmallShell::getInstance();
        return new ForegroundCommand(cmd_line, &(smash.jobs_list));
    }
    if (firstWord.compare("bg") == 0)
    {
        SmallShell& smash = SmallShell::getInstance();
        return new BackgroundCommand(cmd_line, &(smash.jobs_list));
    }
    else
        return new ExternalCommand(cmd_line, 0);
    return nullptr;
}


void SmallShell::executeCommand(const char *cmd_line)
{
    jobs_list.removeFinishedJobs();
    Command *cmd = CreateCommand(cmd_line);
    if (cmd)
        cmd->execute();
    if (cmd->in_foreground)
        delete cmd;
}


///*******************************************************   COMMANDS.CPP IMPLEMENTATION   *******************************************************///


///COMMAND class c'tor

Command::Command(const char* cmd_line)
{
    int i = 0;
    while (cmd_line[i])
    {
        this->cmd_line[i] = cmd_line[i];
        i++;
    }
    this->cmd_line[i] = '\0';
}


///EXTERNAL COMMAND execution

void ExternalCommand::execute()
{
    bool is_timed_command = false;
    int duration, i;
    char* cmd_line_cpy;
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
    if (firstWord == "timeout")
    {
        is_timed_command = true;
        string str;
        for (i = 0; i < (int)strlen(cmd_line); i++)
            str.push_back(cmd_line[i]);
        int begin = str.find_first_of(WHITESPACE); //ugly and temporary, remove first 2 words from cmd_line_cpy
        str = str.substr(begin);
        firstWord = str.substr(0, cmd_s.find_first_of(" \n"));
        duration = stoi(firstWord);
        begin = str.find_first_not_of(WHITESPACE);
        str = str.substr(begin);
        begin = str.find_first_of(WHITESPACE);
        str = str.substr(begin);
        cmd_line_cpy = new char[str.size()+1];
        for (i = 0; i < (signed)str.size(); i++)
            cmd_line_cpy[i] = str[i];
        cmd_line_cpy[i] = '\0';
    }
    else
    {
        cmd_line_cpy = new char[strlen(cmd_line)+1];
        strcpy(cmd_line_cpy, cmd_line);
    }
    pid_t pid = fork();
    if (pid == -1)
        perror("smash error: fork failed");
    SmallShell& smash = SmallShell::getInstance();
    if (pid == 0) //child code
    {
        setpgrp();
        _removeBackgroundSign(cmd_line_cpy);
        char* argv[] = {(char*)"bash", (char*)"-c", cmd_line_cpy, nullptr};
        this->pid = getpid();
        if(this->pid == -1)
            perror("smash error: getpid failed");
        if (execv("/bin/bash", argv) == -1)
        {
            perror("smash error: execv failed");
            exit(0); //exit child's process
        }
    }
    else //father code
    {
        delete[] cmd_line_cpy;
        this->pid = pid;
        if (is_timed_command)
        {
            smash.timed_commands.addTimedCommand(duration, pid, cmd_line);
            int next_alarm = smash.timed_commands.nextAlarm();
            alarm(next_alarm);
        }
        if (!_isBackgroundComamnd(cmd_line)) //smash should wait for process to finish (foreground command)
        {
            int status;
            smash.foreground_command = this;
            waitpid(pid, &status, WUNTRACED);
            smash.foreground_command = NULL;
            if (is_timed_command)
                smash.timed_commands.commands.erase(pid); ////need to add this to fg as well (i think)
        }
        else //background command, should enter to jobslist
        {
            in_foreground = false;
            smash.jobs_list.addJob(this, pid);
        }
    }
}


///Built in commands:

///pwd command

void GetCurrDirCommand::execute()
{
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    cout << cwd << endl;
}


///cd command

ChangeDirCommand::ChangeDirCommand(const char* cmd_line, const std::string& pwd) : BuiltInCommand(cmd_line), plastPwd(pwd)
{
    pair<int,int> args_parse = parseArgs(string(cmd_line), 1);
    if (args_parse.second > 1)
        throw CdArgsNum();
}


void ChangeDirCommand::execute()
{
    string new_path;
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    if (args.empty())
        new_path = string(cwd);
    else
        new_path = args[0];
    if((!args.empty()) && args[0] == "-")
    {
        if(plastPwd.empty())
            throw NoPwd();
        new_path = plastPwd;
    }
    int syscall_res = chdir(new_path.c_str());
    if(syscall_res == -1)
    {
        perror("smash error: chdir failed");
        return;
    }
    SmallShell& smash = SmallShell::getInstance();
    smash.pwd = string(cwd);
}


///chprompt command

ChangePromptCommand::ChangePromptCommand(const char *cmd_line) :BuiltInCommand(cmd_line)
{
    parseArgs(string(cmd_line), 2);
}


void ChangePromptCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();
    if (args.size() > 0)
        smash.prompt = args[0] + "> ";
    else
        smash.prompt = "smash> ";
}


///showpid command

void ShowPidCommand::execute()
{
    SmallShell& inst = SmallShell::getInstance();
    cout << "smash pid is " << inst.shell_pid << endl;
}


///Jobs list implementation

///c'tor
JobsList::JobEntry::JobEntry(int job_id, pid_t pid, time_t init_time, bool is_stopped, ExternalCommand* cmd_ptr) :
        job_id(job_id), pid(pid), init_time(init_time), is_stopped(is_stopped), cmd_ptr(cmd_ptr)
{
    int i = 0;
    while (cmd_ptr->cmd_line[i])
    {
        i++;
    }
    cmd_ptr->cmd_line[i] = '\0';
}

///operator =
JobsList::JobEntry &JobsList::JobEntry::operator=(const JobsList::JobEntry &job_to_copy)
{
    job_id = job_to_copy.job_id;
    pid = job_to_copy.pid;
    init_time = job_to_copy.init_time;
    is_stopped = job_to_copy.is_stopped;
    int i = 0;
    while (job_to_copy.cmd_ptr->cmd_line[i])
    {
        this->cmd_ptr->cmd_line[i] = job_to_copy.cmd_ptr->cmd_line[i];
        i++;
    }
    this->cmd_ptr->cmd_line[i]= '\0';
    return *this;
}

///copy c'tor
JobsList::JobEntry::JobEntry(const JobsList::JobEntry &job_to_copy)
{
    job_id = job_to_copy.job_id;
    pid = job_to_copy.pid;
    init_time = job_to_copy.init_time;
    is_stopped = job_to_copy.is_stopped;
    this->cmd_ptr = job_to_copy.cmd_ptr;
}


///JobsList methods

void JobsList::addJob(ExternalCommand* cmd_ptr, pid_t pid, bool isStopped)
{
    time_t init_time;
    time(&init_time);
    int job_id = cmd_ptr->job_id;
    if(cmd_ptr->job_id == -1)
    {
        if (jobs_list.empty())
            job_id = 1;
        else
        {
            job_id = jobs_list.rbegin()->first + 1;
        }
        cmd_ptr->job_id = job_id;
    }
    JobEntry new_job = JobEntry(job_id, pid, init_time, isStopped, cmd_ptr);
    jobs_list.insert(pair<int,JobEntry>(job_id,new_job));
}

void JobsList::printJobsList()
{
    time_t sec_elapsed;
    time_t curr_time;
    time(&curr_time);
    for (auto const& job : jobs_list)
    {
        sec_elapsed = difftime(curr_time, job.second.init_time);
        cout << '[' << job.second.job_id << "] " << job.second.cmd_ptr->cmd_line << " : " << job.second.pid << " " << sec_elapsed << " secs";
        if (job.second.is_stopped)
            cout << " (stopped)";
        cout << endl;
    }
}

void JobsList::removeFinishedJobs()
{
    if (this->jobs_list.empty())
        return;
    int status, return_pid;
    std::list<int> jobid_to_remove, timed_cmds_to_remove;
    for (auto it = jobs_list.begin(); it != jobs_list.end(); ++it)
    {
        return_pid = waitpid(it->second.pid, &status, WNOHANG);
        if (return_pid == -1) //failed
        {
            return;
            //perror("smash error: waitpid failed");
        }
        else if (return_pid == it->second.pid)
        {
            delete it->second.cmd_ptr; // deallocate job finished in background command instance
            jobid_to_remove.push_back(it->first);
            timed_cmds_to_remove.push_back(it->second.pid);
        }
    }
    for (auto const& job_id : jobid_to_remove)
        jobs_list.erase(job_id);
    SmallShell& smash = SmallShell::getInstance();
    for (auto pid_to_remove : timed_cmds_to_remove) //in case there are timed out commands in jobs list that finished before their alarm
    {
        smash.timed_commands.commands.erase(pid_to_remove);
    }
}

JobsList::JobEntry *JobsList::getLastJob(int *lastJobId)
{
    if (jobs_list.empty())
        return nullptr;
    else
    {
        if (lastJobId)
            *lastJobId = jobs_list.rbegin()->first;
        return &(jobs_list.rbegin()->second);
    }
}

JobsList::JobEntry *JobsList::getLastStoppedJob(int *jobId)
{
    if (jobs_list.empty())
        return nullptr;
    else
    {
        for (auto rit = jobs_list.rbegin(); rit != jobs_list.rend(); rit++) //iterate backwards
        {
            if (rit->second.is_stopped)
            {
                if (jobId)
                    *jobId = rit->first;
                return &(rit->second);
            }
        }
        return nullptr;
    }
}

void JobsList::removeJobById(int jobId)
{
    if (jobs_list.empty())
        return;
    else
    {
        jobs_list.erase(jobId);
    }
}

void JobsList::killAllJobs()
{
    for (auto const& job : jobs_list)
    {
        if (kill(job.second.pid, SIGKILL) == -1)
            perror("smash error: kill failed");
    }
}

JobsList::JobEntry *JobsList::getJobById(int jobId)
{
    if (jobs_list.empty())
        return nullptr;
    else
    {
        auto it = jobs_list.find(jobId);
        return &(it->second);
    }
}

JobsList::~JobsList()
{
    for (const auto& job : jobs_list)
    {
        delete job.second.cmd_ptr;
    }
}


///jobs command

void JobsCommand::execute()
{
    jobs->removeFinishedJobs();
    jobs->printJobsList();
}


///quit command

void QuitCommand::execute()
{
    if (!args.empty() && args[0] == "kill")
    {
        cout << "smash: sending SIGKILL signal to " << jobs->jobs_list.size() << " jobs:" << endl;
        for (auto const& job : jobs->jobs_list)
        {
            cout << job.second.pid << ": " << job.second.cmd_ptr->cmd_line << endl;
        }
        jobs->killAllJobs();
    }
    throw Quit();
}

QuitCommand::QuitCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs)
{
    parseArgs(string(cmd_line), 2);
}



///kill command

KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs)
{
    pair<int, int> args_parse = parseArgs(string(cmd_line), 3);
    if (args_parse.second != 2)
        throw KillInvArgs();

}

void KillCommand::execute()
{
    try {
        std::string signum_str = args[0].substr(1);
        int signum = std::stoi(signum_str);
        int job_id = std::stoi(args[1]);
        if (!jobs->jobs_list.count(job_id)) //jobid doesnt exist in jobs list
        {
            throw KillJobDoesntExist(args[1]);
        }
        auto job = jobs->getJobById(job_id);
        pid_t pid = job->pid;
        if (kill(pid, signum) == -1)
        {
            perror("smash error: kill failed");
            return;
        }
//        int status;
//        waitpid(pid, &status, WNOHANG);
        if (signum == SIGSTOP)
            job->is_stopped = true;
        else if (signum == SIGCONT)
            job->is_stopped = false;
        std::cout << "signal number " << signum << " was sent to pid " << pid << std::endl;
    }
    catch (const std::invalid_argument& e) //in case stoi fails because signum is not a valid number as it should be
    {
        throw KillInvArgs();
    }
    catch (const std::out_of_range& e)
    {
        throw KillInvArgs();
    }
}


///fg command

ForegroundCommand::ForegroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs)
{
    pair<int, int> args_parse = parseArgs(string(cmd_line), 1);
    if (args_parse.second > 1)
        throw FGInvArgs();
}

void ForegroundCommand::execute()
{
    try
    {
        JobsList::JobEntry* job;
        int job_id;
        if (args.empty()) //no job was specified, need to fg the job with maximal job id
        {
            if (jobs->jobs_list.empty())
                throw FGJobsListEmpty();
            job = jobs->getLastJob(&job_id);
        }
        else
        {
            job_id = std::stoi(args[0]);
            if (!jobs->jobs_list.count(job_id)) //jobid doesnt exist in jobs list
            {
                throw FGJobDoesntExist(args[0]);
            }
            job = jobs->getJobById(job_id);
        }

        pid_t pid = job->pid;
        std::cout << job->cmd_ptr->cmd_line << " : " << pid << std::endl;
        if (job->is_stopped)
            if (kill(pid, SIGCONT) == -1)
                perror("smash error: kill failed");
        job->cmd_ptr->in_foreground = true;
        SmallShell& smash = SmallShell::getInstance();
        smash.foreground_command = job->cmd_ptr;
        smash.jobs_list.removeJobById(job->job_id);
        int status;
        waitpid(pid, &status, WUNTRACED);
//        if((!(WIFSTOPPED(status))) || (WIFSTOPPED(status) && WSTOPSIG(status) == SIGKILL))
//        {
//            jobs->jobs_list.erase(job_id);
//        }
//        if (WIFSTOPPED(status))
//        {
//            smash.jobs_list.addJob(smash.foreground_command, smash.foreground_command->pid, true);
//        }
		if (!WIFSTOPPED(status))
			smash.timed_commands.commands.erase(pid);
        smash.foreground_command = NULL;
    }
    catch (const std::invalid_argument& e) //in case stoi fails
    {
        throw FGInvArgs();
    }
    catch (const std::out_of_range& e)
    {
        throw FGInvArgs();
    }
}



///bg command

BackgroundCommand::BackgroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line), jobs(jobs)
{
    pair<int, int> args_parse = parseArgs(string(cmd_line), 1);
    if (args_parse.second > 1)
        throw BGInvArgs();
}

void BackgroundCommand::execute()
{
    try
    {
        JobsList::JobEntry* job;
        if (args.empty()) //no job was specified, need to bg the stopped job with maximal job id
        {
            if (jobs->jobs_list.empty())
                throw BGNoStoppedJobs( );
            int job_id = -1;
            job = jobs->getLastStoppedJob(&job_id);
            if (job_id == -1)
                throw BGNoStoppedJobs();
        }
        else
        {
            int job_id = std::stoi(args[0]);
            job = jobs->getJobById(job_id);
            if (!jobs->jobs_list.count(job_id)) //jobid doesnt exist in jobs list
            {
                throw BGJobDoesntExist( args[0]);
            }
            if (!job->is_stopped)
                throw BGJobAlreadyRunning(args[0]);
        }
        job->cmd_ptr->in_foreground = false;
        std::cout << job->cmd_ptr->cmd_line << " : " << job->pid << std::endl;
        if (kill(job->pid, SIGCONT) == -1)
            perror("smash error: kill failed");
        job->is_stopped = false;
    }
    catch (const std::invalid_argument& e) //in case stoi fails
    {
        throw BGInvArgs();
    }
    catch (const std::out_of_range& e)
    {
        throw BGInvArgs();
    }
}


///pipe command

PipeCommand::PipeCommand(const char* cmd_line) : Command(cmd_line)
{
    commands = parseRedirectionPipe(cmd_line,&std_err);
}
void PipeCommand::execute()
{
    SmallShell& smash_inst = SmallShell::getInstance();
    int fds[2];
    if(pipe(fds)<-1)
        perror("smash error: pipe failed");
    int write_pid = fork();

    if(write_pid<0)
        perror("smash error: fork failed");


    if(!write_pid)
    {//child - handles writing
        setpgrp();
        smash_inst.is_child = true;



        
        if(dup2(fds[1],std_err ? STDERR : STDOUT) == -1)
        {
            perror("smash error: dup2 failed");
            exit(0);

        }

        if(close(fds[0]))
        {
            perror("smash error: close failed");
            exit(0);

        }
        close(fds[1]);
       
        smash_inst.executeCommand(commands.first.c_str());


        exit(0);



    }
    int read_pid = fork();


    if(read_pid<0)
        perror("smash error: fork failed");

    if(!read_pid)
    {//second child code - handles read
        setpgrp();
        smash_inst.is_child = true;
        if(dup2(fds[0], STDIN) == -1)
        {
            perror("smash error: dup2 failed");
            exit(0);
        }
        if(close(fds[1]))
        {
            perror("smash error: close failed");
            exit(0);
        }
        close(fds[0]);
    
        smash_inst.executeCommand(commands.second.c_str());




        exit(0);

    }

    // parent code
    close(fds[0]);
    close(fds[1]);
    waitpid(write_pid, nullptr, WUNTRACED);
    waitpid(read_pid, nullptr, WUNTRACED);
    // if (waitpid(write_pid, nullptr, WUNTRACED) == -1)
    //     perror("smash error: waitpid failed");
    // if (waitpid(read_pid, nullptr, WUNTRACED) == -1)
    //     perror("smash error: waitpid failed");
    //close(smash_inst.out_fd);
    //smash_inst.out_fd = STDOUT;


}




//redirection command

RedirectionCommand::RedirectionCommand(const char* cmd_line, SmallShell* smash_inst) : Command(cmd_line), smash_inst(smash_inst)
{
    bool append = false;
    pair<string,string> split = parseRedirectionPipe(cmd_line, &append);
    dest_fd = append ? open(split.second.c_str(), O_CREAT|O_WRONLY|O_APPEND, S_IRWXU) : open(split.second.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    if(dest_fd == -1)
        perror("smash error: open failed");
    dest_cmd = split.first.c_str();
}

int RedirectionCommand::prepare()
{
    if(dest_fd == -1)
        return -1;
    int prev_out_backup_fd = dup(1);
    if(prev_out_backup_fd == -1)
        perror("smash error: dup failed");
    int dup2_res = dup2(dest_fd,1);
    if(dup2_res == -1)
        perror("smash error: dup2 failed");
    return prev_out_backup_fd;

}

void RedirectionCommand::cleanup(int prev_fd)
{
    assert(prev_fd != -1);

    if(close(1) == -1)
        perror("smash error: close failed");
    if(dup2(prev_fd,1) == -1)
        perror("smash error: dup2 failed");
    if(close(prev_fd) == -1)
        perror("smash error: close failed");
}

void RedirectionCommand::execute()
{
    int prev_out_backup_fd;
//    prev_out_backup_fd = prepare();
//    if(prev_out_backup_fd == -1)
//        return;
//    smash_inst->SmallShell::executeCommand(string(dest_cmd).c_str());
//    cleanup(prev_out_backup_fd);
    try
    {
        prev_out_backup_fd = prepare();
        if(prev_out_backup_fd == -1)
            return;
        smash_inst->SmallShell::executeCommand(string(dest_cmd).c_str());
        cleanup(prev_out_backup_fd);
    }
    catch (const SmashException& e)
    {
        cleanup(prev_out_backup_fd);
        cerr << e.what() << endl;
    }
}

//cat command

CatCommand::CatCommand(const char* cmd_line) : BuiltInCommand(cmd_line)
{
    parseArgs(cmd_line,UNLIMITED_ARGS);
    if(args.empty())
        throw CatArgs();
}

void readFileIntoStdout(string filename)
{
    char* buff = new char(1);
    int fd_read = open(filename.c_str(),O_RDONLY);
    if(fd_read == -1)
    {
        perror("smash error: open failed");
        return;

    }

    int read_comeback = read(fd_read,buff,1);
    while(read_comeback)
    {
        if(read_comeback == -1)
        {
            perror("smash error: read failed");
            return;
        }
        if(write(STDOUT,buff,1) == -1)
        {
            perror("smash error: write failed");
            return;
        }

        read_comeback = read(fd_read,buff,1);
    }
    if(close(fd_read) == -1)
        perror("smash error: close failed");
    free(buff);
}

void CatCommand::execute()
{
    for(int i=0;i<(signed)args.size();i++)
    {
        readFileIntoStdout(args[i]);
    }
}



///*******************************************************   TIMED COMMANDS  *******************************************************///


void TimedCommands::addTimedCommand(int duration, pid_t pid, char* cmd_line)
{
    time_t timestamp;
    time(&timestamp);
    commands.insert(pair<int, TimedCommandNode> (pid, TimedCommandNode(duration, timestamp, pid, cmd_line)));
}

int TimedCommands::nextAlarm()
{
    if (commands.empty())
        return -1;
    time_t curr_time;
    time(&curr_time);
    int next_alarm = commands.begin()->second.duration - difftime(curr_time, commands.begin()->second.timestamp);
    for (auto cmd : commands)
    {
        int curr = cmd.second.duration - difftime(curr_time, cmd.second.timestamp);
        if (curr > 0 && curr < next_alarm)
            next_alarm = cmd.second.duration - difftime(curr_time, cmd.second.timestamp);
    }
    return next_alarm;
}

TimedCommands::TimedCommandNode* TimedCommands::timedOutCommand()
{
    time_t curr_time;
    time(&curr_time);
    if (commands.empty())
        return nullptr;
    for (auto it = commands.begin(); it != commands.end(); it++)
    {
        if (it->second.duration - difftime(curr_time, it->second.timestamp) == 0) //timed out command
        {
            return &(it->second);
        }
    }
    return nullptr;
}

void TimedCommands::clear()
{
    std::list<pid_t> pids_to_remove;
    int res;
    for (auto it = commands.begin(); it != commands.end(); it++)
    {
        res = waitpid(it->second.pid, nullptr, WNOHANG);
        if (res == -1) //failed
        {
            perror("smash error: waitpid failed");
        }
        else if (res == it->second.pid)
        {
            pids_to_remove.push_back(it->first);
        }
    }
    for (auto pid : pids_to_remove)
        commands.erase(pid);
}



///*******************************************************   EXCEPTIONS  *******************************************************///


const char* BuiltInException::what() const noexcept
{
    return error_string.c_str();
}

