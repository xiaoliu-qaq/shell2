#include "parser.hpp"

#include <iostream>
#include <ostream>
#include <istream>

namespace ish {

void issuePrompt(std::ostream &out)
{
    out << "> " << std::flush;
}

void processCommands(std::istream &istr, std::ostream& ostr, bool interactive)
{
    while (!istr.eof() ) {
        std::string input;
        std::vector<ish::command> output;

        if (interactive) {
            issuePrompt(ostr);
        }

        std::getline(istr, input);

        auto f = input.begin(),
             l = input.end();
        if (ish::parser::parseCommands(f, l, output))  {
            for (auto &cmd: output) {
                ostr << cmd << "\n";
            }               
        } else {
            ostr << "Parse error: " << input << "\n";
        }
    }
}


} //namespace ish

int main(int argc, char *argv[])
{
    ish::processCommands(std::cin, std::cout, true);
    return 0;
}


