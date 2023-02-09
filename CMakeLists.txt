cmake_minimum_required(VERSION 3.16.3)

project(null0)

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)

# Adding wasm3 for wasm loading
FetchContent_Declare(m3 GIT_REPOSITORY https://github.com/wasm3/wasm3.git GIT_TAG main)
FetchContent_MakeAvailable(m3)

# Adding PhysFS for filesystm/zip abstraction
set(PHYSFS_ARCHIVE_ZIP ON CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_7Z OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_GRP OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_HOG OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_MVL OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_QPAK OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_SLB OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_ISO9660 OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_VDF OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(PHYSFS_DISABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(physfs GIT_REPOSITORY https://github.com/icculus/physfs.git GIT_TAG main)
FetchContent_MakeAvailable(physfs)
include_directories(${physfs_SOURCE_DIR}/src)

# add pntr for graphics
FETCHCONTENT_DECLARE(pntr GIT_REPOSITORY https://github.com/RobLoach/pntr.git GIT_TAG master)
FETCHCONTENT_MAKEAVAILABLE(pntr)

# add soloudd for sound
FETCHCONTENT_DECLARE(soloud GIT_REPOSITORY https://github.com/jarikomppa/soloud.git GIT_TAG master)
FetchContent_MakeAvailable(soloud)
add_subdirectory(${soloud_SOURCE_DIR}/contrib)
include_directories(${soloud_SOURCE_DIR}/include)

# TODO: figure out how to statically link to this
# FETCHCONTENT_DECLARE(openmpt GIT_REPOSITORY https://github.com/OpenMPT/openmpt.git)
# FetchContent_MakeAvailable(openmpt)

file(GLOB
  SOURCES
  CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/null0-*.[cpp|c]
  ${openmpt_SOURCE_DIR}/libopenmpt/*.cpp
)

# SDL for CLI
find_package(SDL2 REQUIRED)
if (SDL2_FOUND)
    add_executable(${PROJECT_NAME} ${SOURCES} src/entry-cli.c ${soloud_SOURCE_DIR}/src/c_api/soloud_c.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main m3 physfs-static pntr soloud)
else()
    message(WARNING "SDL not found, so no CLI")
endif()


ADD_LIBRARY(${PROJECT_NAME}-libretro SHARED ${SOURCES} src/entry-libretro.c ${soloud_SOURCE_DIR}/src/c_api/soloud_c.cpp)
SET_TARGET_PROPERTIES(${PROJECT_NAME}-libretro PROPERTIES PREFIX "")
target_compile_definitions(${PROJECT_NAME}-libretro PUBLIC NULL0_NULL_SOUND)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-libretro PRIVATE m3 physfs-static pntr soloud)
