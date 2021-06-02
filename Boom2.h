#ifndef BOOM2_H
#define BOOM2_H

#include "AVL.h"
#include "DynamicArray.h"
#include "CourseList.h"
#include "CourseClass.h"
#include "CourseHash.h"



class Boom2
{
    AVL<CourseClass> classes;
    CourseHash courses;
    int classes_num; // number of viewed classes in system

    public:
    Boom2() : classes_num(0){}
    Status addCourse(int courseID);
    Status removeCourse(int courseID);
    Status addClass(int courseID, int* classID);
    Status watchClass(int courseID, int classID, int time);
    Status timeViewed(int courseID, int classID, int* time_viewed);
    Status getIthWatchedClass(int i, int* courseID, int* classID);

    private:
    void removeCourseFromTree(Node<CourseClass>* root, int courseID, int* removed_num); //remove all classes with given course ID from tree - return number of elements removed via given removed_num ptr
    void findIth (Node<CourseClass>* root, int* classID, int* courseID, int* iter_num, int i);
};















#endif