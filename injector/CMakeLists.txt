cmake_minimum_required (VERSION 3.8)

set(SOURCE_FILES
    src/injector.cpp
    src/flipper.cpp
    src/memory.cpp
    src/process.cpp
    src/args.cpp
    src/time.cpp
    src/fault.cpp
    src/pmparser.c)

add_library(cxxopts cxxopts/include/cxxopts.hpp)
set_target_properties(cxxopts PROPERTIES LINKER_LANGUAGE CXX)

include_directories(include)
include_directories(cxxopts/include)

# Add source to this project's executable.
add_executable (injector ${SOURCE_FILES})
target_link_libraries(injector cxxopts)
