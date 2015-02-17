# Copyright (c) 2015 Mikkel Kroman, All rights reserved.
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library.

# Set the project name
project (OFN)

# Set the minimum required version of CMake
cmake_minimum_required (VERSION 2.6)

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Set the compiler standards flag
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

find_package (LibPuzzle REQUIRED)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSPDLOG_DEBUG_ON")

# Set project variables
set (OFN_URL "https://github.com/mkroman/ofn")
set (OFN_DESCRIPTION "Image recognition tool to determine if an image has been seen before.")

# Set source files
set (ofn_SOURCE
  src/OFN/main.cpp
  src/OFN/Context.cpp
  src/OFN/Database.cpp
  src/OFN/Image.cpp
)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/spdlog/include)

add_executable (ofn ${ofn_SOURCE})
target_link_libraries (ofn ${LIBPUZZLE_LIBRARY} sqlite3)
