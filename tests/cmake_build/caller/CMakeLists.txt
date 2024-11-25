cmake_minimum_required(VERSION 3.10)
project(kwargscpp_caller)

find_package(kwargscpp REQUIRED)

add_executable(kwargscpp_caller caller.cpp)
target_link_libraries(kwargscpp_caller kwargscpp::kwargscpp)
