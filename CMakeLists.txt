cmake_minimum_required(VERSION 3.16.3)

project(null0-libretro)

set (CMAKE_CXX_STANDARD 11)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/vendor/soloud/contrib/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/soloud/contrib")
include_directories("${PROJECT_SOURCE_DIR}/vendor/soloud/include/")

# Add PhysFS for filesystm/zip abstraction
SET(PHYSFS_ARCHIVE_ZIP ON CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_7Z OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_GRP OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_HOG OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_MVL OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_QPAK OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_SLB OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_ISO9660 OFF CACHE BOOL "" FORCE)
SET(PHYSFS_ARCHIVE_VDF OFF CACHE BOOL "" FORCE)
SET(PHYSFS_BUILD_STATIC ON CACHE BOOL "" FORCE)
SET(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
SET(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
SET(PHYSFS_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(PHYSFS_DISABLE_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/physfs")
include_directories("${PROJECT_SOURCE_DIR}/vendor/physfs/src")

add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/pntr")
include_directories("${PROJECT_SOURCE_DIR}/vendor/pntr")

add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/wasm3/source")
include_directories("${PROJECT_SOURCE_DIR}/vendor/wasm3/source")

include_directories("${PROJECT_SOURCE_DIR}/src")
FILE(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/rl_config.c src/rl_sound.cpp src/null0.c src/${PROJECT_NAME}.c)
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE physfs-static soloud pntr m3)
