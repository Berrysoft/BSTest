cmake_minimum_required(VERSION 3.1)
project(BSTest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(include)

find_package(sf CONFIG REQUIRED)

file(GLOB BST_HEADERS "include/*.hpp")
add_library(bstest INTERFACE)
target_link_libraries(bstest INTERFACE sf::stream_format)

install(FILES ${BST_HEADERS} DESTINATION include)
install(TARGETS bstest EXPORT bstest_targets)

export(EXPORT bstest_targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/bstest/bstest-targets.cmake"
  NAMESPACE bstest::
)
configure_file(cmake/bstest-config.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/bstest/bstest-config.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/bstest)
install(EXPORT bstest_targets
  FILE
  bstest-targets.cmake
  NAMESPACE
  bstest::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/bstest-config.cmake
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

add_executable(sample sample/main.cpp)
target_link_libraries(sample INTERFACE sf::stream_format)
target_link_libraries(sample INTERFACE bstest)
