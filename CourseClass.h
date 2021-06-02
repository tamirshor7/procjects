#ifndef COURSECLASS_H
#define COURSECLASS_H

//Class representing all data fields related to certain course class.
class CourseClass {
private:
    int courseID;
    int classID;
    int minutesWatched;
public:
    //constructor of CourseClass.
    CourseClass(int courseID, int classID, int minutesWatched) : courseID(courseID)\
    ,classID(classID),minutesWatched(minutesWatched){}

    //Return course id of the course class.
    int GetCourseID() const{return courseID;}

    //Return class id of the course class.
    int GetClassID() const{return classID;}

    //Return total number of minutes watched in course class.
    int GetMinutesWatched() const{return minutesWatched;}

    //Return whether the course classes are equivalent according to the format.
    inline bool operator==(const CourseClass cls) const;

    //Return whether the course class "cls" is smaller then the course class "cls1" according to the format.
    inline bool operator<(const CourseClass cls) const;

    //Return whether the course class "cls" is bigger then the course class "cls1" according to the format.
    inline bool operator>(const CourseClass cls) const;
};

    bool CourseClass::operator<(const CourseClass cls) const 
    {
        if (minutesWatched < cls.minutesWatched)
            return true;
        else if (minutesWatched == cls.minutesWatched)
        {
            if (courseID > cls.courseID)
                return true;
            else if(courseID == cls.courseID)
                return classID > cls.classID;
        }
        return false;
    }

    bool CourseClass::operator>(const CourseClass cls) const
    {
        return(!((*(this))<cls)&&!((*(this))==cls));
    }

    bool CourseClass::operator==(const CourseClass cls) const
    {
        return ((minutesWatched == cls.minutesWatched) && (classID == cls.classID)) && (courseID == cls.courseID);
    }
#endif //COURSECLASS_H



