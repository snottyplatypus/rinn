cmake_minimum_required (VERSION 3.8)

project("scroll")

include_directories(./include)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${TCOD_INCLUDE_DIR})
include_directories(${CEREAL_INCLUDE_DIR})

file(GLOB SOURCES "src/*.cpp")
file(GLOB SOURCES "src/**/*.cpp")

message(STATUS ${SOURCES})

add_library(scroll STATIC ${SOURCES})
target_link_libraries(scroll ${SDL2_LIBRARY} ${TCOD_LIBRARY})