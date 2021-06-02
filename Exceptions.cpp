#include "Exceptions.h"

//Node Exceptions

const char* IllegalSemicolon::what() const noexcept
{
    return error_string.c_str();
}
const char* IllegalBracketSeries::what() const noexcept
{
    return error_string.c_str();
}

const char* IllegalCharacter::what() const noexcept
{
    return error_string.c_str();
}

const char* EmptyNodeName::what() const noexcept
{
    return error_string.c_str();
}

//EdgeExceptions

const char* EmptyEdgeName::what() const noexcept
{
    return error_string.c_str();
}

const char* IllegalEdgeIndicatorComposition::what() const noexcept
{
    return error_string.c_str();
}

const char* AttemptedToCreateSelfLoop::what() const noexcept
{
    return error_string.c_str();
}

//Graph Exceptions
const char* UnknownVariable::what() const noexcept 
{
    return error_string.c_str();
}

const char* UnrecognizedCommand::what() const noexcept 
{
    return error_string.c_str();
}

const char* InvalidVariableName::what() const noexcept 
{
    return error_string.c_str();
}

const char* IllegalGraphLiteral::what() const noexcept 
{
    return error_string.c_str();
}

const char* FoundNodeDuplication::what() const noexcept 
{
    return error_string.c_str();
}
const char* FoundEdgeDuplication::what() const noexcept 
{
    return error_string.c_str();
}

const char* EdgesWithUnknownNodes::what() const noexcept 
{
    return error_string.c_str();
}

const char* IllegalParenthesisSeries::what() const noexcept 
{
    return error_string.c_str();
}

const char* EmptyCommandEntered::what() const noexcept 
{
    return error_string.c_str();
}

const char* MissingParenthesis::what() const noexcept 
{
    return error_string.c_str();
}

const char* EmptyVariableName::what() const noexcept 
{
    return error_string.c_str();
}

const char* EmptyArgument::what() const noexcept 
{
    return error_string.c_str();
}

const char* FileInaccessible::what() const noexcept 
{
    return error_string.c_str();
}

const char* FileReadWriteError::what() const noexcept 
{
    return error_string.c_str();
}

const char* DBReadException::what() const noexcept 
{
    return error_string.c_str();
}
const char* DBWriteException::what() const noexcept 
{
    return error_string.c_str();
}

const char* CorruptFile::what() const noexcept 
{
    return error_string.c_str();
}

const char* NonNumericalID::what() const noexcept 
{
    return error_string.c_str();
}
 
const char* IdNotInDB::what() const noexcept 
{
    return error_string.c_str();
}
