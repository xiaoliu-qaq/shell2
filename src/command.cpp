#include "command.hpp"

namespace ish {
    
std::ostream& operator<<(std::ostream& out, const ish::command& cmd)
{
   
    out << "[ Command: " << "\n";
    out << "  name: " << cmd.getName() << "\n"
        << "  args:\n";

    for (auto arg : cmd.getArguments())
    {
        out << "\t" << arg << "\n";
    }
    std::string path;
    bool err, append;
    out << "  redirections:\n";
    for (auto redir: cmd.getRedirections()) {
        switch(redir.type) {
        case REDIRECT_IN:
            out << "\tinput: ";
            break;
        case REDIRECT_OUT:
            out << "\toutput: ";
            break;
        case REDIRECT_APPEND:
            out << "\tappend output: ";
            break;
        case REDIRECT_OUTERR:
            out << "\toutput and error: ";
            break;
        case REDIRECT_APPENDERR:
            out << "\tappend output and error: ";
            break;
        }
        out << redir.path << "\n";
    }

    if (!cmd.getForeground()) out << "  Background Job\n";
    out << "]";
    return out;
}

} // namespace ish
