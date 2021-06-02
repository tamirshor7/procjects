#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "Faculty.h"
#include <string>



typedef enum Status_t {
    SUCCEEDED = 0,
    FAILED = -1,
    ALLOC_ERROR = -2,
    INPUT_INVALID = -3
} Status;

void loadFaculties(std::vector<Faculty>& db);
const std::string* idToName(std::string id);

#endif