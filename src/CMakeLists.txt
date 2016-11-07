if(NOT DEFINED SRCS)
  message(FATAL_ERROR "Please compile using the repo's root CMakeLists.txt")
endif()

set(D ${CMAKE_CURRENT_SOURCE_DIR})
set(SRCS
  ${SRCS}
  ${D}/DatabaseConnection.cpp
  ${D}/Entity.cpp
  ${D}/main.cpp
  ${D}/SimpleConfig.cpp
  ${D}/TCPConnection.cpp
  ${D}/TCPListener.cpp
  ${D}/UUID.cpp
  ${D}/World.cpp
  PARENT_SCOPE
)
