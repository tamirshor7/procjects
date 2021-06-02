#include "Course.h"


void Course::addClass(int* classID)
{
    if(arrayFull())
    {
        classes.expand(EXPANSION_RATE);
    }
    classes[size] = 0;
    *classID = size;
    size++;
}





bool Course::operator<(const Course& course) const 
{
    return (courseID < course.courseID) ? true : false;
}

bool Course::operator>(const Course& course)  const
{
    return (!((*(this))<course) && !((*(this))==course));
    
}

bool Course::operator==(const Course& course) const 
{
    return (courseID == course.courseID);
}