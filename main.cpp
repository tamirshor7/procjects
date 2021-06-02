#include "Graph.h"
#include "Faculty.h"
#include "Utils.h"
#include "runView.h"
#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include <ctype.h>
#include <vector>
#include <algorithm>
#include <assert.h>


using std::cout; using std::cin; using std::string; using std::map; using std::ostream; using std::endl; using std::vector; using std::set;
using std::ifstream; using std::ofstream; using std::cerr; using std::istream; using std::pair;

std::vector<Faculty> database; //db global variable

void handleNoParenthesisExclamationMarks(string& str, map<string,Graph>& var_list, set<char> known_binary_operators);
bool RemoveFirstEvenExclamationPair(string& str);
void compressExclamations(string& str);
const string getNextDummyName (map<string,Graph>& var_list);
const string parseLeftWing (const string& left_wing_input);
bool parseInputDisambiguation(const string& input, map<string,Graph>& var_list, ostream& os);
const Graph parseRightWing (string& right_wing_input, map<string,Graph>& var_list);
void compressLoads(string& input, map<string,Graph>& var_list);
void cleanDummyVariables (map<string,Graph>& var_list);
void parseRightWingSingleParenthesis (string& input_segment, map<string,Graph>& var_list);
string spacelessVectorCompressor (vector<string> spaceless, map<string,Graph>& var_list);
void validateSpacelessVector (const string& input_segment, const vector<string>& spaceless, const set<char>& known_binary_operators, const map<string,Graph>& var_list);
void handleNoParenthesisLiterals(string& str, map<string,Graph>& var_list);
Graph handleSingleLiteral (string& str);
void eliminateSpacesAroundCommas (string& str);
set<Node_s> parse_string_to_node_set (string& str);
set<Edge> parse_string_to_edge_set (string& str);
int commaOccurences(const string& str);
void handlePrint (const string& input, map<string,Graph>& var_list, ostream& os);
const Graph handleLoad(const string& input);
void handleSave (const string& input, map<string,Graph>& var_list);
void handleDelete (const string& input, map<string,Graph>& var_list);
string validateCommand(const string input, const string& target_command);
void validateSingleWord (const string& input, const string& target_command);
void handleWho (const string& input, map<string,Graph>& var_list, ostream& os);
void handleReset (const string& input, map<string,Graph>& var_list);
void handleQuit (const string& input);
bool sequenceInit (string& input, map<string,Graph>& var_list, ostream& os, istream& is);
int runSched (int argc,char* argv[]);


int main(int argc, char* argv[])
{
    if(argc == 1)//UI mode
    {
        char mode;
        cout<<"Welcome to Courses Manager - Enter [s] to initiate scheduling mode,\n";
        cout<<"or [v] to access viewing mode.\n";
        cin >> mode;
        while (mode != 's' && mode != 'v')
        {
            cout<< "Invalid choice indicator. Please enter 's' or 'v' to choose mode.\n";
            cin>>mode;
        }
        try
        {
            loadFaculties(database);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        if (mode == 's')
            runSched(argc,argv);
        else
        {
            runView();
        }
    }
    else
    {
        runSched(argc,argv);
    }

}

int runSched (int argc,char* argv[])
{
    map<string,Graph> var_list;
    string input;
    
    bool quit = false;

    if (argc == 1)
    {//shell mode - user interaction
        cout<<"Entering Scheduling Mode - Refer to README file to learn interface details.\n";
        cout<<"Enter 'quit' to end session. \n";
        while(!quit)
        {
            
            quit = sequenceInit(input, var_list, cout, cin);
            cout<<"SchedCalc> ";
        }
        char continue_s;
        cout<<"Session Ended - would you like to continue to a viewing session? [y], [n]\n";
        cin>>continue_s;
        if(continue_s == 'y')
            runView();
        
    }

    else if(argc == 3)
    {
        ifstream in_file(argv[1]);
        if(!in_file)
        {
            cerr<<"Fatal Error: Cannot open source file "<<argv[1]<<endl;
            return -1;
        }

        ofstream out_file(argv[2]);
        if(!out_file)
        {
            cerr<<"Fatal Error: Cannot open destination file "<<argv[2]<<endl;
            return -1;
        }

        
        while(!in_file.eof() && !quit)
        {
            quit = sequenceInit(input, var_list, out_file, in_file);
        }

    }

    else
    {
        cerr << "Fatal Error: unexpected argument number given";
        
    }
    return 0;
    
    


}

bool sequenceInit (string& input, map<string,Graph>& var_list, ostream& os, istream& is)
{
    try
        {
            getline(is,input);
            if(!is)
            {
                return true;
            }
            return parseInputDisambiguation(input, var_list, os);

        }
        catch (Exception& e)
        {
            os<<e.what()<<endl;
            return false;
        }
        catch (std::bad_alloc& e)
        {
            std::cerr<<e.what();
            return true;
        }

        catch (...)// should never get here
        {
            std::cerr << "Unknown error occurred"<<endl;
            return true;

        }

}
bool parseInputDisambiguation(const string& input, map<string,Graph>& var_list, ostream& os)
{

    int first_none_space = input.find_first_not_of(' ');
    if(first_none_space == (int)string::npos)
    {
        return false;
    }
    int first_equal_sign = input.find('=');
    if (first_equal_sign == (int)input.size()-1) //ensure line doesn't end with '='
    {
        throw UnrecognizedCommand(input);
    }
    if(first_equal_sign == (int)string::npos) //user wants to activate function over a variable (besides load)
    {

        string no_head_spaces = input.substr(first_none_space,input.size()-first_none_space);
        switch(input[first_none_space])
        {
            case 'p': handlePrint(no_head_spaces,var_list,os); break;
            case 'd': handleDelete(no_head_spaces,var_list);   break;
            case 'w': handleWho(no_head_spaces, var_list, os); break;
            case 'r': handleReset(no_head_spaces, var_list);   break;
            case 's': handleSave(no_head_spaces, var_list);    break;
            case 'q': handleQuit(no_head_spaces);              return true;
            default: throw UnrecognizedCommand(input);
        }
        

        

    }

    else // user wants to define  a new variable or assign new value to an existing variable
    {
        string left_wing = parseLeftWing(input.substr(0,first_equal_sign));
        try// to pass on the correct error string, without dummy variables
        {
            string right_wing_input = input.substr(first_equal_sign+1,input.size()-first_equal_sign-1);
            Graph right_wing = parseRightWing(right_wing_input, var_list);
            var_list[left_wing] = right_wing;
            
        }
        catch(const UnrecognizedCommand& e)
        {
            throw UnrecognizedCommand(input);
        }
        catch (const IllegalParenthesisSeries& e)
        {
            throw IllegalParenthesisSeries (input);
        }
        
    }

    return false;

}


void handlePrint (const string& input, map<string,Graph>& var_list, ostream& os)
{
    string eval_exp = validateCommand(input, string("print"));
    os << parseRightWing(eval_exp, var_list);
    
}

void handleDelete (const string& input, map<string,Graph>& var_list)
{
    string eval_exp = validateCommand(input, string("delete"));
    if(eval_exp.empty())
    {
        throw EmptyVariableName();
    }
    if(!var_list.erase(eval_exp))
    {
        throw UnknownVariable(eval_exp);
    }

}

void handleWho (const string& input, map<string,Graph>& var_list, ostream& os)
{
    validateSingleWord(input, string("who"));
    cleanDummyVariables(var_list);
    for(map<string,Graph>::iterator it = var_list.begin(); it!= var_list.end(); it++)
    {
        os<<it->first<<endl;
    }

}

void handleReset (const string& input, map<string,Graph>& var_list)
{
    validateSingleWord(input,string("reset"));
    var_list.clear();
}

void handleQuit (const string& input)
{
    validateSingleWord(input,string("quit"));
}

void handleSave (const string& input, map<string,Graph>& var_list)
{
    string eval_exp = validateCommand(input, string("save"));
    if (!commaOccurences(eval_exp))
    {
        throw EmptyArgument(string("save"));
    }
    int buffer = eval_exp.find_last_of(",");
    string graph_exp = eval_exp.substr(0,buffer);
    Graph save_data = parseRightWing(graph_exp, var_list);

    int filename_start = eval_exp.find_first_not_of(" ",buffer+1);
    if(filename_start == (int)string::npos)
    {
       throw EmptyArgument(string("save"));
    }

    string filename = eval_exp.substr(filename_start,eval_exp.size()-filename_start);
    save_data.encodeGraphToFile(filename);

}

const Graph handleLoad(const string& input)
{
    set<Node_s> nodes;
    set<Edge> edges;
    vector<Node_s> all_nodes;
    string filename = validateCommand(input, string("load"));
    ifstream source_file(filename, std::ios_base::binary);
    if(!source_file)
    {
        throw FileInaccessible(filename, "read");
    }
    int nodes_num; int edges_num;
    source_file.read((char*) &nodes_num,sizeof(int));
    source_file.read((char*) &edges_num,sizeof(int));
    int nodes_count =0;
    while (nodes_count != nodes_num+edges_num*2)
    {
        int node_size;
        source_file.read((char*) &node_size,sizeof(int));
        if(source_file.fail())
        {
            throw FileReadWriteError(filename, "read");
        }
        char* temp = new char[node_size+1];
        if(!temp)
        {
            throw std::bad_alloc();
        }
        source_file.read(temp, node_size);
        temp[node_size] = '\0';
        Node_s decoded = string(temp);
        all_nodes.push_back(decoded);
        delete [] temp;
        nodes_count++;
    }

    int i=0;
    
    while(i<nodes_num+edges_num*2)
    {
        if(i<nodes_num)
        {
            nodes.insert(all_nodes[i]);
            i++;
        }
        else
        {
            Edge decoded(all_nodes[i], all_nodes[i+1]);
            edges.insert(decoded);
            i += 2;
        }

    }

    source_file.close();
    return Graph(nodes,edges);


    



}

string validateCommand(const string input, const string& target_command)
{
    if((input.size() < target_command.size()) || (input.substr(0,target_command.size()) != target_command)) 
    {
        throw UnrecognizedCommand(input);
    }
    if(input.size() == target_command.size())
    {
        throw EmptyArgument(target_command);
    }
    string eval_exp = input.substr((int)target_command.size(),input.size()-target_command.size());
    int first_open_parenthesis = eval_exp.find_first_of('(');
    int last_close_parenthesis = eval_exp.find_last_of(')'); 
    if (first_open_parenthesis == (int)string::npos || first_open_parenthesis!= (int)eval_exp.find_first_not_of(' ') || last_close_parenthesis == (int)string::npos  || last_close_parenthesis != (int)eval_exp.find_last_not_of(' '))
    {
        throw MissingParenthesis();
    }
    int exp_first_none_space = eval_exp.find_first_not_of(" ",first_open_parenthesis+1);
    int exp_last_none_space = eval_exp.find_last_not_of(" ",last_close_parenthesis-1);
    return eval_exp.substr(exp_first_none_space, exp_last_none_space - exp_first_none_space +1);

}

void validateSingleWord (const string& input, const string& target_command)
{
    if((int)input.size() < (int)target_command.size() || input.substr(0,target_command.size()) != target_command || (int)input.find_first_not_of(" ",(int)target_command.size()) != (int)string::npos)
    {
        throw UnrecognizedCommand(input);
    }

}



const string parseLeftWing (const string& left_wing_input) //parse and return left wing var name. throw invalid var name exception if name is invalid
{
    int first_none_space = left_wing_input.find_first_not_of(' ');
    if(first_none_space == (int) string::npos || !isalpha(left_wing_input[first_none_space]))
    {
        throw InvalidVariableName(left_wing_input);
    }
    int last_none_space = left_wing_input.find_last_not_of(' ');
    string var_name_candidate = left_wing_input.substr(first_none_space,last_none_space-first_none_space+1);
    for (string::iterator it=var_name_candidate.begin(); it!=var_name_candidate.end(); ++it)
    {
        if(!isalnum(*it))
        {
            throw InvalidVariableName(left_wing_input);
        }

    }
    return var_name_candidate;
}

const Graph parseRightWing (string& right_wing_input, map<string,Graph>& var_list)
{
    compressLoads(right_wing_input,var_list);
    int last_open_parenthesis = right_wing_input.find_last_of('(');
    while (last_open_parenthesis != (int) string::npos)
    {
        int matching_close = right_wing_input.find_first_of(')',last_open_parenthesis);
        if (matching_close == (int) string::npos)
        {
            throw IllegalParenthesisSeries(right_wing_input);
        }
        string single_parenthesis_segment = right_wing_input.substr(last_open_parenthesis+1,matching_close-last_open_parenthesis-1);
        parseRightWingSingleParenthesis(single_parenthesis_segment, var_list);
        right_wing_input = right_wing_input.replace(last_open_parenthesis, matching_close-last_open_parenthesis+1, single_parenthesis_segment);
        last_open_parenthesis = right_wing_input.find_last_of('(');
    }
    parseRightWingSingleParenthesis(right_wing_input, var_list);
    Graph result = var_list[right_wing_input];
    cleanDummyVariables(var_list);
    return result;

}

void compressLoads(string& input, map<string,Graph>& var_list)
{
    int first_load = input.find("load");
    while (first_load != (int)string::npos)
    {
        int command_end = input.find(")",first_load+1);
        if (command_end == (int)string::npos)
        {
            throw MissingParenthesis();
        }
        string load_string = input.substr(first_load, command_end-first_load+1);
        Graph load_data = handleLoad(load_string);
        string new_var_name = getNextDummyName(var_list);
        var_list[new_var_name] = load_data;
        input.replace(first_load,command_end-first_load+1, new_var_name);
        first_load = input.find("load");

    }
}

void cleanDummyVariables (map<string,Graph>& var_list)
{
    for (map<string,Graph>::iterator it = var_list.begin();it!=var_list.end();)
    {
        if(it->first[0] == '~')
        {
           it=var_list.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void parseRightWingSingleParenthesis (string& input_segment, map<string,Graph>& var_list)
{
    const set<char> known_binary_operators = {'+','^','-','*'};
    handleNoParenthesisLiterals(input_segment, var_list);
    handleNoParenthesisExclamationMarks(input_segment, var_list, known_binary_operators);
    vector<string> spaceless;
    int first_none_space = input_segment.find_first_not_of(" ");
    while (first_none_space != (int) string::npos)
    {
        if(known_binary_operators.find(input_segment[first_none_space]) != known_binary_operators.end())
        {
            spaceless.push_back(string(1,input_segment[first_none_space]));
            first_none_space = input_segment.find_first_not_of(" ", first_none_space+1);
        }
        else
        {
            string var_name;
            while (first_none_space < (int)input_segment.size() && known_binary_operators.find(input_segment[first_none_space]) == known_binary_operators.end() && input_segment[first_none_space]!= ' ')
            {
                
                var_name += input_segment[first_none_space];
                first_none_space++;
                if(first_none_space == (int) input_segment.size())
                {
                    break;
                }
            }
            spaceless.push_back(var_name);
            if( first_none_space == (int)input_segment.size())
            {
                break;
            }
            if(input_segment[first_none_space] == ' ')
            {
                first_none_space = input_segment.find_first_not_of(" ", first_none_space+1);
            }
        }
    }


    validateSpacelessVector(input_segment, spaceless, known_binary_operators, var_list);
    input_segment = spacelessVectorCompressor(spaceless, var_list);
    
    
}

string spacelessVectorCompressor (vector<string> spaceless, map<string,Graph>& var_list)
{
    if(spaceless.size() == 1)
    {
        return spaceless[0];
    }

    else
    {
        Graph left = var_list[spaceless[0]];
        Graph right = var_list[spaceless[2]];
        Graph result = left;
        switch (spaceless[1][0])
        {
            case '+':  result = Graph::unite(left, right); break;
            case '^':  result = Graph::intersect(left, right); break;
            case '*':  result = left.Graph::multiply(right); break;
            case '-':  result = left.Graph::subtract(right); break;
            default :  assert(1<0); break; // should never get here
        }
        spaceless.erase (spaceless.begin(),spaceless.begin()+2);
        string new_var_name = getNextDummyName(var_list);
        var_list[new_var_name] = result;
        spaceless[0] = new_var_name;
        return spacelessVectorCompressor(spaceless, var_list);

    }
}


void validateSpacelessVector (const string& input_segment, const vector<string>& spaceless, const set<char>& known_binary_operators, const map<string,Graph>& var_list)
{
    bool expected_var = true;
    for (vector<string> :: const_iterator it = spaceless.begin(); it!=spaceless.end();it++)
    {
        if (expected_var)
        {
            if(var_list.find(*it) == var_list.end())
            {
                if((*it).find(")") != string::npos)
                {
                    throw IllegalParenthesisSeries();
                }
                throw UnknownVariable (*it);
            }
        }

        else
        {
            if( (*it).size() != 1 || known_binary_operators.find((*it)[0]) == known_binary_operators.end())
            {
                throw UnrecognizedCommand(input_segment); 
            } 
        }
        expected_var = 1- expected_var;
    }
    if(expected_var)//to make sure last item in line is a variable
    {
         throw UnrecognizedCommand(input_segment);
    }
    
}

const string getNextDummyName (map<string,Graph>& var_list)
{
    if(var_list.empty())
    {
        return string("~0");
    }
    string last_name = var_list.rbegin()->first; 
    if(last_name[0] != '~')
    {
        return string("~0");
    }
    last_name = last_name.substr(1,last_name.size()-1);
    return string("~") + std::to_string(std::stoi(last_name)+1);

}
void handleNoParenthesisExclamationMarks(string& str, map<string,Graph>& var_list, set<char> known_binary_operators)
{
    compressExclamations(str);

    int firstExclamation = str.find('!');
    while (firstExclamation!= (int) string::npos)
    {
        int var_name_start = str.find_first_not_of(" ", firstExclamation+1);
        string var_name = "";

        int i= var_name_start;
        for (;i<(int)str.size() && str[i] != ' ' && known_binary_operators.find(str[i]) == known_binary_operators.end() ;i++)
        {
            var_name += str[i];
        }
        if(var_list.find(var_name) == var_list.end())//unknown variable
        {
            throw UnrecognizedCommand(str);
        }
        Graph complemented = var_list[var_name].complement();
        string new_var_name = getNextDummyName(var_list);
        str.replace(firstExclamation,i-firstExclamation,new_var_name);
        var_list[new_var_name] = complemented;

        firstExclamation = str.find('!');
    }

}

void compressExclamations(string& str)
{
    int last_none_space = str.find_last_not_of(" ");
    if(last_none_space == (int)string::npos)
    {
        throw EmptyCommandEntered();
    }
    if(str[last_none_space] == '!')
    {
        throw UnrecognizedCommand(str);
    }
    while(RemoveFirstEvenExclamationPair(str)){}
}
bool RemoveFirstEvenExclamationPair(string& str)
{
    int candidate = (int)str.find_first_of('!');
    int next_none_space = str.find_first_not_of(" ", candidate+1);
    while(candidate!= (int)string::npos && str[next_none_space] != '!')
    {
        candidate = str.find_first_of('!',candidate+1);
        next_none_space = str.find_first_not_of(" ", candidate+1);
    }
    if(candidate == (int)string::npos)
    {
        return false;
    }
    str = str.erase(candidate, next_none_space-candidate+1);
    return true;

}

void handleNoParenthesisLiterals(string& str, map<string,Graph>& var_list)
{
    int first_open_bracket = str.find('{');
    while(first_open_bracket != (int) string::npos)
    {
        int first_close_bracket = str.find('}');
        if(first_close_bracket == (int) string::npos)
        {
            throw IllegalGraphLiteral(str.substr(first_open_bracket, str.size()-first_open_bracket));
        }

        string literal_string = str.substr(first_open_bracket, first_close_bracket-first_open_bracket+1);
        string new_var_name = getNextDummyName(var_list);
        Graph literal_graph = handleSingleLiteral(literal_string);
        str.replace(first_open_bracket,first_close_bracket-first_open_bracket+1, new_var_name);
        var_list[new_var_name] = literal_graph;
        
        first_open_bracket = str.find('{');


    }
}

Graph handleSingleLiteral (string& str)
{
    str = str.substr(1,str.size()-2);
    int literal_buffer = str.find('|');
    string nodes_str;
    string edges_str;
    if (literal_buffer == (int) string::npos)
    {
        nodes_str = str.substr(0,str.size());
    }
    else
    {
        nodes_str = str.substr(0,literal_buffer);
        if(literal_buffer == (int)str.size()-1)
        {
            edges_str = "";
        }
        else
        {
            edges_str = str.substr(literal_buffer+1,str.size()-literal_buffer-1);
        }
    }
    set<Node_s> nodes = parse_string_to_node_set(nodes_str);
    set<Edge> edges = parse_string_to_edge_set (edges_str);
    for (set<Edge>::iterator it = edges.begin(); it!= edges.end(); it++)
    {
        if(!(*it).inNodeSet(nodes))
        {
            string error_string =string ("<"+ (*it).getOrig().getName() + "," + (*it).getDest().getName() + ">");
            throw EdgesWithUnknownNodes (error_string);
        }
    }
    return Graph (nodes,edges);
    
}
void handleConsecutiveCommas (const string& str)
{
    for (int i=0; i<(int)str.size(); i++)
    {
        if(str[i] == ',' && str[str.find_first_not_of(" ", i+1)] == ',')
        {
            throw EmptyNodeName();
        }
    
    }
}
void eliminateSpacesAroundCommas (string& str)
{
    vector<string> vec;
    if(str.find_first_not_of(" ") == string::npos)
    {
        str.clear();
        return;
    }
    int current_comma = str.find(',');
    int prev_comma = 0;
    if(current_comma == (int)string::npos)
    {
        str = str.substr(str.find_first_not_of(" "),str.find_last_not_of(" ")- str.find_first_not_of(" ") + 1 );
        return;
    }
    handleConsecutiveCommas(str);
    string current_sub;
    while(prev_comma != (int)str.size()-1)
    {
        current_sub = str.substr(prev_comma,current_comma-prev_comma+1);
        if(current_comma == (int)string::npos)
        {
            current_comma = str.size()-1;
        }
        int var_name_start = (int)current_sub.find_first_not_of(" ,");
        int var_name_end = (int)current_sub.find_last_not_of(" ,");
        vec.push_back(current_sub.substr(var_name_start,var_name_end-var_name_start+1));
        prev_comma = current_comma;
        current_comma = str.find(',',current_comma+1);
    }
    str = "";
    for(vector<string>::iterator it = vec.begin(); it!= vec.end(); it++)
    {
        str = str +  *it + string(1,',');
    }
    str = str.substr(0,str.size()-1);
        
}

set<Node_s> parse_string_to_node_set (string& str)
{
    if(str.find_first_not_of(" ") != string::npos && (str[str.find_first_not_of(" ")] == ',' || str[str.find_last_not_of(" ")] == ','))
    {
        throw EmptyNodeName();
    }
    eliminateSpacesAroundCommas(str);
    set<Node_s> nodes;
    for(int i=0;i<(int)str.size();i++)
    {
        string course_id;
        while(str[i] != ',' && i<(int)str.size())
        {
            course_id += str[i];
            i++;
        }
        for(int i=0; i<course_id.size();i++)//validate id is purely numerical
        {
            if(!isdigit(course_id[i]))
                throw NonNumericalID(course_id);
        }
        const string* name = idToName(course_id);
        if(!name)
            throw IdNotInDB(course_id);
        bool already_exists = !nodes.insert(Node_s(course_id)).second; //All forms of illegal node names would be caught here by throws form Node_s constructor
        if(already_exists)
        {
            throw FoundNodeDuplication(course_id);
        }

    }
    return nodes;

}

set<Edge> parse_string_to_edge_set (string& str)
{
    set<Edge> edges;
    if((int)str.find_first_not_of(' ') == (int)(string::npos))
    {
        return edges;
    }
   if(str[str.find_first_not_of(" ")] == ',' || str[str.find_last_not_of(" ")] == ',')
    {
        throw IllegalEdgeIndicatorComposition(str);
    }
    eliminateSpacesAroundCommas(str);
    for(int i=0;i<(int)str.size();i++)
    {
        if(str[i] ==' ')
        {
            continue;
        }
        if(!edges.empty())
        {
            if(str[i] != ',')
            {
                 throw IllegalEdgeIndicatorComposition(str);
            }
            i++; // go over ',' seperator between edge indicators
           
        }
        string edge_indicator;
        while(i<int(str.size()))
        {
            edge_indicator += str[i];
            if(str[i] == '>')
            {
                break;
            }
            i++;
        }
        if(edge_indicator.size()<=2 || edge_indicator[0] != '<' || edge_indicator[edge_indicator.size()-1] != '>' || commaOccurences(edge_indicator)!=1)
        {
            throw IllegalEdgeIndicatorComposition(edge_indicator);
        }

        edge_indicator = edge_indicator.substr(1,edge_indicator.size()-2);

        vector<string> edge;
        for(int j=0;j<(int)edge_indicator.size();j++)
        {
            string node_name;
            while(edge_indicator[j] != ',' && j<(int)edge_indicator.size())
                {
                    if(edge_indicator[j] != ' ')
                    {
                        node_name += edge_indicator[j];
                    }
                    j++;

                }
                edge.push_back(node_name);
        }
        if(edge[0] == edge[1])
        {
            throw AttemptedToCreateSelfLoop(edge[0]);
        }

        Node_s node_orig = Node_s(edge[0]);//All forms of illegal node names would be caught here by throws form Node_s constructor
        Node_s node_dest = Node_s(edge[1]);
        bool already_exists = !edges.insert(Edge(node_orig,node_dest)).second; 
        if(already_exists)
        {
            throw FoundEdgeDuplication(edge_indicator);
        }

    }
    return edges;
}

int commaOccurences(const string& str)
{
    int count = 0;
    for (int i=0;i<(int)(str.size());i++)
    {
        if(str[i] ==  ',')
        {
            count++;
        }
    }
    return count;
}

const string* idToName(string id)
{//search dtatbase for course num. return name if exists. Null otherwise
    const string* name;
    for(int i=0;i<database.size();i++)
    {
        name = database[i].getNameById(id);
        if(name)
            return name;
    }
    return nullptr;
}