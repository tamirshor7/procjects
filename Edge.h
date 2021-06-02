#ifndef EDGE_H
#define EDGE_H
#include <string>
#include "Exceptions.h"
#include "Node_s.h"
#include <set>

class Edge
{
    Node_s orig;
    Node_s dest;
    public:
    Edge(Node_s orig, Node_s dest) : orig(orig), dest(dest){}
    Edge(const Edge& other) = default;
    ~Edge() = default;
    Edge& operator= (const Edge& other) = default;
    const Node_s getOrig () const {return orig;} //get origin and destination nodes in read-only
    const Node_s getDest () const {return dest;}
    bool operator< (const Edge edge) const //for set comparison
    {
        if(orig.getName() == edge.orig.getName())
        {
            return dest<edge.dest;
        }
        return orig<edge.orig;
         
    } //for set comparison
    bool inNodeSet (const std::set<Node_s>& nodes) const {return nodes.find(orig)!=nodes.end() && nodes.find(dest)!=nodes.end();}//return true if both nodes of *this are in the set
};

#endif