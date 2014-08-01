cmake_minimum_required(VERSION 2.8)

project(img_glypher_tests)

# Build setup
set(TESTS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TESTS_INCLUDES_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(lib_tests)
