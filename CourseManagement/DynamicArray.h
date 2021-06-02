/*
Class implementing a dynamic array, with size being doubled and halved according to array capacity.
*/

#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "Exceptions.h"

const int DEFAULT_SIZE = 10;



template <class T>
class DynamicArray
{
    T* array;
    int size;

    public:
    DynamicArray(int init_size = DEFAULT_SIZE);
    ~DynamicArray();
    const T& operator[] (int index) const;
    T& operator[] (int index) ;
    void expand(double factor);
    int getCapacity() const;
    int realloc(double update_factor);
};



template <class T>
DynamicArray<T>::DynamicArray(int init_size) : size(init_size)
{
    if(init_size<1)
    {
        throw IllegalArraySize();
    }
    array = new T [init_size];
}

template <class T>
DynamicArray<T>::~DynamicArray()
{
    delete[] array;
}

template <class T>
const T& DynamicArray<T>::operator[] (int index) const
{
    if(index >= size || index < 0)
    {
        throw IllegalIndex();
    }
    return *(array+index);
}

template <class T>
T& DynamicArray<T>::operator[] (int index) 
{
    if(index >= size || index < 0)
    {
        throw IllegalIndex();
    }
    return *(array+index);
}

template <class T>
void DynamicArray<T>::expand(double factor)
{
    T* new_array = new T [(int)(size*factor)];
    for(int i=0;i<size; i++)
    {
        new_array[i] = array[i];
    }
    delete array;
    array = new_array;
    size = (int)(size*factor);
}

template <class T>
int DynamicArray<T>::getCapacity() const
{
    return size;
}



// template <class T>
// int DynamicArray<T>::realloc(double update_factor)
// {
//     DynamicArray<T> new_array = DynamicArray<T>((int)(update_factor*array.getCapacity()));
//     int new_course_list_size = 0;
//     for(int i=0;i<array.getCapacity(); i++)
//     {
//         ListNode* iter = array[i].head;
//         while(iter)
//         {
//             new_array[iter->data->GetCourseID()%new_array.getCapacity()].add_allocated(iter);
//             if(new_array[iter->data->GetCourseID()%new_array.getCapacityy()].size ==1)
//             {
//                 new_course_list_size++;
//             }
//             iter = iter->next;

//         }
//         array[i].head = nullptr;
//     }
//     delete array;
//     this = new_array;
//     array.size = (int)(array.size*update_factor);
//     return new_course_list_size;




// }





#endif