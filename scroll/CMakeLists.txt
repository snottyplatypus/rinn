cmake_minimum_required (VERSION 3.8)

project("scroll")

include_directories(./include)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${TCOD_INCLUDE_DIR})
include_directories(${CEREAL_INCLUDE_DIR})
include_directories(${CGAL_INCLUDE_DIR})

file(GLOB SCROLL_SOURCES "src/*.cpp")
file(GLOB SCROLL_SOURCES "src/**/*.cpp")

message(STATUS "Scroll sources:")
message(STATUS ${SCROLL_SOURCES})

add_library(scroll STATIC ${SCROLL_SOURCES})
target_link_libraries(scroll ${SDL2_LIBRARY} ${TCOD_LIBRARY} CGAL::CGAL)