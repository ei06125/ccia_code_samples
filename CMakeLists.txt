cmake_minimum_required(VERSION 3.21)

project(thread_safe_project)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

add_library(thread_safe INTERFACE)
target_include_directories(thread_safe INTERFACE include)

add_subdirectory(tests)