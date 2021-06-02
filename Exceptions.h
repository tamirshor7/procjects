#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <iostream>
#include <string>

class Exception : public std::exception{};
class NodeException : public Exception{};
class EdgeException : public Exception{};
class GraphException : public Exception{};
class DBException : public Exception{};
    

//View Exceptions
class IllegalIndex : public Exception{};
class IllegalArraySize : public Exception{};
//Node Exceptions
class IllegalBracketSeries : public NodeException
{
    const std::string error_string;
    public:
    IllegalBracketSeries(const std::string& name) : error_string(std::string("Error: Illegal series of brackets in entered Node name ") + name){}
    const char* what() const noexcept override;
};

class IllegalSemicolon : public NodeException
{
    const std::string error_string;
    public:
    IllegalSemicolon(const std::string& name) : error_string(std::string("Error: Entered semicolon outside of set of brackets in enterd Node name ") + name){}
    const char* what() const noexcept override;
};

class IllegalCharacter : public NodeException
{
    const std::string error_string;
    public:
    IllegalCharacter(const std::string& name) : error_string(std::string("Error: Entered an unexpected character in Node named: ") + name){}
    const char* what() const noexcept override;
};

class EmptyNodeName : public NodeException
{
    const std::string error_string = "Error: Entered empty node name";
    public:
    const char* what() const noexcept override;

};
class NonNumericalID : public NodeException
{
    const std::string error_string;
    public:
    NonNumericalID(const std::string& id) : error_string(std::string("Error: Entered a non fully numerical course id: ") + id){}
    const char* what() const noexcept override;

};


//Edge Exceptions

class EmptyEdgeName : public EdgeException
{

    const std::string error_string = "Error: Entered empty edge indicator";
    public:
    const char* what() const noexcept override;

};

class IllegalEdgeIndicatorComposition : public EdgeException
{
    const std::string error_string;
    public:
    IllegalEdgeIndicatorComposition(const std::string& name) : error_string(std::string("Error: Entered Illegal edge inidicator: ") + name){}
    const char* what() const noexcept override;
};

class AttemptedToCreateSelfLoop : public EdgeException
{
    const std::string error_string;
    public:
    AttemptedToCreateSelfLoop(const std::string& name) : error_string(std::string("Error: Entered Illegal edge. Attempted to create self-loop with node name: ") + name){}
    const char* what() const noexcept override;

};

//Graph Exceptions
class UnknownVariable : public GraphException
{
    const std::string error_string;
    public:
    UnknownVariable(const std::string& var_name) : error_string(std::string("Error: Unknown variable '") + var_name + std::string("'")){}
    const char* what() const noexcept override;
};

class UnrecognizedCommand : public GraphException
{
    const std::string error_string;
    public:
    UnrecognizedCommand(const std::string& command) : error_string(std::string("Error: Unable to parse command: '") + command + std::string("'")){}
    const char* what() const noexcept override;
};

class InvalidVariableName : public GraphException
{
     const std::string error_string;
    public:
    InvalidVariableName(const std::string& var_name) : error_string(std::string("Error: Attempted to initialize variable with invalid name: '") + var_name + std::string("'")){}
    const char* what() const noexcept override;

};

class IllegalGraphLiteral : public GraphException
{
    const std::string error_string;
    public:
    IllegalGraphLiteral(const std::string& literal_def) : error_string(std::string("Error: Illegal attemp to initialize graph by literal definition: ") + literal_def){}
    const char* what() const noexcept override;
};

class FoundNodeDuplication : public GraphException
{
    const std::string error_string;
    public:
    FoundNodeDuplication(const std::string& literal_def) : error_string(std::string("Error: Found node duplication in graph literal: ") + literal_def){}
    const char* what() const noexcept override;
};

class FoundEdgeDuplication : public GraphException
{
    const std::string error_string;
    public:
    FoundEdgeDuplication(const std::string& literal_def) : error_string(std::string("Error: Found edge duplication in graph literal: ") + literal_def){}
    const char* what() const noexcept override;
};

class EdgesWithUnknownNodes : public GraphException
{
    const std::string error_string;
    public:
    EdgesWithUnknownNodes(const std::string& literal_def) : error_string(std::string("Error: Found edge with unknown nodes: ") + literal_def){}
    const char* what() const noexcept override;
};

class IllegalParenthesisSeries : public GraphException
{
    const std::string error_string;
    public:
    IllegalParenthesisSeries(const std::string& literal_def= "") : error_string(std::string("Error: input contains unclosed parenthesis set: ") + literal_def){}
    const char* what() const noexcept override;

};

class EmptyCommandEntered : public GraphException
{
    const std::string error_string = "Error: Entered empty command";
    public:
    const char* what() const noexcept override;

};

class MissingParenthesis : public GraphException
{
    const std::string error_string = "Error: Entered command without parenthesis. Please enter with parenthesis around desired expression.";
    public:
    const char* what() const noexcept override;

};

class EmptyVariableName : public GraphException
{
    const std::string error_string = "Error: Entered empty variable name to command argument";
    public:
    const char* what() const noexcept override;


};
class EmptyArgument : public GraphException
{
    const std::string error_string;
    public:
    EmptyArgument(const std::string& command) : error_string(std::string("Error: missing argument for command: ") + command){}
    const char* what() const noexcept override;

};

class FileInaccessible : public GraphException
{
    const std::string error_string;
    public:
    FileInaccessible(const std::string& filename, const std::string& mode) : error_string(std::string("Error: unable to access file named ") + filename + " in " + mode + " mode"){}
    const char* what() const noexcept override;

};

class FileReadWriteError : public GraphException
{
    const std::string error_string;
    public:
    FileReadWriteError(const std::string& filename, const std::string& mode) : error_string(std::string("Error: Encountered error while ") + mode + "ing file "+filename){}
    const char* what() const noexcept override;
};

//DB Exceptions
class DBReadException : public DBException
{
    const std::string error_string;
    public:
    DBReadException(const std::string fac_name) : error_string(std::string("Error: Cannot Resolve Faculty Database file for ")+ fac_name + std::string(". Please create or import one to load Faculty.")){}
    const char* what() const noexcept override;
};

class DBWriteException : public DBException
{
    const std::string error_string;
    public:
    DBWriteException(const std::string path) : error_string(std::string("Error: Cannot Open file on ")+ path  + std::string("for writing.")){}
    const char* what() const noexcept override;
};

class CorruptFile : public DBException
{
    const std::string error_string;
    public:
    CorruptFile(const std::string filename) : error_string(std::string("Error: Corrupt format in file ") + filename){}
    const char* what() const noexcept override;

};
class IdNotInDB : public DBException
{
    const std::string error_string;
    public:
    IdNotInDB(const std::string id) : error_string(std::string("Error: ID given is not in course database: ") + id){}
    const char* what() const noexcept override;


};
#endif
