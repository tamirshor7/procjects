cmake_minimum_required (VERSION 3.0.0)
project(ex1q4)
#set (MTM_FLAGS_DEBUG "-std=gnu++11 -Wall --pedantic-errors -Werror")
set (MTM_FLAGS_DEBUG "-std=gnu++11")
set (MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG")
set (CMAKE_CXX_FLAGS ${MTM_FLAGS_DEBUG})
add_executable(smash smash.cpp Commands.cpp signals.cpp)
#add_executable(drf draft1.cpp)



