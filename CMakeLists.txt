cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

#set(STINKY_VERSION ${GLM_VERSION_MAJOR}.${GLM_VERSION_MINOR}.${GLM_VERSION_PATCH}.${GLM_VERSION_REVISION})
project(wunder VERSION 1.0.0 LANGUAGES CXX C)
#message(STATUS "Stinky : Version " ${STINKY_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

#Fetch common dependencies
include(cmake/FetchDependencies.cmake)

set(OPENGL_LIB glad)
set(WINDOW_ABSTRACTION_LIB glfw)

#Submodules
add_subdirectory(wunder-renderer)
add_subdirectory(wunder-sandbox)

add_library(wunder::wunder-renderer ALIAS wunder-renderer)
add_executable(wunder::wunder-sandbox ALIAS wunder-sandbox)
