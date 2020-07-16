cmake_minimum_required(VERSION 3.12.4)

enable_testing()

#find_package(GTest REQUIRED
#    PATHS "../libs/googletest/googletest/generated"
#)

project(ionlang_tests)

file(
    GLOB_RECURSE SOURCES
    "*.h"
    "*.cpp"
)

include_directories("${gtest_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}")

add_executable("${PROJECT_NAME}" ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    gtest
    ionlang
)

# Specify additional include directories to look for imports.
target_include_directories("${PROJECT_NAME}" PRIVATE "../src")

# Prepare a macro for adding tests.
# macro(package_add_test TESTNAME)
#     add_executable(${TESTNAME} ${ARGN})
#     target_link_libraries(${TESTNAME} gtest gmock gtest_main)
#     add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
#     set_target_properties(${TESTNAME} PROPERTIES FOLDER test)
# endmacro()

# # Add tests.
# package_add_test(StreamTest stream.cpp)

#set(GTEST_ROOT "c:/path/to/gtest/root" CACHE PATH "path to gtest") (Helps find gtest through find_package)
