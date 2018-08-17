cmake_minimum_required (VERSION 3.8)

project("rinn")

include_directories(./include)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${TCOD_INCLUDE_DIR})
include_directories(${CEREAL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/scroll)

add_executable (rinn "rinn.cpp" "rinn.h")
target_link_libraries(rinn PUBLIC scroll)