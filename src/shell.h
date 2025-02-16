#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include "command.hpp"
#include "parser.hpp"

#ifdef __unix__
#include <unistd.h>
#include <sys/types.h>
#endif

namespace ish {

class Shell {
public:
    Shell();
    void run();

private:
    bool executeCommand(const command& cmd);
    bool isBuiltin(const std::string& cmd) const;
    bool executeBuiltin(const command& cmd);
    void printPrompt() const;
    void readIshrc();
    bool handleRedirection(const redirection& redir);
    
    bool running;
    std::string currentDir;
    std::string hostname;
};

}  // namespace ish

#endif // SHELL_H 