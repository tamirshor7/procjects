#include "Node_s.h"
#include <cctype>
using std::string;

static const string validateName(const string& name)
{
    if(name =="")
    {
        throw EmptyNodeName();
    }
    int open_bracket_counter = 0;
    int close_bracket_counter = 0;
    for (string::const_iterator it=name.begin(); it!=name.end(); ++it)
    {
        if(open_bracket_counter < close_bracket_counter)
        {
            throw IllegalBracketSeries(name);
        }
        switch(*it)
        {
            case '[': open_bracket_counter++; break;
            case ']': close_bracket_counter++; break;
            case ';': if(!open_bracket_counter||open_bracket_counter == close_bracket_counter) {throw IllegalSemicolon(name);} break;
            default : if(!isalnum(*it)) {throw IllegalCharacter(name);} break;       
        }

    }
    if(open_bracket_counter != close_bracket_counter)
    {
        throw IllegalBracketSeries(name);
    }

    return name;

}

Node_s::Node_s(std::string name) : name(validateName(name)){}

bool Node_s::operator<(const Node_s Node_s) const
{
     return (*this).name<Node_s.name;
}

const std::string Node_s::getName () const
{
    return name;
}