cmake_minimum_required (VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

set(VCPKG_INSTALL_OPTIONS --no-print-usage) # Suppress vcpkg usage message.

project(
    rinn # Project name, change this as needed.
    LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")  # Keep all runtime files in one directory.

include_directories(${PROJECT_SOURCE_DIR}/rinn/include) # Include the header files.
include_directories(${PROJECT_SOURCE_DIR}/scroll) # Include scroll header files.

file( # Find all source files in the src directory.
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS  # Automatically reconfigure if source files are added/removed.
    ${PROJECT_SOURCE_DIR}/rinn/src/*.cpp
)

# Add executables.
add_executable(rinn "rinn/rinn.cpp" ${SOURCE_FILES})
add_executable(data_visualizer "tools/data_visualizer.cpp" ${SOURCE_FILES})

# Ensure the C++23 standard is available.
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Enforce UTF-8 encoding on MSVC.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# Enable warnings recommended for new projects.
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Include sub-projects.
add_subdirectory ("scroll")

# Link libraries.
find_package(SDL2 CONFIG REQUIRED)
find_package(libtcod CONFIG REQUIRED)
find_package(CGAL CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        libtcod::libtcod
		CGAL::CGAL
		cereal::cereal
        scroll
)
target_link_libraries(
    data_visualizer
    PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        libtcod::libtcod
		CGAL::CGAL
		cereal::cereal
        scroll
)