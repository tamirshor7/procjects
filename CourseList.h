#ifndef COURSE_LIST_H
#define COURSE_LIST_H

#include "Utils.h"
#include "Course.h"

struct ListNode
{
    Course* data;
    ListNode* next;

    ListNode(Course* data, ListNode* next = nullptr) : data(data), next(next){}
    ~ListNode(){delete data;}
};

struct CourseList
{
    ListNode* head;
    int size;

    public:
    CourseList() : head(nullptr), size(0){}
    ~CourseList();
    Status add(int id);
    void add_allocated(ListNode* node); // add allocated resources to list (do not allocate)
    Status remove(int id);
    bool isEmpty() {return !size;}
    Course* idInList(int id); //retrieve course from list with given ID

    



};



#endif