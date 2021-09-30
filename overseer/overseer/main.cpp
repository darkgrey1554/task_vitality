#include <iostream>
#include <sys/file.h>
#include <chrono>
#include <thread>
#include <unistd.h>
//#include "logger.h"
#include <signal.h>
#include <cstring>
#define block_file_name "/home/user/block_file.txt"
#define proccess "/home/user/projects/process/bin/x64/Release/process.out"

unsigned int GetStatus();
void KillProcess(int pid);
void CreateProcess();


enum class ListCommand
{
    STATUS,
    KILL,
    CREATE
};

int main(int arg, char** args)
{
    std::string str_help;

    int block_file = 0;
    int res = 0;
    char simvol = 0;
    int sys_error = 0;
    pid_t pid;
    ListCommand Command;

    std::cout << "HELLO I AM OVERSEER" << std::endl;

    if (arg != 2)
    {
        std::cout << "NOT COMMAND" << std::endl;
        return 0;
    }


    /// --- DIFINING COMMAND --- ///
    if (std::strcmp(args[1], "status") == 0)
    {
        Command = ListCommand::STATUS;
    }
    else if (std::strcmp(args[1], "kill") == 0)
    {
        Command = ListCommand::KILL;
    }
    else if (std::strcmp(args[1], "create") == 0)
    {
        Command = ListCommand::CREATE;
    }
    else
    {
        std::cout << "COMMAND NOT DEFINED" << std::endl;
        return 0;
    }

    if (Command == ListCommand::STATUS)
    {
        std::cout << "REQUEST STATUS ...." << std::endl;
        GetStatus();
    }
    else if (Command == ListCommand::KILL)
    {
        int pid;
        std::cout << "REQUEST STATUS ...." << std::endl;
        pid = GetStatus();
        if (pid == 0)
        {
            std::cout << "ERROR REQUEST KILL" << std::endl;
        }
        else
        {
            std::cout << "REQUEST KILL ...." << std::endl;
            KillProcess(pid);
        }

    }
    else if (Command == ListCommand::CREATE)
    {
        int pid;
        std::cout << "REQUEST STATUS ...." << std::endl;
        pid = GetStatus();
        if (pid != 0)
        {
            std::cout << "ERROR REQUEST CREATE" << std::endl;
        }
        else
        {
            std::cout << "REQUEST CREATE ...." << std::endl;
            CreateProcess();
        }
    }
    
    return 0;
};

unsigned int GetStatus()
{
    
    int block_file;
    int res;
    int sys_error;
    char simvol;
    std::string str_help;
    block_file = open(block_file_name, O_RDONLY);
    if (block_file == -1)
    {
        std::cout << "ERROR OPEN FILE : PROCCESS DEAD " << errno << std::endl;
        return 0;
    }

    res = flock(block_file, LOCK_EX | LOCK_NB);
    if (res != 0)
    {
        sys_error = errno;
        if (sys_error != EWOULDBLOCK)
        {
            std::cout << "ERROR BLOCKING FILE: " << errno << std::endl;
            close(block_file);
            return 0;
        }
        else
        {
            std::cout << "PROCESS IS LIVE" << std::endl;
        }
    }
    else
    {
        std::cout << "PROCCESS IS DEAD " << std::endl;
        flock(block_file, LOCK_UN);
        close(block_file);
        return 0;
    }

    while (read(block_file, &simvol, 1) > 0)
    {
        str_help += simvol;
    }

    std::cout << "PID PROCCESS: " << str_help << std::endl;
    close(block_file);
    return std::stoi(str_help.c_str());
}

void KillProcess(int pid)
{
    if (kill(pid, 9) != 0)
    {
        std::cout << "ERROR KILL PROCCESS: " << errno << std::endl;
        return;
    }
    std::cout << "PROCCESS DEAD" << std::endl;
    return;
}

void CreateProcess()
{
    int pid_f;

    pid_f = fork();
    if (pid_f == 0)
    {
        pid_f = execv(proccess, NULL);
        if (pid_f == -1)
        {
            std::cout << "ERROR CREATE PROCCESS: " << errno << std::endl;
        }
    }
    else if (pid_f == -1)
    {
        std::cout << "ERROR CREATE PROCCESS: " << std::endl;
    }

    std::cout << "PROCESS CREATING " << std::endl;
    return;
}