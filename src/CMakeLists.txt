if(NOT DEFINED SRCS)
  message(FATAL_ERROR "Please compile using the repo's root CMakeLists.txt")
endif()

set(D ${CMAKE_CURRENT_SOURCE_DIR})
set(SRCS
  ${SRCS}
  ${D}/io/FileStream.cpp
  ${D}/io/MemoryStream.cpp
  ${D}/io/Stream.cpp
  ${D}/net/DatabaseConnection.cpp
  ${D}/net/Network.cpp
  ${D}/Battle.cpp
  ${D}/Card.cpp
  ${D}/ClientThread.cpp
  ${D}/Entity.cpp
  ${D}/EntityContainer.cpp
  ${D}/main.cpp
  ${D}/Player.cpp
  ${D}/SimpleConfig.cpp
  ${D}/net/TCPConnection.cpp
  ${D}/net/TCPListener.cpp
  ${D}/UUID.cpp
  ${D}/World.cpp
  PARENT_SCOPE
)
