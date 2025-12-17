cmake_minimum_required(VERSION 3.30)
project(basedlib VERSION 0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES FALSE)

add_subdirectory(lib)
add_subdirectory(test)
