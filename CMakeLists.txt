cmake_minimum_required(VERSION 3.10)
project(PathPulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(pathpulse
  src/main.cpp
  src/Maze.cpp
  src/Generators.cpp
  src/Solvers.cpp)

if(MSVC)
  target_compile_options(pathpulse PRIVATE /W4)
else()
  target_compile_options(pathpulse PRIVATE -Wall -Wextra -pedantic)
endif()
