/* 
 * Header-only Boost Spirit X3 parser for the CS587 ISH project
 * Author: Patrick Bridges <pat
 */

#pragma once

#include <string> 
#include <boost/spirit/home/x3.hpp>

#include "command.hpp"

namespace ish {

/*
 * A Parsed command line is a vector of ish::command structures. 
 * (1) The top level is the list of commands to execute - there may be 
 *     more than one entry in this list for ';' seperated lists of commands
 * (2) The actual IShH commands are a Command struct as defined below  
 *
 * Credit: The general struture of the Boost::Spirit::X3 recursive decent
 * parser was inspired by the ppsh parser (https://github.com/peter-facko/ppsh).
 * (1) Users the parser to capture whether the command passed is a builtin or   
 * (2) Only captures the name of the file to redirect output to or from, and
 * (3) We don't parse pipelines
 * (4) We only parse up to a newline, not to end of input. The higher-level code
 *     invoking the parser has to handle end of input
 * (5) We handle background commands
 * 
 * Another reference I used when building this parser are the slides of an X3
 * tutorial workshop found here: 
 * https://ciere.com/assets/uploads/cppnow15/using_x3.h-16280204.pdf
 */



namespace parser {

namespace x3 = boost::spirit::x3;


/* Development step:
 * Figure out the basic tokens in the input stream and what characters 
 * are in each of them
 */
auto const eoln = x3::eol;
auto const ishpunct = x3::char_(",./`!@#$%^*=+") | x3::char_('-') | x3::char_('_');

// Escape handling courtesy of 
// https://stackoverflow.com/questions/61695235/creating-a-boostspiritx3-parser-for-quoted-strings-with-escape-sequence-hand
auto const escapes = "\\n" >> x3::attr('\n')
    | "\\b" >> x3::attr('\b')
    | "\\f" >> x3::attr('\f')
    | "\\t" >> x3::attr('\t')
    | "\\v" >> x3::attr('\v')
    | "\\0" >> x3::attr('\0')
    | "\\r" >> x3::attr('\r')
    | "\\n" >> x3::attr('\n')
    | "\\"  >> x3::char_("\"\\");
auto const dquote = x3::rule<class dquote, std::string>() 
    = x3::lexeme['"' >> *( escapes | ~x3::char_('"')) >> '"'];
auto const squote = x3::rule<class squote, std::string>() 
    = x3::lexeme['\'' >> *( escapes |  ~x3::char_('\'')) >> '\''];
auto const backspecial = '\\' >> x3::char_;

// A token is a what the shell actually uses for arguments and filenames and such
// Need an action to make a std::string out its contents
auto const token = x3::rule<class token, std::string>()
    = x3::lexeme[+(x3::char_("a-zA-Z0-9") | backspecial | ishpunct | dquote | squote) ] [(
        [](auto& context)
        {
            std::string tk;

            // A little complicated because some of the alternatives have character
            // but some (backspecial, squote, dquote) have string attributes. 
            for (auto c : _attr(context))
            {
                if (c.type() == typeid(char)) {
                    tk.push_back(get<char>(c));
                }
                else if ( c.type() == typeid(std::string)) {
                    tk.append(get<std::string>(c));
                }
            }

            _val(context) = std::move(tk);
        })];


/* Create a class to capture state of a redirection while parsing. Handed from the 
 * redirection handling to the construction of the command that uses the redirection. */


struct redirection_type_symbols_ : boost::spirit::x3::symbols<enum redirection_type>
{
    redirection_type_symbols_()
    {
        add(">", REDIRECT_OUT);
        add(">>", REDIRECT_APPEND);
        add(">&", REDIRECT_OUTERR);
        add(">>&", REDIRECT_APPENDERR);
        add("<", REDIRECT_IN);
    }
} static const redirection_type_symbols;

/* The rule which turns a redirection into a redirectin structure*/
const x3::rule<class redirect_rule, std::pair<enum redirection_type, std::string>> redirect = "redirect";
auto const redirect_def = (redirection_type_symbols >> token)[(
    [](auto &context)
    {
            enum redirection_type redir = at_c<0>(_attr(context));
            auto & path = at_c<1>(_attr(context));
            auto r = make_pair(redir, std::move(path));
            _val(context) = std::move(r);
    })];
BOOST_SPIRIT_DEFINE(redirect);

/* Almost there: The code to turn a command (a path followed by a list of either 
 * redirections or arguments into command structure. This is complicated because any
 * piece of the rest of the command line can be either a simple string (an argument) 
 * or a redirection symbol followed by a simple string (the filename). As a result,
 * we have to walk this list of arguments and check each if it's a redirection or 
 * argument, and then add it to the command, as encountered. */
const x3::rule<class commandel_rule, class ish::command> commandel = "commandel";
auto const commandel_def = (token >> *(token|redirect))[(
    [](auto& context)
    {
        auto& command_path = at_c<0>(_attr(context));
        const auto& redir_or_args_list = at_c<1>(_attr(context));
        class ish::command cmd(std::move(command_path));

        for (const auto& redir_or_arg: redir_or_args_list) {
            if (redir_or_arg.type() == typeid(std::pair<enum redirection_type,std::string>)) {
                auto r = get<std::pair<enum redirection_type, std::string>>(redir_or_arg);
                cmd.registerRedirection(get<0>(r), get<1>(r));
            } else if (redir_or_arg.type() == typeid(std::string)) {
                auto args = get<std::string>(redir_or_arg);
                cmd.registerArgument(args);
            } else {
                std::cerr << "Invalid redirection typeid\n";
            }
        }
        _val(context) = std::move(cmd);
    })];
BOOST_SPIRIT_DEFINE(commandel);

auto const commandseq = commandel % x3::char_(';');

auto const grammar = x3::rule<class grammar, std::vector<class ish::command>>()
    = (-commandseq >> (eoln | x3::eoi))[(
        [](auto& context)
        {
            auto& cmd_opt = _attr(context);
            std::vector<ish::command> v;
            if (cmd_opt.has_value()) {
                v = cmd_opt.value();
            }
            _val(context) = std::move(v);
        })];


template <class Iterator>
bool parseCommands(Iterator &begin, Iterator &end, std::vector<class ish::command> &output) 
{
    return x3::phrase_parse(begin, end, grammar, x3::ascii::space - x3::char_('\n'), output);
};  

} // Namespace parser
} // Namespace isp
