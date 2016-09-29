project(ptut)
cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE FILELIST "*.h" "*.hpp" "*.cpp")
add_custom_target(files SOURCES ${FILELIST})

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    Entity.cpp
    DatabaseConnection.cpp
    SimpleConfig.cpp
    World.cpp
    UUID.cpp
    TCPConnection.cpp
)
target_link_libraries(${CMAKE_PROJECT_NAME} pq)
