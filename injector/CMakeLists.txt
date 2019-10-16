cmake_minimum_required (VERSION 3.8)

set(SOURCE_FILES
    src/injector.cpp
    src/flipper.cpp
    src/memory.cpp
    src/process.cpp
    src/args.cpp
    src/time.cpp
    src/fault.cpp
    src/sticker.cpp
    src/pmparser.c)

add_library(cxxopts cxxopts/include/cxxopts.hpp)
set_target_properties(cxxopts PROPERTIES LINKER_LANGUAGE CXX)
add_library(cppzmq cppzmq/zmq.hpp cppzmq/zmq_addon.hpp)
set_target_properties(cppzmq PROPERTIES LINKER_LANGUAGE CXX)

include_directories(include)
include_directories(cxxopts/include)
include_directories(cppzmq)

# Add source to this project's executable.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pthread")
add_executable (injector ${SOURCE_FILES})
target_link_libraries(injector cxxopts)
target_link_libraries(injector cppzmq)
