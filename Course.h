#ifndef COURSE_H
#define COURSE_H

#include "DynamicArray.h"
#include "Utils.h"

const double EXPANSION_RATE = 2.0;
const double DIVISION_RATE = 0.5;

//Class representing all data fields related to certain course class.
class Course 
{
private:
    int courseID;
    DynamicArray<int> classes; // array of all course classes - each element in array is class's overall view time
    int size; //number of classes currently held
public:

    //constructor of Course
    Course(int courseID) : courseID(courseID), size(0){};

    //destructor of Course
    ~Course() = default;

    //add class with 0 view time to Course classes array
    void addClass(int* classID);

    //Return course id of object.
    int GetCourseID() const{return courseID;}

    //Return number of classes of object
    const int GetSize() const {return size;}

    //Return total number of minutes watched in class number class_num in course.
    int GetMinutesWatched(int class_num) const{return classes[class_num];}

    //Update total number of minutes watched in class number class_num in course.
    void SetMinutesWatched(int class_num, int time) {classes[class_num] += time;}

    //Return whether two courses are equal according to their course ID.
    bool operator==(const Course& course) const;

    //Return whether course1 is less than course2 according to their course ID.
    bool operator<(const Course& course) const;

    //Return whether course1 is greater than course2 according to their course ID.
    bool operator>(const Course& course) const;

    private:
    bool arrayFull() const
    {
        return size == classes.getCapacity();
    }
    

   
};

    
#endif //COURSE_H