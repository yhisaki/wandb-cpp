cmake_minimum_required(VERSION 3.16)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20) # C++20...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wno-long-long -pedantic")
set(JSON_BuildTests OFF CACHE INTERNAL "")

option(BUILD_WANDBCPP_EXE OFF)

project(wandb-cpp)

find_library(PTHREAD_LIBRARY pthread)
find_package(Python3 COMPONENTS Development NumPy)

file(GLOB src_files src/*.cpp)

add_library(wandbcpp ${src_files})

target_include_directories(wandbcpp PUBLIC 
  /usr/local/include
  ${Python3_NumPy_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wandbcpp PUBLIC
  Python3::Python
  Python3::NumPy
  ${PTHREAD_LIBRARY}
)

if(BUILD_WANDBCPP_EXE)
  add_subdirectory(examples)
endif(BUILD_WANDBCPP_EXE)
