cmake_minimum_required(VERSION 3.3)
project(A1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Main Main/BufferMgr/headers/CheckLRU.h Main/BufferMgr/headers/MyDB_Page.h Main/BufferMgr/source/MyDB_page.cpp Main/BufferMgr/source/CheckLRU.cpp)
add_executable(A1 ${SOURCE_FILES})