/* 
 * Header defining the structure for ISH commands and redirections used
 * by the CS587 ISH project parser
 * Author: Patrick Bridges <patrickb@unm.edu>
 */

#pragma once

#include <string> 
#include <iostream>
#include <boost/spirit/home/x3.hpp>


namespace ish {

enum redirection_type {REDIRECT_IN, REDIRECT_OUT, REDIRECT_APPEND, REDIRECT_OUTERR, REDIRECT_APPENDERR};
struct redirection {
    enum redirection_type type;
    std::string path;
    redirection(enum redirection_type t, std::string p) 
        :type(t), path(p){}
};

class command {
    std::string name;
    std::vector<std::string> arguments;
    bool isForeground;
    std::vector<struct redirection> redirections;
  
public:
    command( std::string n ) 
        :name(n), isForeground(true) {
        arguments = std::vector<std::string>();
    }

    command( ) 
        :isForeground(true) {
    }

    void registerArgument(std::string arg) {
        arguments.push_back(arg);
    }
    void registerRedirection(enum redirection_type type, std::string path)
    {
        struct redirection r(type, path);
        redirections.push_back(r);
    }
    void setBackground() {
        isForeground = false;
    }
    void setForeground() {
        isForeground = true;
    }
    bool getForeground() const { return isForeground; }
    std::string getName() const { return name; }
    std::vector<struct redirection> getRedirections() const { return redirections; }
    std::vector<std::string> getArguments() const { return arguments; }
};

std::ostream& operator<<(std::ostream& out, const ish::command& cmd);

} // namespace ish