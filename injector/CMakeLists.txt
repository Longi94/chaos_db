﻿cmake_minimum_required (VERSION 3.8)

set(SOURCE_FILES
    src/injector.cpp
    src/flipper.cpp
    src/memory.cpp
    src/process.cpp
    src/args.cpp
    src/time.cpp
    src/fault.cpp
    src/sticker.cpp
    src/server.cpp
    src/database.cpp
    src/pmparser.c)

add_library(cxxopts cxxopts/include/cxxopts.hpp)
set_target_properties(cxxopts PROPERTIES LINKER_LANGUAGE CXX)

include_directories(include)
include_directories(cxxopts/include)
include_directories(/usr/include)
link_directories(/usr/lib)

# Add source to this project's executable.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pthread -lsqlite3")
add_executable (injector ${SOURCE_FILES})
target_link_libraries(injector cxxopts)
target_link_libraries(injector sqlite3)
