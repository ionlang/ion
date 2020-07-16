# General project settings.
cmake_minimum_required(VERSION 3.12.4)
project(ionlang VERSION 1.0.0 LANGUAGES CXX)

# Setup compiler flag(s) & standard(s).
set(CMAKE_CXX_STANDARD 17)

# General project information.
set(PROJECT_URL "https://github.com/ionlang/ionlang")

# Setup source and build directories.
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Scan dependencies.
find_package(ionshared REQUIRED)
find_package(ionir REQUIRED)
find_package(CLI11 CONFIG)

# Options.
option(USE_CLANG "Use Clang and Clang++ as compilers")

if (USE_CLANG)
    # TODO
    set(T "")
endif (USE_CLANG)

# Setup versioning.
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION "$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)")

# Set source file(s).
file(
    GLOB_RECURSE SOURCES
    "include/${PROJECT_NAME}/*.h"
    "src/*.cpp"
    "src/*.h"
)

# Setup LLVM include directories.
find_package(LLVM 9.0.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Specify that this project is a library.
add_library("${PROJECT_NAME}" ${SOURCES})

llvm_map_components_to_libnames(llvm_libs all)

# Link against various libraries such as LLVM, libionir, and libionshared.
target_link_libraries("${PROJECT_NAME}" PRIVATE LLVM ionir::ionir ionshared::ionshared)

# Provide include directories to be used in the build command. Position in file matters.
target_include_directories("${PROJECT_NAME}" PUBLIC "src" "include" "libs")

# Setup unit testing using Google Test (GTest) if applicable. This binds the CMakeLists.txt on the test project.
option(BUILD_TESTS "Build tests" OFF)

# TODO: Make an option for building tests.
add_subdirectory(libs/googletest)
add_subdirectory(test)

# Setup install target.
install(
    TARGETS "${PROJECT_NAME}"
    EXPORT ${PROJECT_NAME}-config LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR} # Destination is relative to ${CMAKE_INSTALL_PREFIX}.
)

install(
    EXPORT ${PROJECT_NAME}-config
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    DIRECTORY include/${PROJECT_NAME}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.h" PATTERN "LICENSE" # Match only header files & LICENSE file.
)

# Support for CLion install target.
add_custom_target(
    install_${PROJECT_NAME}
    "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
    DEPENDS ${PROJECT_NAME}
    COMMENT "Installing ${PROJECT_NAME}"
)
