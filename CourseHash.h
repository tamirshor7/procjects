/*
Class implementing a hash table of Course Object instances, with the hashing function being the mod value of
the current array size. Collosions are solved in chain-hashing. 
*/


#ifndef HASH_H
#define HASH_H

#include "CourseList.h"
#include "DynamicArray.h"
#include "Utils.h"


class CourseHash
{
    DynamicArray<CourseList>* courses;
    int occupancy;

    public:
    CourseHash();
    ~CourseHash();
    Status addCourse(int courseID);
    void removeCourse(int courseID);
    void realloc(double factor);
    Course* getCourseByID(int courseID) {return (*courses)[courseID%((*courses).getCapacity())].idInList(courseID);} //retrieve course from table based on ID
    void resetLists(DynamicArray<CourseList>* array); //reset size field to 0
};


#endif