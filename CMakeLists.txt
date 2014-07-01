project (OFN)
cmake_minimum_required (VERSION 2.6)

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall")

set (OFN_FILES
  ./src/ofn.c
  ./src/database.c
)

add_executable (commit ${OFN_FILES} src/commit.c)
add_executable (search ${OFN_FILES} src/search.c)

target_link_libraries (commit puzzle sqlite3)
target_link_libraries (search puzzle sqlite3)
