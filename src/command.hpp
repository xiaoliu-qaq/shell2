/* 
 * Header defining the structure for ISH commands and redirections used
 * by the CS587 ISH project parser
 * Author: Patrick Bridges <patrickb@unm.edu>
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace ish {

enum redirection_type {REDIRECT_IN, REDIRECT_OUT, REDIRECT_APPEND, REDIRECT_OUTERR, REDIRECT_APPENDERR};
struct redirection {
    enum redirection_type type;
    std::string path;
    redirection(enum redirection_type t, std::string p) 
        :type(t), path(p){}
};

class command {
public:
    command() = default;
    command(const std::string& name) : name_(name) {}

    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }

    void addArgument(const std::string& arg) { args_.push_back(arg); }
    const std::vector<std::string>& getArguments() const { return args_; }

    void registerRedirection(enum redirection_type type, std::string path) {
        redirections.push_back(redirection(type, path));
    }
    
    void setBackground() { isForeground = false; }
    void setForeground() { isForeground = true; }
    bool getForeground() const { return isForeground; }
    const std::vector<redirection>& getRedirections() const { return redirections; }

private:
    std::string name_;
    std::vector<std::string> args_;
    bool isForeground = true;
    std::vector<redirection> redirections;
};

std::ostream& operator<<(std::ostream& out, const ish::command& cmd);

} // namespace ish