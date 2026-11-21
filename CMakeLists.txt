cmake_minimum_required(VERSION 3.10)
project(PathPulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Everything except main() lives in a library so the tests can reuse it.
add_library(pathpulse_core
  src/Maze.cpp
  src/Generators.cpp
  src/Solvers.cpp)
target_include_directories(pathpulse_core PUBLIC src)

add_executable(pathpulse src/main.cpp)
target_link_libraries(pathpulse PRIVATE pathpulse_core)

enable_testing()
add_executable(pathpulse_tests tests/test_pathpulse.cpp)
target_link_libraries(pathpulse_tests PRIVATE pathpulse_core)
add_test(NAME pathpulse_tests COMMAND pathpulse_tests)

foreach(target pathpulse_core pathpulse pathpulse_tests)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
  endif()
endforeach()
