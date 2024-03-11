cmake_minimum_required(VERSION 3.27)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
include(VersionHelper)
project(integrate
    VERSION ${VERSION_FULL})

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Threads REQUIRED)
target_link_libraries(${BIN_TARGET} Threads::Threads)

find_package(OpenMP REQUIRED)
target_link_libraries(${BIN_TARGET} OpenMP::OpenMP_CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(include)
configure_file(include/version.h.in ${PROJECT_BINARY_DIR}/version.h)
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(src)
