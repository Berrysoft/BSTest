cmake_minimum_required(VERSION 3.1)
project(BSTest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(include)

find_path(SF_INCLUDE_DIR NAMES sf/format.hpp sf/sformat.hpp)
include_directories(${SF_INCLUDE_DIR})

function(add_headers VAR)
  set(headers ${${VAR}})
  foreach (header ${ARGN})
    set(headers ${headers} include/${header})
  endforeach()
  set(${VAR} ${headers} PARENT_SCOPE)
endfunction()

add_headers(BST_HEADERS bstest.hpp)

install(FILES ${BST_HEADERS} DESTINATION include)

add_executable(sample sample/main.cpp)
