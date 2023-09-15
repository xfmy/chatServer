cmake_minimum_required(VERSION  3.0)

project(chat)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
#include_directories(${PROJECT_SOURCE_DIR}/include/db)
 
#添加vcpkg目录  
include_directories("/home/xf/install/vcpkg/installed/x64-linux/include/") 
#link_directories(chat "/home/xf/install/vcpkg/installed/x64-linux/lib")
link_directories(chat "/home/xf/install/vcpkg/installed/x64-linux/debug/lib")

add_subdirectory(src) 

 
