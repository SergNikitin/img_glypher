cmake_minimum_required(VERSION 2.8)

project(lib_tests)

# Build setup
set(LIB_TESTS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(LIB_TESTS_INCLUDES_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${FREETYPE_SRC}/include)
include_directories(${SDL2_SRC}/include)
include_directories(${SDL2_IMAGE_SRC})

# Setting up already built libraries
add_library(freetype STATIC IMPORTED)
set_property(TARGET freetype PROPERTY IMPORTED_LOCATION ${FREETYPE_BIN}/libfreetype.a)

add_library(sdl2 STATIC IMPORTED)
set_property(TARGET sdl2 PROPERTY IMPORTED_LOCATION ${SDL2_BIN}/libSDL2.a)

add_library(sdl2_image STATIC IMPORTED)
set_property(TARGET sdl2_image PROPERTY IMPORTED_LOCATION ${SDL2_IMAGE_BIN}/.libs/libSDL2_image.a)

# Building test files
add_executable(freetype_test ${LIB_TESTS_SRC_DIR}/freetype_test.cpp)
add_dependencies(freetype_test freetype_ext_project)

target_link_libraries(freetype_test ${FREETYPE_BIN}/libfreetype.a)

add_executable(sdl2_image_test ${LIB_TESTS_SRC_DIR}/sdl2_image_test.c)
add_dependencies(sdl2_image_test    sdl2_ext_project
                                    sdl2_image_ext_project)
target_link_libraries(sdl2_image_test   ${SDL2_BIN}/libSDL2.a
                                        ${SDL2_IMAGE_BIN}/.libs/libSDL2_image.a
                                        pthread
                                        m
                                        dl)
