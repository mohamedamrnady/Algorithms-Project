cmake_minimum_required(VERSION 3.29)
project(Algorithms_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(Task01)
include_directories(Task07)

add_executable(
        task1_cli
        Task01/main.cpp
        Task01/Board.hpp
)
 
if (WIN32)
    add_executable(task1_gui WIN32
             Task01/gui_main.cpp
             Task01/Board.hpp
    )
endif()

add_executable(task7_DP
        Task07/Job.hpp
        Task07/job_processing_DP.cpp "Task10/main.cpp")
add_executable(
        task7_BF
        Task07/Job.hpp
        Task07/job_processing_BF.cpp
 "Task10/main.cpp")
