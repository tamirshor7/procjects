#include "CourseList.h"
#include <assert.h>

 
CourseList::~CourseList()
{
    ListNode* iter = head;
    ListNode* target;
    while(size && iter)
    {
        target = iter;
        iter = iter->next;
        delete target;
    }

}


Status CourseList::add(int id)
{
    assert(id>0);
    if(size && idInList(id))
    {
        return FAILED;
    }
    Course* new_course = new Course(id);
    ListNode* target = new ListNode(new_course, head);
    if(!size)
    {
        head = nullptr; //to avoid using non-default constructor - structure needs to be in array
    }
    
    
    target->next = head;
    head = target;
    size++;
    
    return SUCCEEDED;
    

}

Status CourseList::remove(int id)
{
    if(id <= 0)
    {
        return INPUT_INVALID;
    }
    if(!size)
    {
        return FAILED;
    }
    ListNode* iter = head;
    ListNode* target;

    if(size == 1)
    {
        head = nullptr;
        size--;
        delete iter;
        return SUCCEEDED;
    }
    while(iter->next)
    {
        if(iter->next->data->GetCourseID() == id)
        {
            target = iter->next;
            iter->next = iter->next->next;
            delete target;
            size--;
            return SUCCEEDED;
        }
        iter = iter->next;
    }

    return FAILED;

}
Course* CourseList::idInList(int id)
{
    if(!size)
    {
        return nullptr;
    }
    ListNode* iter = head;
    while(iter)
    {
        if(iter->data->GetCourseID() == id)
        {
            return iter->data;
        }
        iter = iter->next;
    }
    return nullptr;
}



void CourseList::add_allocated(ListNode* node)
{
    if(!size)
    {
        head = nullptr; 
    }


    node->next = head;
    head = node;
    size++;

}
