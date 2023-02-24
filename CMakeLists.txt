cmake_minimum_required(VERSION 3.24)
project(develdb VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEVELDB_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party
        )


include_directories(
        ${PROJECT_SOURCE_DIR}
        ${DEVELDB_THIRD_PARTY_INCLUDE_DIR}
)
include_directories(BEFORE db) # This is needed for gtest.

add_library(develdb "")
target_sources(develdb
        PRIVATE
        "${PROJECT_SOURCE_DIR}/db/skiplist_test.cc"
        "${PROJECT_SOURCE_DIR}/db/skiplist.hpp"
        "${PROJECT_SOURCE_DIR}/util/random.h"
        )

add_executable(skiplist_test db/skiplist_test.cc)
target_link_libraries(skiplist_test gtest gtest_main)

add_subdirectory(third_party)
