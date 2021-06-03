#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <string.h>
#include <map>
#include <list>
#include <iostream>

#define COMMAND_MAX_ARGS (20)
#define COMMAND_ARGS_MAX_LENGTH (200)

///*******************************************************   BASE CLASSES   *******************************************************///


class Command {
public:
    bool in_foreground = true;
    char cmd_line[COMMAND_ARGS_MAX_LENGTH];
    Command(const char* cmd_line);
    virtual ~Command() = default;
    virtual void execute() = 0;
    //virtual void prepare();
    //virtual void cleanup();
};


class BuiltInCommand : public Command {

protected:
    std::vector<std::string> args;
    std::pair<int,int> parseArgs(std::string cmd_line, int arg_num);  //parse arg_num input from cmd_line into args vec - on return first is success flag, second is num of args given in cmd_line
    //first word in input (command) is overlooked
public:
    BuiltInCommand(const char* cmd_line) : Command(cmd_line){}
    virtual ~BuiltInCommand() = default;
};


class ExternalCommand : public Command {
public:
    pid_t pid;
    int job_id;
    ExternalCommand(const char* cmd_line, pid_t pid) : Command(cmd_line), pid(pid), job_id(-1) {}
    virtual ~ExternalCommand() {}
    void execute() override;
};


///*******************************************************   BUILT IN COMMANDS   *******************************************************///


class ChangeDirCommand : public BuiltInCommand
{
public:
    std::string plastPwd;
    ChangeDirCommand(const char* cmd_line, const std::string& plastPwd);
    virtual ~ChangeDirCommand() {}
    void execute() override;
};


class ChangePromptCommand : public BuiltInCommand
{
public:
    ChangePromptCommand(const char* cmd_line);
    virtual ~ChangePromptCommand() {}
    void execute() override;
};


class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line){}
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};


class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
    virtual ~ShowPidCommand() {}
    void execute() override;
};


///jobs list

class JobsList {
public:
    class JobEntry {
    public:
        int job_id;
        pid_t pid;
        time_t init_time;
        bool is_stopped;
        ExternalCommand* cmd_ptr;
    public:
        JobEntry(int job_id, pid_t pid, time_t init_time, bool is_stopped, ExternalCommand* cmd_ptr);
        ~JobEntry() = default;
        JobEntry& operator=(const JobEntry& job_to_copy);
        JobEntry(const JobEntry& job_to_copy);
    };
    std::map<int, JobEntry> jobs_list;
public:
    JobsList() = default;
    ~JobsList();
    void addJob(ExternalCommand* cmd_ptr, pid_t pid, bool isStopped = false);
    void printJobsList();
    void killAllJobs();
    void removeFinishedJobs();
    JobEntry * getJobById(int jobId);
    void removeJobById(int jobId);
    JobEntry * getLastJob(int* lastJobId);
    JobEntry *getLastStoppedJob(int *jobId);
};


class JobsCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    JobsCommand(const char* cmd_line, JobsList* jobs) : BuiltInCommand(cmd_line), jobs(jobs) {}
    virtual ~JobsCommand() = default;
    void execute() override;
};


class ForegroundCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~ForegroundCommand() {}
    void execute() override;
};


class BackgroundCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    BackgroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~BackgroundCommand() {}
    void execute() override;
};


class KillCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    KillCommand(const char* cmd_line, JobsList* jobs);
    virtual ~KillCommand() {}
    void execute() override;
};


class QuitCommand : public BuiltInCommand {
    JobsList* jobs;
public:
    QuitCommand(const char* cmd_line, JobsList* jobs);
    virtual ~QuitCommand() {}
    void execute() override;
};


///*******************************************************   SPECIAL COMMANDS   *******************************************************///

class SmallShell;

enum fdtDefs {STDIN = 0,STDOUT = 1,STDERR = 2};


class PipeCommand : public Command {
    bool std_err = false;
    std::pair<std::string,std::string> commands;
public:
    PipeCommand(const char* cmd_line);
    virtual ~PipeCommand() {}
    void execute() override;
};


class RedirectionCommand : public Command {
    int dest_fd;
    std::string dest_cmd;
    SmallShell* smash_inst;
public:
    explicit RedirectionCommand(const char* cmd_line, SmallShell* smash_inst);
    virtual ~RedirectionCommand() {}
    void execute() override;
    int prepare();
    void cleanup(int prev);
};


class CatCommand : public BuiltInCommand {
public:
    CatCommand(const char* cmd_line);
    virtual ~CatCommand() {}
    void execute() override;
};


///*******************************************************   ALARM HANDLER  *******************************************************///

class TimedCommands {
public:
    class TimedCommandNode {
    public:
        int duration;
        time_t timestamp;
        pid_t pid;
        std::string cmd_line;
        TimedCommandNode(int duration, time_t timestamp, pid_t pid, char* cmd_line) :
                    duration(duration), timestamp(timestamp), pid(pid), cmd_line(cmd_line) {}
    };
    std::map<int, TimedCommandNode> commands;
    TimedCommands() {}
    void addTimedCommand(int duration, pid_t pid, char* cmd_line);
    TimedCommandNode* timedOutCommand();
    void clear();
    int nextAlarm();
};


///*******************************************************   SMASH CLASS  *******************************************************///

class SmallShell {
private:
    SmallShell() : foreground_command(nullptr) {}
public:
    std::string pwd;
    pid_t shell_pid;
    bool is_child = false;
    std::string prompt = "smash> ";
    JobsList jobs_list;
    TimedCommands timed_commands;
    ExternalCommand* foreground_command;
    Command* CreateCommand(const char* cmd_line);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell() {}
    void executeCommand(const char* cmd_line);
};



///*******************************************************   EXCEPTIONS  *******************************************************///


class Exception        : public std::exception{};
class SmashException   : public Exception{};

class BuiltInException : public SmashException
{
protected:
    const std::string error_string;
    BuiltInException(const std::string error_string) : error_string(error_string){}
public:
    const char* what() const noexcept override;
};



class CdException   : public BuiltInException
{
protected:
    CdException(const std::string& error_string) : BuiltInException(error_string){}
};

class CdArgsNum : public CdException
{
public:
    CdArgsNum() : CdException(std::string("smash error: cd: too many arguments")){}
};

class NoPwd : public CdException
{
public:
    NoPwd() : CdException(std::string("smash error: cd: OLDPWD not set")){}
};



class KillException : public BuiltInException
{
protected:
    KillException(const std::string& error_string) : BuiltInException(error_string) {}
};

class KillInvArgs : public KillException
{
public:
    KillInvArgs() : KillException("smash error: kill: invalid arguments") {}
};

class KillJobDoesntExist : public KillException
{
public:
    KillJobDoesntExist(std::string& job_id) : KillException("smash error: kill: job-id " + job_id + " does not exist") {}
};



class FGException   : public BuiltInException
{
protected:
    FGException(const std::string& error_string) : BuiltInException(error_string) {}
};

class FGInvArgs : public FGException
{
public:
    FGInvArgs() : FGException("smash error: fg: invalid arguments") {}
};

class FGJobsListEmpty : public FGException
{
public:
    FGJobsListEmpty() : FGException("smash error: fg: jobs list is empty") {}
};

class FGJobDoesntExist : public FGException
{
public:
    FGJobDoesntExist(const std::string& job_id) : FGException("smash error: fg: job-id " + job_id + " does not exist") {}
};



class BGException   : public BuiltInException
{
protected:
    BGException(const std::string& error_string) : BuiltInException(error_string) {}
};

class BGInvArgs : public BGException
{
public:
    BGInvArgs() : BGException("smash error: bg: invalid arguments") {}
};

class BGJobDoesntExist : public BGException
{
public:
    BGJobDoesntExist(const std::string& job_id) : BGException("smash error: bg: job-id " + job_id + " does not exist") {}
};

class BGNoStoppedJobs : public BGException
{
public:
    BGNoStoppedJobs() : BGException("smash error: bg: there is no stopped jobs to resume") {}
};

class BGJobAlreadyRunning : public BGException
{
public:
    BGJobAlreadyRunning(const std::string& job_id) : BGException("smash error: bg: job-id " + job_id + " is already running in the background") {}
};

class CatArgs : public BuiltInException
{
public:
    CatArgs() : BuiltInException("smash error: cat: not enough arguments"){}
};

class Quit : public BuiltInException
{
public:
    Quit() : BuiltInException("") {}
};


#endif //SMASH_COMMAND_H_