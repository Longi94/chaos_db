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

include_directories(include)
include_directories(cxxopts/include)

add_library(objlib OBJECT ${SOURCE_FILES})
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# Add source to this project's executable.
add_executable(injector $<TARGET_OBJECTS:objlib>)
target_link_libraries(injector cxxopts)

set(CMAKE_CXX_FLAGS "-fPIC")
add_library(inject SHARED $<TARGET_OBJECTS:objlib>)
target_link_libraries(inject cxxopts)
