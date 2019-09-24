cmake_minimum_required (VERSION 3.8)

set(SOURCE_FILES
    src/injector.cpp
    src/flipper.cpp
    src/memory.cpp
    src/process.cpp
    src/args.cpp
    src/pmparser.c)

add_subdirectory (cxxopts)

include_directories(include)
include_directories("cxxopts/include")

# Add source to this project's executable.
add_executable (injector ${SOURCE_FILES})
