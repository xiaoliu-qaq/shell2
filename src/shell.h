#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include <map>

namespace ish {
class command;
struct redirection;

class Shell {
public:
    Shell();
    void run();
    
private:
    bool executeCommand(const command& cmd);
    bool isBuiltin(const std::string& cmd) const;
    bool executeBuiltin(const command& cmd);
    bool handleRedirection(const redirection& redir);
    bool findExecutable(const std::string& cmd, std::string& fullPath);
    std::vector<char*> prepareEnvironment();
    
    void initializeEnvironment();
    void readIshrc();
    void printPrompt() const;
    
    bool handleSetenv(const command& cmd);
    bool handleUnsetenv(const command& cmd);
    bool handlePrintenv();
    
    bool running;
    std::string currentDir;
    std::string hostname;
    
    std::map<std::string, std::string> environment;
};

}  // namespace ish

#endif // SHELL_H 