cmake_minimum_required(VERSION 3.31)
project(C__Course_Coroutines)

set(CMAKE_CXX_STANDARD 20)

add_executable(SimpleWait simple_wait/main.cc)
add_executable(Tasks tasks/main.cc)
