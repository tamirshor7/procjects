#include "Graph.h"
#include <fstream>
using std::set; using std::string;

static Node_s generateCrossNamedNode (const Node_s& node1, const Node_s& node2);//encode two nodes to necessary [v;w] format
static set<Edge> crossEdgeSets (const set<Edge>& set1, const set<Edge>& set2);//cross two sets of edges to a new edge with nodes in required [v;w] format
static set<Node_s> crossNodeSets (const set<Node_s>& set1, const set<Node_s>& set2);//cross two sets of nodes to a new set with nodes in required [v;w] format

Graph Graph::unite(const Graph& graph1, const Graph& graph2)
{
    Graph united = graph1;
    for (set<Node_s>::iterator it=graph2.nodes.begin(); it!=graph2.nodes.end(); ++it)
    {
        united.nodes.insert(*it);
    }
    for (set<Edge>::iterator it=graph2.edges.begin(); it!=graph2.edges.end(); ++it)
    {
        united.edges.insert(*it);
    }

    return united;
}

Graph Graph::intersect(const Graph& graph1, const Graph& graph2)
{
    Graph intersection = graph1;
    for (set<Node_s>::iterator it=graph1.nodes.begin(); it!=graph1.nodes.end(); ++it)
    {
        if(graph2.nodes.find(*it) == graph2.nodes.end())
        {
            intersection.nodes.erase(*it);
        }
        
    }
    for (set<Edge>::iterator it=graph1.edges.begin(); it!=graph1.edges.end(); ++it)
    {
        if(graph2.edges.find(*it) == graph2.edges.end())
        {
            intersection.edges.erase(*it);
        }
        
    }

    return intersection;
}

Graph Graph::subtract(const Graph& graph) const
{
    set<Node_s> subtraction_nodes = nodes;
    set<Edge> subtraction_edges = edges;
    for (set<Node_s>::iterator it=nodes.begin(); it!=nodes.end(); ++it)
    {
        if(graph.nodes.find(*it)!=graph.nodes.end())
        {
            subtraction_nodes.erase(*it);
        }
    }

    for (set<Edge>::iterator it=edges.begin(); it!=edges.end(); ++it)
    {
        if(!(*it).inNodeSet(subtraction_nodes))
        {
            subtraction_edges.erase(*it);
        }
    }

    return Graph(subtraction_nodes,subtraction_edges);
}

Graph Graph::multiply(const Graph& graph) const
{
    return Graph(crossNodeSets(nodes,graph.nodes),crossEdgeSets(edges,graph.edges));
}

static set<Node_s> crossNodeSets (const set<Node_s>& set1, const set<Node_s>& set2)
{
    set<Node_s> cross;
    for (set<Node_s>::iterator it1=set1.begin(); it1!=set1.end(); ++it1)
    {
        for (set<Node_s>::iterator it2=set2.begin(); it2!=set2.end(); ++it2)
        {
            cross.insert(generateCrossNamedNode(*it1,*it2));
        }
    }
    
    return cross;
} 

static set<Edge> crossEdgeSets (const set<Edge>& set1, const set<Edge>& set2)
{
    set<Edge> cross;
    for (set<Edge>::iterator it1=set1.begin(); it1!=set1.end(); ++it1)
    {
        for (set<Edge>::iterator it2=set2.begin(); it2!=set2.end(); ++it2)
        {
            Edge crossEdge (generateCrossNamedNode((*it1).getOrig(),(*it2).getOrig()), generateCrossNamedNode((*it1).getDest(),(*it2).getDest()));
            cross.insert(crossEdge);
        }
    }

    return cross;

}

static Node_s generateCrossNamedNode (const Node_s& node1, const Node_s& node2)
{
    string crossed_name = string("[") + node1.getName() + ";" + node2.getName() + string("]");
    return Node_s(crossed_name);
}

Graph Graph::complement() const
{
    set<Edge> complementEdgeSet;
    for (set<Node_s>::iterator it1=nodes.begin(); it1!=nodes.end(); ++it1)
    {
        for (set<Node_s>::iterator it2=nodes.begin(); it2!=nodes.end(); ++it2)
        {
            if((*it1).getName() != (*it2).getName()) //avoid self loops
            {
                Edge candidate(*it1,*it2);
                if(edges.find(candidate) == edges.end())
                {
                    complementEdgeSet.insert(candidate);
                }
            }

        }
    }
    
    return Graph(nodes, complementEdgeSet);

}

std::ostream& operator<<(std::ostream& os,const Graph& graph)
{
    for (set<Node_s>::iterator it=graph.nodes.begin(); it!=graph.nodes.end(); ++it)
    {
        os<<(*it).getName()<<std::endl;
    }

    os<<'$'<<std::endl;

    for (set<Edge>::iterator it=graph.edges.begin(); it!=graph.edges.end(); ++it)
    {
        
        os<<(*it).getOrig().getName()<<" "<<(*it).getDest().getName()<<std::endl;
    }

    return os;

}

void Graph::encodeGraphToFile (const string& filename) const
{
    std::ofstream file_dest(filename, std::ios_base::binary);
    if(!file_dest)
    {
        throw FileInaccessible(filename, "write");
    }
    int node_num = nodes.size();
    int edge_num = edges.size();
    file_dest.write((const char*)&node_num, sizeof(int));
    file_dest.write((const char*)&edge_num, sizeof(int));
    for (set<Node_s>::const_iterator it =nodes.begin(); it!= nodes.end(); it++)
    {
        string name = (*it).getName();
        int size=(int)name.size();
        file_dest.write((char*)&size,sizeof(size));
        file_dest.write(name.c_str(),size);
    }

    for (set<Edge>::const_iterator it =edges.begin(); it!= edges.end(); it++)
    {
        string orig_name = (*it).getOrig().getName();
        int orig_size=(int)orig_name.size();
        file_dest.write((char*)&orig_size,sizeof(orig_size));
        file_dest.write(orig_name.c_str(),orig_size);

        string dest_name = (*it).getDest().getName();
        int dest_size=(int)dest_name.size();
        file_dest.write((char*)&dest_size,sizeof(dest_size));
        file_dest.write(dest_name.c_str(),dest_size);

    }
    file_dest.close();


}

bool validate (Graph* graph)
{
    if(!graph)
    {
        std::cout<<"Error: Null pointer given"<<std::endl;
        return false;
    }
    return true;
}

Graph* create()
{
    try
    {
        Graph* graph = new Graph();
        return graph;
    }
    catch(const std::bad_alloc& e)
    {
        std::cout << "Error: out of memory" << '\n';
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }
    return nullptr;
      
}

void destroy(Graph* target)
{
    if(target)
    {
        delete target;
    }
}

Graph* addVertex (Graph* graph, const char* node_name)
{
    validate(graph);
    try
    {
        bool already_exists = !(*graph).nodes.insert(Node_s(std::string(node_name))).second;
        if(already_exists)
        {
            std::cout<<"Error: attempted to add an already existent node"<<std::endl;
        }
    }
    catch(const Exception& e)
    {
        std::cout << e.what() << '\n';
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    return graph;
    
}

Graph* addEdge (Graph* graph, const char* orig_name,const char* dest_name)
{
    validate(graph);
    try
    {
       Edge add = Edge(Node_s(std::string(orig_name)), Node_s(std::string(dest_name)));
       if(!add.inNodeSet((*graph).nodes))
       {
           std::cout<<"Error: attempted to add edge with unknown vertices"<<std::endl;
           return graph;
       }
       bool already_exists = !((*graph).edges.insert(add)).second;
       if(already_exists)
       {
            std::cout<<"Error: attempted to add an already existent edge"<<std::endl;
       }

    }
    catch(const Exception& e)
    {
        std::cout << e.what() << '\n';
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    return graph;

}

void disp (Graph* graph)
{
    if(!validate(graph))
    {
        return;
    }
    std::cout<<(*graph);
}

Graph* graphUnion(Graph* in1, Graph* in2, Graph* out)
{
    if(validate(in1) && validate(in2) && validate(out))
    {
        *out = Graph::unite(*in1,*in2);
    }

    return out;  
}

Graph* graphIntersection(Graph* in1, Graph* in2, Graph* out)
{
    if(validate(in1) && validate(in2) && validate(out))
    {
        *out = Graph::intersect(*in1,*in2);
    }
    return out;    
}

Graph* graphDifference(Graph* in1, Graph* in2, Graph* out)
{
    if(validate(in1) && validate(in2) && validate(out))
    {
        *out = (*in1).Graph::subtract(*in2);
    }  
     return out;  
}

Graph* graphProduct(Graph* in1, Graph* in2, Graph* out)
{
    if(validate(in1) && validate(in2) && validate(out))
    {
        *out = (*in1).Graph::multiply(*in2);
    } 
    return out;   
}

Graph* graphComplement(Graph* in, Graph* out)
{
    if(validate(in) && validate(out))
    {
        *out = (*in).Graph::complement();
    }
    return out;    
}




