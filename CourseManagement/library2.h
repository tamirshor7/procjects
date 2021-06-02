/****************************************************************************/
/*                                                                          */
/* Class implementing Interface for Course viewing system
Supported functionallity and Complexity, n being the number of courses in the system, m being the overall number of lectures:

Overall class space complexity: O(n+m)

void * Init() - initialize all necessary datasturctes in Constant time. System is empty of any courses and classes after init.

StatusType AddCourse(void* DS, int courseID) - 

add a new course to the viewing with given ID, initialized with 0 lectures.
O(1) on average amortized time.

StatusType RemoveCourse(void* DS, int courseID)- 

remove course from system, with all corresponding lectures in said course, and deallocate relevant 
resources. O(m*log(M)) time, m being number of lectures of said course.
M is the number of overall lectures in the system.

StatusType AddClass(void* DS, int courseID, int* classID) - 

add a new class, with lowest id available, to given course. O(1) in amortized average.
                                                    
StatusType WatchClass(void* DS, int courseID, int classID, int time)- 

Watch passed class from passed course, for an added time of given parameter. O(log(m)) on average.

StatusType TimeViewed(void* DS, int courseID, int classID, int* timeViewed) - 

retrieve overall viewing time of a certain class  into given time Viewed ptr. O(1) on average.

StatusType GetIthWatchedClass(void* DS, int i, int* courseID, int* classID)-

retrieve the Ith watched class, out of all classes of all courses in the database. Sorting is by the following 
parameters, in decreasing importance:
                                    1.Largest Lecture View Time.
                                    2.Lowest Course ID
                                    3.Lowest Lecture ID
O(log(m)), worst case.

void Quit(void **DS) - 
End session and deallocate all resources, in O(n+m) time (Worst Case Analysis).
                                                                              
*/

/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* File Name : library.h                                                   */
/*                                                                          */
/****************************************************************************/

#ifndef VIEW
#define VIEW

#ifdef __cplusplus
extern "C" {
#endif

/* Return Values
 * ----------------------------------- */
typedef enum {
    SUCCESS = 0,
    FAILURE = -1,
    ALLOCATION_ERROR = -2,
    INVALID_INPUT = -3
} StatusType;


void *Init();

StatusType AddCourse(void* DS, int courseID);

StatusType RemoveCourse(void *DS, int courseID);

StatusType AddClass(void* DS, int courseID, int* classID);

StatusType WatchClass(void *DS, int courseID, int classID, int time);

StatusType TimeViewed(void *DS, int courseID, int classID, int *timeViewed);

StatusType GetIthWatchedClass(void* DS, int i, int* courseID, int* classID);

void Quit(void** DS);

#ifdef __cplusplus
}
#endif

#endif   