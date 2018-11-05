cmake_minimum_required (VERSION 3.8)

project("rinn")

include_directories(./include)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${TCOD_INCLUDE_DIR})
include_directories(${CEREAL_INCLUDE_DIR})
include_directories(${CGAL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/scroll)

file(GLOB RINN_SOURCES "src/*.cpp")
file(GLOB RINN_SOURCES "src/**/*.cpp")

message(STATUS "Rinn sources:")
message(STATUS ${RINN_SOURCES})

add_executable(rinn "rinn.cpp" ${RINN_SOURCES})
target_link_libraries(rinn PUBLIC scroll)
add_executable(data_visualizer "visualizer/data_visualizer.cpp" ${RINN_SOURCES})
target_link_libraries(data_visualizer PUBLIC scroll)