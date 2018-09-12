cmake_minimum_required(VERSION 3.10)
project(stray)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(embree 3.2 REQUIRED)
find_package(OpenEXR REQUIRED)

include_directories(${EMBREE_INCLUDE_DIRS})
include_directories(${ILMBASE_INCLUDE_DIR})
include_directories(${OPENEXR_INCLUDE_DIR})


add_executable(stray src/stray/Stray.cpp)

target_link_libraries(stray
    ${EMBREE_LIBRARY}
    ${ILMBASE_LIBRARY}
    ${OPENEXR_LIBRARY}
)
