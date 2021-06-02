/* 
Class implementing faculties, in order to support maintaining a course database relevant for a certain faculty.
Default definitions are loaded from hard-coded files CS.txt, Phys.txt, EE.txt and Math.txt, via the 
parseFacultyDataFromFile function.
Every course has a name string, retrieved by getNameById.

*/

#ifndef FAC_H
#define FAC_H

#include <string>
#include <map>

class Faculty
{
    const std::string name;
    std::map<std::string,std::string> courses;

    public:
    Faculty(std::string name) : name(name){}
    void parseFacultyDataFromFile(const std::string& filename);
    const std::string* getNameById(std::string id) const;

};

#endif