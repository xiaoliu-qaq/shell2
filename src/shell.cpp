#include "shell.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

#ifdef __unix__
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace ish {

Shell::Shell() : running(true) {
#ifdef __unix__
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        currentDir = cwd;
    }
    
    char hostname_buf[256];
    if (gethostname(hostname_buf, sizeof(hostname_buf)) == 0) {
        hostname = hostname_buf;
    } else {
        hostname = "unknown";
    }
#else
    currentDir = ".";
    hostname = "unknown";
#endif
    
    readIshrc();
}

void Shell::printPrompt() const {
    std::cout << hostname << "% ";
}

void Shell::run() {
    std::string input;
    
    while (running) {
        printPrompt();
        
        if (!std::getline(std::cin, input)) {
            std::cout << "\nBye!\n";
            break;
        }
        
        std::vector<command> commands;
        auto begin = input.begin();
        auto end = input.end();
        
        if (ish::parser::parseCommands(begin, end, commands)) {
            for (const auto& cmd : commands) {
                executeCommand(cmd);
            }
        } else {
            std::cerr << "Parse error\n";
        }
    }
}

bool Shell::handleRedirection(const redirection& redir) {
#ifdef __unix__
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // 644权限

    switch (redir.type) {
        case REDIRECT_IN:  // <
            fd = open(redir.path.c_str(), O_RDONLY);
            if (fd == -1) {
                std::cerr << "Error: Cannot open " << redir.path << " for input\n";
                return false;
            }
            dup2(fd, STDIN_FILENO);
            break;

        case REDIRECT_OUT:  // >
            fd = open(redir.path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
            if (fd == -1) {
                std::cerr << "Error: Cannot open " << redir.path << " for output\n";
                return false;
            }
            dup2(fd, STDOUT_FILENO);
            break;

        case REDIRECT_APPEND:  // >>
            fd = open(redir.path.c_str(), O_WRONLY | O_CREAT | O_APPEND, mode);
            if (fd == -1) {
                std::cerr << "Error: Cannot open " << redir.path << " for append\n";
                return false;
            }
            dup2(fd, STDOUT_FILENO);
            break;

        case REDIRECT_OUTERR:  // >&
            fd = open(redir.path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
            if (fd == -1) {
                std::cerr << "Error: Cannot open " << redir.path << " for output\n";
                return false;
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            break;

        case REDIRECT_APPENDERR:  // >>&
            fd = open(redir.path.c_str(), O_WRONLY | O_CREAT | O_APPEND, mode);
            if (fd == -1) {
                std::cerr << "Error: Cannot open " << redir.path << " for append\n";
                return false;
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            break;
    }
    
    if (fd != -1) {
        close(fd);
    }
    return true;
#else
    std::cerr << "Redirection not supported on this platform\n";
    return false;
#endif
}

bool Shell::executeCommand(const command& cmd) {
    if (cmd.getName().empty()) {
        return true;
    }
    
    if (isBuiltin(cmd.getName())) {
        return executeBuiltin(cmd);
    }
    
#ifdef __unix__
    pid_t pid = fork();
    
    if (pid == -1) {
        std::cerr << "Fork failed\n";
        return false;
    }
    
    if (pid == 0) {  // 子进程
        for (const auto& redir : cmd.getRedirections()) {
            if (!handleRedirection(redir)) {
                _exit(1);
            }
        }
        
        std::vector<char*> args;
        args.push_back(const_cast<char*>(cmd.getName().c_str()));
        
        for (const auto& arg : cmd.getArguments()) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr);
        
        execvp(cmd.getName().c_str(), args.data());
        
        std::cerr << "Command not found: " << cmd.getName() << std::endl;
        _exit(1);
    }
    
    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
#else
    std::cerr << "External commands not supported on this platform\n";
    return false;
#endif
}

bool Shell::isBuiltin(const std::string& cmd) const {
    return cmd == "cd" || cmd == "quit";
}

bool Shell::executeBuiltin(const command& cmd) {
    if (cmd.getName() == "quit") {
        running = false;
        return true;
    }
    
    if (cmd.getName() == "cd") {
        const auto& args = cmd.getArguments();
        const char* dir = args.empty() ? 
            getenv("HOME") : args[0].c_str();
            
        if (chdir(dir) == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                currentDir = cwd;
            }
            return true;
        }
        std::cerr << "cd: No such file or directory\n";
        return false;
    }
    
    return false;
}

void Shell::readIshrc() {
    const char* home = getenv("HOME");
    if (!home) return;
    
    std::string ishrc_path = std::string(home) + "/.ishrc";
    std::ifstream ishrc(ishrc_path);
    if (!ishrc) return;
    
    std::string line;
    while (std::getline(ishrc, line)) {
        std::vector<command> commands;
        auto begin = line.begin();
        auto end = line.end();
        
        if (ish::parser::parseCommands(begin, end, commands)) {
            for (const auto& cmd : commands) {
                executeCommand(cmd);
            }
        }
    }
}

}  // namespace ish 