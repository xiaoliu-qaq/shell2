#include "command.hpp"

namespace ish {

std::ostream& operator<<(std::ostream& out, const command& cmd) {
    out << cmd.getName();
    
    // 输出参数
    for (const auto& arg : cmd.getArguments()) {
        out << " " << arg;
    }
    
    // 输出重定向
    for (const auto& redir : cmd.getRedirections()) {
        switch (redir.type) {
            case REDIRECT_OUT:
                out << " > " << redir.path;
                break;
            case REDIRECT_APPEND:
                out << " >> " << redir.path;
                break;
            case REDIRECT_IN:
                out << " < " << redir.path;
                break;
            case REDIRECT_OUTERR:
                out << " >& " << redir.path;
                break;
            case REDIRECT_APPENDERR:
                out << " >>& " << redir.path;
                break;
        }
    }
    
    return out;
}

} // namespace ish
