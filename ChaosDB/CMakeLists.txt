cmake_minimum_required (VERSION 3.8)

set(SOURCE_FILES
    src/injector.cpp
    src/flipper.cpp
    src/memory.cpp
    src/pmparser.c)

include_directories(include)

# Add source to this project's executable.
add_executable (injector ${SOURCE_FILES})
