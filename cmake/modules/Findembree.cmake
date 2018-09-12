find_path(EMBREE_INCLUDE_DIRS embree3/rtcore.h
  /include
  /usr/include
  /usr/local/include
  /opt/local/include)

find_library(EMBREE_LIBRARY NAMES embree embree3 PATHS
  /lib
  /usr/lib
  /usr/local/lib
  /opt/local/lib)

if (EMBREE_INCLUDE_PATH AND EMBREE_LIBRARY)
  set(EMBREE_FOUND TRUE)
endif()