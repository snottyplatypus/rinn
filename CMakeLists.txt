# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("rinn")

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)
add_definitions(-D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if(NOT BOOST_ROOT)
	set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/lib/boost)
endif(NOT BOOST_ROOT)
set(SDL2_PATH ${CMAKE_SOURCE_DIR}/lib/SDL2)
set(TCOD_PATH ${CMAKE_SOURCE_DIR}/lib/libtcod)

find_package(CGAL REQUIRED)
find_package(SDL2 REQUIRED)
set(TCOD_LIBRARY ${TCOD_PATH}/lib/libtcod.lib;${TCOD_PATH}/lib/libtcodgui.lib)

set(TCOD_INCLUDE_DIR ${TCOD_PATH}/include)
set(CEREAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/cereal/include)
if(NOT CGAL_ROOT)
	set(CGAL_ROOT ${CMAKE_SOURCE_DIR}/lib/CGAL)
endif(NOT CGAL_ROOT)
set(CGAL_INCLUDE_DIR ${CGAL_ROOT}/include)

include(${CGAL_USE_FILE})
include(CGAL_CreateSingleSourceCGALProgram)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)

# Include sub-projects.
add_subdirectory ("scroll")
add_subdirectory ("rinn")