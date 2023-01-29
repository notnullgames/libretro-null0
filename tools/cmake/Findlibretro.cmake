include(FetchContent)

FETCHCONTENT_DECLARE(libretro GIT_REPOSITORY https://github.com/libretro/libretro-common.git GIT_TAG master)
FETCHCONTENT_MAKEAVAILABLE(libretro)
INCLUDE_DIRECTORIES(PUBLIC ${libretro_SOURCE_DIR}/include)