cmake_minimum_required(VERSION 3.10)
project(stray)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

# Find dependencies
find_package(embree 3.2 REQUIRED)
find_package(OpenEXR REQUIRED)

include_directories(
    src/stray
    ${EMBREE_INCLUDE_DIRS}
    ${ILMBASE_INCLUDE_DIR}/OpenEXR
    ${OPENEXR_INCLUDE_DIR}/OpenEXR
)

# Compile ISPC code
add_custom_command(
    OUTPUT
    Kernels.o

    COMMAND
    ispc --target=sse4 ${CMAKE_SOURCE_DIR}/src/stray/Kernels.ispc -o Kernels.o

    DEPENDS
    src/stray/Kernels.ispc
)

set(
    ISPC_OBJECTS
    Kernels.o
)

add_executable(
    stray

    src/stray/Stray.cpp

    ${ISPC_OBJECTS}
)

target_link_libraries(
    stray

    ${EMBREE_LIBRARY}
    ${ILMBASE_LIBRARY}
    ${OPENEXR_LIBRARY}
)
