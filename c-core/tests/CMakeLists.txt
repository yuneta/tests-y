##############################################
#   CMake
##############################################
project(tests-c-core C)
cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFiles)
include(CheckSymbolExists)

set(CMAKE_INSTALL_PREFIX /yuneta/development/yuneta/^yuneta/tests-y/output)

set(INC_DEST_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(LIB_DEST_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(BIN_DEST_DIR ${CMAKE_INSTALL_PREFIX}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")


##############################################
#   sub-projects
##############################################
add_subdirectory(startup)
add_subdirectory(dynrule)
add_subdirectory(pepon-teston)

# compile in Release mode optimized but adding debug symbols, useful for profiling :
#
#     cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
#
# or compile with NO optimization and adding debug symbols :
#
#     cmake -DCMAKE_BUILD_TYPE=Debug ..
#
# Compilando para ARM poner cmake -DNOT_INCLUDE_LIBUNWIND
#
