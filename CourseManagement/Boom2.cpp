#include "Boom2.h"



Status Boom2::addCourse(int courseID)
{
    return courses.addCourse(courseID);
}

Status Boom2::removeCourse(int courseID)
{
    if(courseID <= 0)
    {
        return INPUT_INVALID;
    }
    Course* target = courses.getCourseByID(courseID);
    if(!target)
    {
        return FAILED;
    }
    try
    {
        int removed_count = 0;
        for(int i=0; i<target->GetSize();i++)
        {
            if(classes.remove(CourseClass(courseID,i,target->GetMinutesWatched(i))) == SUCCEEDED)
            {
                removed_count++;
            }
        }
        classes_num -= removed_count;
        courses.removeCourse(courseID);
       
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOC_ERROR;
    }
    return SUCCEEDED;
   

}

 Status Boom2::addClass(int courseID, int* classID)
 {
    if(courseID <= 0)
    {
        return INPUT_INVALID;
    }
    Course* target = courses.getCourseByID(courseID);
    if(!target)
    {
        return FAILED;
    }
    try
    {
        target->addClass(classID); //updates pointer and adds class
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOC_ERROR;
    }
    return SUCCEEDED;

 }


Status Boom2::watchClass(int courseID, int classID, int time)
{
    if(time <= 0 || courseID <= 0 || classID<0)
    {
        return INPUT_INVALID;
    }
    Course* target  = courses.getCourseByID(courseID);
    if(target && target->GetSize()<= classID)
    {
        return INPUT_INVALID;
    }

    if(!target)
    {
        return FAILED;
    }
    int prev_view_time = target->GetMinutesWatched(classID);
    target->SetMinutesWatched(classID,time);
    
    
    classes.remove(CourseClass(courseID,classID,prev_view_time));
    
    CourseClass addition = CourseClass(courseID, classID,prev_view_time + time);
    try
    {
        classes.add(addition);
        if(!prev_view_time)
        {
            classes_num++;
        }
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOC_ERROR;
    }
    return SUCCEEDED;


}

Status Boom2::timeViewed(int courseID, int classID, int* time_viewed)
{
    
    if(courseID <= 0 || classID < 0)
    {
        return INPUT_INVALID;
    }
    Course* target = courses.getCourseByID(courseID);
    if (target&& classID >= target->GetSize())
    {
        return INPUT_INVALID;
    }
    if(!target)
    {
        return FAILED;
    }
    *time_viewed = target->GetMinutesWatched(classID);
    return SUCCEEDED;

}

Status Boom2::getIthWatchedClass(int i, int* courseID, int* classID)
{
    if(i<=0)
    {
        return INPUT_INVALID;
    }
    if(i>classes_num)
    {
        return FAILED;
    }
    int iter_num_val = 0;
    int* iter_num = &iter_num_val;
    findIth(classes.root, classID, courseID, iter_num, i);
    return SUCCEEDED;

}

void Boom2::removeCourseFromTree(Node<CourseClass>* root,int courseID, int* removed_num)
{
    if(root->left)
    {
        removeCourseFromTree(root->left, courseID,removed_num);
    }

    if(root->right)
    {
        removeCourseFromTree(root->right, courseID, removed_num);
    }
    if(root->data.GetCourseID() == courseID)
    {
        *removed_num = *removed_num + 1;
        classes.remove(root->data);
    }
    
}

void Boom2::findIth (Node<CourseClass>* root, int* classID, int* courseID, int* iter_num, int i)
{
    if(root->right)
    {
        findIth(root->right, classID, courseID, iter_num, i);
    }

    *iter_num = *iter_num+1;
    if(*iter_num == i)
    {
        *courseID = root->data.GetCourseID();
        *classID = root->data.GetClassID();
        return;
    }

     if(root->left)
    {
        findIth(root->left, classID, courseID, iter_num, i);
    }

    
}