project(ptut)
cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE FILELIST "*.h" "*.hpp" "*.cpp")
add_custom_target(files SOURCES ${FILELIST})

set(SRCS ";")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(${CMAKE_PROJECT_NAME} ${SRCS})
target_link_libraries(${CMAKE_PROJECT_NAME} pq)
