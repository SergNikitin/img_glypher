cmake_minimum_required(VERSION 2.8)

project(img_glypher_tests)

# Build setup
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(TESTS_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TESTS_INCLUDES_DIR ${PROJECT_SOURCE_DIR}/include)
# set(LIB_TESTS_SRC_DIR ${PROJECT_SOURCE_DIR}/lib_tests)

# set(PRODUCT_DIR ${PROJECT_SOURCE_DIR}/product)
# set(LIB_DIR ${PRODUCT_DIR}/lib)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PRODUCT_DIR}/bin)

add_subdirectory(lib_tests)
