#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char* argv[])
{
    if (signal(SIGTSTP , ctrlZHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-Z handler");
    }

    if (signal(SIGINT , ctrlCHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-C handler");
    }

    struct sigaction alarm_handler;
    alarm_handler.sa_flags = SA_RESTART;
    alarm_handler.sa_handler = &alarmHandler;
    sigemptyset(&alarm_handler.sa_mask);

    if (sigaction(SIGALRM, &alarm_handler, NULL) == -1)
    {
        perror("smash error: failed to set alarm handler");
    }

    SmallShell& smash = SmallShell::getInstance();
    smash.shell_pid = getpid();
    if(smash.shell_pid == -1)
        perror("smash error: getpid failed");
    while(true)
    {
        //wait(nullptr);
        std::cout << smash.prompt;
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        try
        {
            smash.executeCommand(cmd_line.c_str());
        }
        catch (const Quit& e)
        {
            return 0;
        }
        catch (const SmashException&e)
        {
            std::cerr << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "something went wrong" << std::endl;
        }
        if(smash.is_child)
            exit(0);
    }
    return 0;
}