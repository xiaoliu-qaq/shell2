#include "shell.h"
#include "command.hpp"
#include "parser.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

// 如果系统没有定义X_OK
#ifndef X_OK
#define X_OK 1
#endif

namespace ish {

Shell::Shell() : running(true) {
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
    
    initializeEnvironment();
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
}

bool Shell::executeCommand(const command& cmd) {
    if (cmd.getName().empty()) {
        return true;
    }
    
    if (isBuiltin(cmd.getName())) {
        return executeBuiltin(cmd);
    }
    
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
        
        std::string fullPath;
        if (!findExecutable(cmd.getName(), fullPath)) {
            std::cerr << cmd.getName() << ": Command not found.\n";
            _exit(127);
        }
        
        std::vector<char*> args;
        args.push_back(const_cast<char*>(fullPath.c_str()));
        for (const auto& arg : cmd.getArguments()) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr);
        
        auto envp = prepareEnvironment();
        execve(fullPath.c_str(), args.data(), envp.data());
        
        std::cerr << "Command not found: " << fullPath << std::endl;
        _exit(1);
    }
    
    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

bool Shell::isBuiltin(const std::string& cmd) const {
    return cmd == "cd" || cmd == "quit" || 
           cmd == "setenv" || cmd == "unsetenv" ||
           cmd == "printenv";
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
    
    if (cmd.getName() == "setenv") {
        return handleSetenv(cmd);
    }
    if (cmd.getName() == "unsetenv") {
        return handleUnsetenv(cmd);
    }
    if (cmd.getName() == "printenv") {
        return handlePrintenv();
    }
    
    return false;
}

bool Shell::handleSetenv(const command& cmd) {
    const auto& args = cmd.getArguments();
    if (args.empty()) {
        return handlePrintenv();
    }
    if (args.size() == 1) {
        environment[args[0]] = "";
    } else {
        environment[args[0]] = args[1];
    }
    return true;
}

bool Shell::handleUnsetenv(const command& cmd) {
    const auto& args = cmd.getArguments();
    if (args.empty()) {
        std::cerr << "unsetenv: Too few arguments.\n";
        return false;
    }
    environment.erase(args[0]);
    return true;
}

bool Shell::handlePrintenv() {
    for (const auto& [name, value] : environment) {
        std::cout << name << "=" << value << "\n";
    }
    return true;
}

bool Shell::findExecutable(const std::string& cmd, std::string& fullPath) {
    if (cmd.find('/') != std::string::npos) {
        fullPath = cmd;
        return access(fullPath.c_str(), X_OK) == 0;
    }
    
    auto pathIt = environment.find("PATH");
    if (pathIt == environment.end()) return false;
    
    std::string pathVar = pathIt->second;
    size_t pos = 0;
    while ((pos = pathVar.find(':')) != std::string::npos) {
        std::string dir = pathVar.substr(0, pos);
        std::string testPath = dir + "/" + cmd;
        if (access(testPath.c_str(), X_OK) == 0) {
            fullPath = testPath;
            return true;
        }
        pathVar.erase(0, pos + 1);
    }
    if (!pathVar.empty()) {
        std::string testPath = pathVar + "/" + cmd;
        if (access(testPath.c_str(), X_OK) == 0) {
            fullPath = testPath;
            return true;
        }
    }
    return false;
}

std::vector<char*> Shell::prepareEnvironment() {
    static std::vector<std::string> envStrings;
    envStrings.clear();
    std::vector<char*> envp;
    
    for (const auto& [name, value] : environment) {
        std::string envStr = name + "=" + value;
        envStrings.push_back(envStr);
        envp.push_back(const_cast<char*>(envStrings.back().c_str()));
    }
    envp.push_back(nullptr);
    return envp;
}

void Shell::readIshrc() {
    const char* home = getenv("HOME");
    if (!home) return;
    
    std::string ishrc_path = std::string(home) + "/.ishrc";
    std::ifstream ishrc(ishrc_path);
    if (!ishrc) return;
    
    std::string line;
    while (std::getline(ishrc, line)) {
        if (line.empty() || line[0] == '#') continue;  // 跳过空行和注释
        
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

void Shell::initializeEnvironment() {
    // 使用getenv代替直接访问environ
    std::string pathValue = getenv("PATH") ? getenv("PATH") : "/usr/local/bin:/usr/bin:/bin";
    environment["PATH"] = pathValue;
    
    const char* home = getenv("HOME");
    if (home) {
        environment["HOME"] = home;
    }
    
    // 复制其他重要的环境变量
    const char* vars[] = {
        "TERM", "DISPLAY", "LANG", "LC_ALL", "USER", "SHELL", 
        "LOGNAME", "PWD", "OLDPWD", nullptr
    };
    
    for (const char** var = vars; *var; ++var) {
        const char* value = getenv(*var);
        if (value) {
            environment[*var] = value;
        }
    }
    
    // 设置shell
    environment["SHELL"] = "ish";
}

}  // namespace ish 