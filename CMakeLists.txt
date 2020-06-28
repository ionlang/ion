# General project settings.
cmake_minimum_required(VERSION 3.12.4)
project(ionlang VERSION 1.0.0 LANGUAGES CXX)

# Setup compiler flag(s) & standard(s).
set(CMAKE_CXX_STANDARD 17)

# General project information.
set(PROJECT_URL "https://github.com/ionlang/ionlang")

# Setup source and build directories.
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Scan dependencies.
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
    "src/*.h"
    "src/*.cpp"
)

# Setup LLVM include directories.
find_package(LLVM 9.0.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Specify that this project is a library.
add_executable(${PROJECT_NAME} ${SOURCES})

llvm_map_components_to_libnames(llvm_libs all)

# Link against LLVM libraries.
target_link_libraries("${PROJECT_NAME}" PRIVATE LLVM)

# Link against libionir.
target_link_libraries("${PROJECT_NAME}" PRIVATE ionir::ionir)

# Provide include directories to be used in the build command. Position in file matters.
target_include_directories(${PROJECT_NAME} PUBLIC "src" "include" "libs")

# Setup unit testing using Google Test (GTest) if applicable. This binds the CMakeLists.txt on the test project.
option(BUILD_TESTS "Build tests" OFF)

# TODO
if (BUILD_TESTS)
    add_subdirectory(libs/googletest)
    # add_subdirectory(test)
endif ()

# Add install target.
install(
    TARGETS ${LIBRARY_TARGET_NAME}
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
)

# Support for CLion install target.
add_custom_target(
    install_${PROJECT_NAME}
    "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
    DEPENDS ${PROJECT_NAME}
    COMMENT "Installing ${PROJECT_NAME}"
)
