cmake_minimum_required (VERSION 3.20)

project(scroll)

# Include the header files.
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find all source files in the src directory.
file(
    GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS  # Automatically reconfigure if source files are added/removed.
    ${PROJECT_SOURCE_DIR}/*.cpp
)

# This will create a static library named scroll.
add_library(scroll STATIC ${SOURCE_FILES})

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
)