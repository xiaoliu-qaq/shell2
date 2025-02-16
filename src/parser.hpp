/* 
 * Header-only Boost Spirit X3 parser for the CS587 ISH project
 * Author: Patrick Bridges <pat
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/variant/get.hpp>

namespace ish {
namespace parser {

namespace x3 = boost::spirit::x3;
namespace fusion = boost::fusion;

using boost::get;
using boost::fusion::at_c;

template <class Iterator>
bool parseCommands(Iterator &begin, Iterator &end, std::vector<class ish::command> &output) 
{
    return x3::phrase_parse(begin, end, grammar, x3::ascii::space - x3::char_('\n'), output);
};  

} // namespace parser
} // namespace ish
