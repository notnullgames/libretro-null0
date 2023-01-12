CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(null0_libretro)

SET(SOURCE_FILES libretro.c libretro.h)
ADD_LIBRARY(null0_libretro SHARED ${SOURCE_FILES})
set_target_properties(null0_libretro PROPERTIES PREFIX "")
