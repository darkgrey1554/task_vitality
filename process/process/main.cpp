#include <iostream>
#include <sys/file.h>
#include "logger.h"
#include <chrono>
#include "daemon_init.h"
#include <unistd.h>
#include <sys/types.h>

constexpr auto block_file_name = "/home/user/block_file.txt";

int main()
{
    /// --- initialization  of daemon --- ///
    InitDaemon();

    /// --- work directory -- ///
    chdir("/home/user/log");

    /// ---  initialization  of logger --- ///
    LoggerSpace::Logger* log;
    log = LoggerSpace::Logger::getpointcontact();
    log->SetNameLog("LogProccess");
    log->TurnOnLog();

    int block_file = 0;
    int result = 0;
    int sys_error = 0;   

    /// --- try open block_file --- ///
    block_file = open(block_file_name, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    if (block_file == -1)
    {
        sys_error = errno;
        log->WriteLogWARNING("ERROR OPEN FILE", 0, sys_error
        );
        return 0;
    }

    /// --- try blocking file --- ///
    result = flock(block_file, LOCK_EX | LOCK_NB);
    if (result != 0)
    {
        close(block_file);
        sys_error = errno;
        if (sys_error != EWOULDBLOCK)
        {
            log->WriteLogWARNING("ERROR BLOKING FILE", 0, sys_error);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            close(block_file);
            return 0;
        }
        else
        {
            log->WriteLogINFO("THE FILE IS ALREADY BLOCK", 0, sys_error);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            close(block_file);
            return 0;
        }        
    }    
    log->WriteLogINFO("BLOCK FILE SUCCSESFUL");

    /// --- work process --- ///

    /// --- Clear File --- /// 
    {
        int file;
        file = open(block_file_name, O_CREAT | O_TRUNC |O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        close(file);
    }

    pid_t pid = getpid();
    std::string str;
    str += std::to_string(pid);

    if (write(block_file, str.c_str(), str.size()) != str.size())
    {
        log->WriteLogWARNING("ERROR WRITE PID IN FILE", 0, errno);
    }

    for (;;)
    {

        str.clear();
        str+="WORK PROCESS PID: " + std::to_string(pid);
        log->WriteLogINFO(str.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        
        if (get_sign() != 0)
        {
            log->WriteLogINFO("TAKE COMMAND FROM SYSTEM", get_sign(), 0);
            break;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
 }
