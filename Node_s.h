#ifndef NODE_S_H
#define NODE_S_H
#include <string>
#include "Exceptions.h"

class Node_s
{
    const std::string name;

    public:
    Node_s(std::string name);
    Node_s(const Node_s& other) = default;
    ~Node_s() = default;
    Node_s& operator=(const Node_s& other) = default;
    bool operator<(const Node_s Node_s) const; //for set comparison
    const std::string getName () const;// get name in read only

};

#endif