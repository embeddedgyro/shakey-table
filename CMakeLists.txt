cmake_minimum_required(VERSION 3.10)
# Project name and version
project(ShakeyTable VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include subdirectories
add_subdirectory(lib)
add_subdirectory(src)

# Generate Doxyfile and associated target
find_package(Doxygen)
set(DOXYGEN_GENERATE_LATEX YES)
doxygen_add_docs(
    doc
    ${PROJECT_SOURCE_DIR}
)
