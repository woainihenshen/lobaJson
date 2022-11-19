cmake_minimum_required(VERSION 3.0.0)
project(lobaJson VERSION 0.1.0)

add_library(lobaJson lobaJson.cpp)

add_executable(lobaJsonTest test.cpp)
target_link_libraries(lobaJsonTest lobaJson)
