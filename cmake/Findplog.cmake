set(PLOG_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/plog")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D'htonll(x)=((1==htonl(1))?(x):((uint64_t)htonl((x)&0xFFFFFFFF)<<32)|htonl((x)>>32))' -D'ntohll(x)=((1==ntohl(1))?(x):((uint64_t)ntohl((x)&0xFFFFFFFF)<<32)|ntohl((x)>>32))'")

add_subdirectory("${PLOG_ROOT_DIR}")

set(PLOG_INCLUDE_DIR "${PLOG_ROOT_DIR}/include")
if(NOT EXISTS "${PLOG_INCLUDE_DIR}")
  set(PLOG_INCLUDE_DIR)
endif()

set(PLOG_LIBRARY plog)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(plog
  FOUND_VAR plog_FOUND
  REQUIRED_VARS PLOG_INCLUDE_DIR PLOG_LIBRARY)

mark_as_advanced(PLOG_INCLUDE_DIR PLOG_LIBRARY)
