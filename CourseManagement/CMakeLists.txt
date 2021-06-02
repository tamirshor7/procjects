cmake_minimum_required (VERSION 3.0.0)
project(ex1q4)
#set (MTM_FLAGS_DEBUG "-std=gnu++11 -Wall --pedantic-errors -Werror")
set (MTM_FLAGS_DEBUG "-std=gnu++11")
set (MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG")
set (CMAKE_CXX_FLAGS ${MTM_FLAGS_DEBUG})
#add_executable(draft2 draft.cpp)
add_executable(drf Boom2.cpp Course.cpp CourseHash.cpp CourseList.cpp Exceptions.cpp Faculty.cpp Graph.cpp library2.cpp main.cpp Node_s.cpp runView.cpp utils.cpp)
#add_executable(draft2 old_smash.cpp old_coms.cpp old_signals.cpp)


