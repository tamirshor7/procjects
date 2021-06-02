#include "library2.h"
#include "Boom2.h"

void *Init()
{
    try
    {
        Boom2* ptr = new Boom2();
        return ptr;
    }
    catch(const std::bad_alloc& e)
    {
        return nullptr;
    }
    
}

StatusType AddCourse (void *DS, int courseID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }

    return StatusType(((Boom2*)DS)->addCourse(courseID));
}

StatusType RemoveCourse(void* DS, int courseID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }

    return StatusType(((Boom2*)DS)->removeCourse(courseID));

}

StatusType AddClass(void* DS, int courseID, int* classID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }
    return StatusType(((Boom2*)DS)->addClass(courseID,classID));
}

StatusType WatchClass(void *DS, int courseID, int classID, int time)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }

    return StatusType(((Boom2*)DS)->watchClass(courseID,classID,time));
}

StatusType TimeViewed(void *DS, int courseID, int classID, int *timeViewed)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }

    return StatusType(((Boom2*)DS)->timeViewed(courseID,classID,timeViewed));
}

StatusType GetIthWatchedClass(void* DS, int i, int* courseID, int* classID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }

    return StatusType(((Boom2*)DS)->getIthWatchedClass(i,courseID,classID));
    

}

void Quit(void** DS)
{
    delete *((Boom2**)DS);
    *DS = nullptr;
}