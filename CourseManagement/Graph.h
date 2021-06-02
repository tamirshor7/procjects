/*
Graph implementation for scheduling system. 
Supported Functionality:
unite - unite Node groups of two graphs, and all edges contained in union.
intersect - intersect Node groups of two graphs, and all edges contained in intersection.
subtract - Return a new graph, consisting of all nodes in *this, and not in the graph passed as second parameter.
           Edges of new graph are determined the same way.
multiply - Node group would be the cartesian multiplication of both input graphs. An edge [u;v] -> [x;y] would
           Exist in output graph in case an edge u->x and an edge v->y existed in input graphs.
complement - Return a new graph, with the same node group as input, and all possible edges which did not exist in input
             graph (and only these edges).
encodeGraphToFile - save graph in binary, format being : number of nodes as int, number of edges as int,
                    all nodes (as string), followed by all edges. Every node and every edge will start with an int indicating
                    its size, followed by all relevant bytes.
Rest of functions are for SWIG based python interface support - Not supported on Windows OS.
*/

#ifndef GRAPH_H
#define GRAPH_H
#include <set>
#include <iostream>
#include "Exceptions.h"
#include "Node_s.h"
#include "Edge.h"

class Graph
{
    std::set<Node_s> nodes;
    std::set<Edge> edges;

    public:
    Graph() = default;
    Graph(std::set<Node_s> nodes, std::set<Edge> edges) : nodes(nodes), edges(edges){}
    Graph(const Graph& other) = default;
    ~Graph() = default;
    Graph& operator=(const Graph& other) = default;
    static Graph unite(const Graph& graph1, const Graph& graph2);
    static Graph intersect(const Graph& graph1, const Graph& graph2);
    Graph subtract(const Graph& graph) const;
    Graph multiply(const Graph& graph) const;
    Graph complement() const;
    void encodeGraphToFile (const std::string& filename) const;
    friend std::ostream& operator<<(std::ostream& os,const Graph& graph);

    //Python interface infratructure
    friend Graph* addVertex (Graph* graph, const char* node_name);
    friend Graph* addEdge (Graph* graph, const char* orig_name, const char* dest_name);

};

std::ostream& operator<<(std::ostream& os,const Graph& graph);

//Python interface infratructure

Graph* create();
void destroy(Graph* target);
Graph* addVertex (Graph* graph, const char* node_name);
Graph* addEdge (Graph* graph, const char* orig_name, const char* dest_name);
void disp (Graph* graph);
Graph* graphUnion(Graph* in1, Graph* in2, Graph* out);
Graph* graphIntersection(Graph* in1, Graph* in2, Graph* out);
Graph* graphDifference(Graph* in1, Graph* in2, Graph* out);
Graph* graphProduct(Graph* in1, Graph* in2, Graph* out);
Graph* graphComplement(Graph* in, Graph* out);

#endif