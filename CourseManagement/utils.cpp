#include "Utils.h"
#include <string>

using std::string; 

void loadFaculties(std::vector<Faculty>& db)
{
    Faculty CS = Faculty(std::string("Computer Science"));
    Faculty Math = Faculty(std::string("Math"));
    Faculty EE = Faculty(std::string("Electrical Engineering"));
    Faculty Phys = Faculty(std::string("Physics"));
    CS.parseFacultyDataFromFile(std::string("CS.txt"));
    Math.parseFacultyDataFromFile(std::string("Math.txt"));
    EE.parseFacultyDataFromFile(std::string("EE.txt"));
    Phys.parseFacultyDataFromFile(std::string("Phys.txt"));
    db.push_back(CS);
    db.push_back(Math);
    db.push_back(EE);
    db.push_back(Phys);
}

