#include "CourseHash.h"
#include "Utils.h"
#include "Faculty.h"
#include <string>

static double EXPANSION_FACTOR = 2.0;
static double SLICING_FACTOR = 0.5;
static int SLICING_INHIBIT = 4; 

CourseHash::CourseHash(): occupancy(0) 
{
    courses = new DynamicArray<CourseList>();
    resetLists(courses);
}

CourseHash::~CourseHash()
{
    delete courses;
}
Status CourseHash::addCourse(int courseID)
{
    if(courseID <= 0)
    {
        return INPUT_INVALID;
    }
    if(!idToName(std::to_string(courseID)))
        return FAILED;

    try
    {
        
        if((*courses)[courseID%(courses->getCapacity())].isEmpty())
        {
            occupancy++; //addition won't fail if list is empty
        }
        Status add_stat = (*courses)[courseID%(courses->getCapacity())].add(courseID);
        if(add_stat != SUCCEEDED)
        {
            return add_stat;
        }
        
        if(occupancy == courses->getCapacity())
        {
            realloc(EXPANSION_FACTOR);
        }
        return SUCCEEDED;
        
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOC_ERROR;
    }
}

 void CourseHash::removeCourse(int courseID) //remove course from hash table
 {
    (*courses)[courseID%(courses->getCapacity())].remove(courseID);
    if((*courses)[courseID%(courses->getCapacity())].isEmpty())
    {
        occupancy--; 
    }
    
    if(courses->getCapacity()*SLICING_FACTOR >= SLICING_INHIBIT && occupancy <= courses->getCapacity()*SLICING_FACTOR)
    {
        realloc(SLICING_FACTOR);
    }
   
 }

void CourseHash::realloc(double factor) // reallocate memory resources and copy contents
{
    DynamicArray<CourseList>* new_array = new DynamicArray<CourseList>((int)(factor*(*courses).getCapacity()));
    resetLists(new_array);
    occupancy = 0;
    for(int i=0;i<courses->getCapacity(); i++)
    {
        ListNode* iter = !(*courses)[i].size ? nullptr : (*courses)[i].head;
        ListNode* next_iter;
        while(iter)
        {
            next_iter = iter->next ? iter->next : nullptr;
            (*new_array)[iter->data->GetCourseID()%(*new_array).getCapacity()].add_allocated(iter);
            if((*new_array)[iter->data->GetCourseID()%(*new_array).getCapacity()].size ==1)
            {
                occupancy++;
            }
            iter = next_iter;

        }
        (*courses)[i].head = nullptr;
    }
    delete courses;
    courses = new_array;
    
}

void CourseHash::resetLists(DynamicArray<CourseList>* array) // reset list sizes to 0
{
    for(int i=0;i<array->getCapacity();i++) 
    {
        (*array)[i].size=0;
    }
        
}