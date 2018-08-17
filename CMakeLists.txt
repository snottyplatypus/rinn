# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("rinn")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(SDL2_PATH ${CMAKE_SOURCE_DIR}/lib/SDL2)
set(TCOD_PATH ${CMAKE_SOURCE_DIR}/lib/libtcod)

find_package(SDL2 REQUIRED)
set(TCOD_LIBRARY ${TCOD_PATH}/lib/libtcod.lib;${TCOD_PATH}/lib/libtcodgui.lib)

set(TCOD_INCLUDE_DIR ${TCOD_PATH}/include)
set(CEREAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/cereal/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)

# Include sub-projects.
add_subdirectory ("scroll")
add_subdirectory ("rinn")
