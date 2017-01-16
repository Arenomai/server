project(arnserver)
cmake_minimum_required(VERSION 2.8)

### Packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(CXX14)
if (NOT CXX14_FOUND)
  message(FATAL_ERROR "C++14 support is required")
endif()
set(CMAKE_CXX_FLAGS "${CXX14_FLAGS} ${CMAKE_CXX_FLAGS}")

find_package(goodform REQUIRED)
if(NOT goodform_FOUND)
  message(FATAL_ERROR "Bundled goodform not found. Have you checked out submodules?
git submodule update --init")
endif()

### Includes
include_directories(BEFORE
  ${GOODFORM_INCLUDE_DIR}
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

### Source file globbing
# file(GLOB_RECURSE FILELIST "*.h" "*.hpp" "*.cpp")
# add_custom_target(files SOURCES ${FILELIST})


set(SRCS ";")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

include_directories(
    "/usr/include/postgresql")

target_link_libraries(${CMAKE_PROJECT_NAME}
  ${GOODFORM_LIBRARY}
  pthread
  pq)
