cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

project(freetype_test)

# Build setup
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(MAIN_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(MAIN_INCLUDES_DIR ${PROJECT_SOURCE_DIR}/include)

set(PRODUCT_DIR ${PROJECT_SOURCE_DIR}/product)
set(LIB_DIR ${PRODUCT_DIR}/lib)

set(FREETYPE_VER "2.5.3")
set(SDL2_VER "2.0.3")
set(SDL2_IMAGE_VER "2.0.0")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PRODUCT_DIR}/bin)



# Freetype library
ExternalProject_Add(freetype_ext_project
    URL http://download.savannah.gnu.org/releases/freetype/freetype-${FREETYPE_VER}.tar.gz
    PREFIX ${LIB_DIR}/freetype
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(freetype_ext_project SOURCE_DIR)
ExternalProject_Get_Property(freetype_ext_project BINARY_DIR)

set(FREETYPE_SRC ${SOURCE_DIR})
set(FREETYPE_BIN ${BINARY_DIR})



# SDL2 library
ExternalProject_Add(sdl2_ext_project
    URL http://www.libsdl.org/release/SDL2-${SDL2_VER}.tar.gz
    PREFIX ${LIB_DIR}/SDL2
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(sdl2_ext_project SOURCE_DIR)
ExternalProject_Get_Property(sdl2_ext_project BINARY_DIR)

set(SDL2_SRC ${SOURCE_DIR})
set(SDL2_BIN ${BINARY_DIR})



# SDL2_image library
set(SDL2_IMAGE_ENV_VARS LDFLAGS=-L${SDL2_BIN}
                        CFLAGS=-I${SDL2_SRC}/include
                        SDL2_CONFIG=${PROJECT_SOURCE_DIR}/build_utils/sdl2-config-modified)

ExternalProject_Add(sdl2_image_ext_project
    URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-${SDL2_IMAGE_VER}.tar.gz
    DEPENDS sdl2_ext_project
    PREFIX ${LIB_DIR}/SDL2_image
    CONFIGURE_COMMAND ${SDL2_IMAGE_ENV_VARS} <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared=no --enable-static=yes
    BUILD_COMMAND make
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(sdl2_image_ext_project SOURCE_DIR)
ExternalProject_Get_Property(sdl2_image_ext_project BINARY_DIR)

set(SDL2_IMAGE_SRC ${SOURCE_DIR})
set(SDL2_IMAGE_BIN ${BINARY_DIR})



# Main application
include_directories(${FREETYPE_SRC}/include)
include_directories(${SDL2_SRC}/include)
include_directories(${SDL2_IMAGE_SRC})
include_directories(${MAIN_INCLUDES_DIR})

file(GLOB SRC_FILES
    "${MAIN_SRC_DIR}/*.cpp"
)

add_executable(img_glypher ${SRC_FILES})
add_dependencies(img_glypher    freetype_ext_project
                                sdl2_ext_project
                                sdl2_image_ext_project)

target_link_libraries(img_glypher   ${FREETYPE_BIN}/libfreetype.a
                                    ${SDL2_BIN}/libSDL2.a
                                    ${SDL2_IMAGE_BIN}/.libs/libSDL2_image.a
                                    )
