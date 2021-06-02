#include "Faculty.h"
#include "Exceptions.h"
#include <iostream>
#include <fstream>
#include <sstream>

using std::string; using std::map; using std::ifstream; using std::stringstream;

void Faculty::parseFacultyDataFromFile(const string& filename)
{
    std::ifstream source("..//"+filename);
    if(source.fail())
        throw DBReadException(name);
    string line;
    while(getline(source,line))
    {
        int delim = line.find_first_of(',');
        if(delim == (int)string::npos || delim == line.size()-1)
            throw CorruptFile(filename);
        courses.insert({line.substr(0,delim),line.substr(delim+1)});
    }
    source.close();
}

const std::string* Faculty::getNameById(string id) const
{
    if(!courses.count(id))
        return nullptr;
    return &(courses.at(id));
}