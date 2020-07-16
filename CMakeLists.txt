# General project settings.
cmake_minimum_required(VERSION 3.12.4)
project(ionlang VERSION 1.0.0 LANGUAGES CXX)

# Setup compiler flag(s) & standard(s).
set(CMAKE_CXX_STANDARD 20)

# General project information.
set(PROJECT_URL "https://github.com/ionlang/ionlang")

# Setup source and build directories.
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Scan dependencies.
find_package(ionshared REQUIRED)
find_package(ionir REQUIRED)

# Setup default build flags.
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT")

# Option to use MD.
option(USE_MD "Use MD instead of MTd" OFF)

if (USE_MD)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    message(STATUS "Using /MD flag")
endif (USE_MD)

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

# Configure and use LLVM. Also ensures LLVM 9.0.0 is installed.
find_package(LLVM 9.0.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM source directories (for lexical support, etc.). The SYSTEM symbol supposedly marks the directory to suppress warning(s).
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Disable warnings from includes.
#add_library(llvm INTERFACE)
#target_sources(llvm INTERFACE)
#target_include_directories(llvm PRIVATE include/ SYSTEM INTERFACE include/)

# Set source file(s).
file(
    GLOB_RECURSE SOURCES
    "include/${PROJECT_NAME}/*.h"
    "src/*.cpp"
    "src/*.h"
)

# Specify that this project is a library.
add_library("${PROJECT_NAME}" ${SOURCES})

# Link .libs (same functionality handled by llvm_map_components_to_libnames().)
#target_link_libraries(${PROJECT_NAME} ${LIB_SOURCES})

# Provide include directories to be used in the build command. Position in file matters.
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

# Find the libraries that correspond to the LLVM components
# that we wish to use.
llvm_map_components_to_libnames(llvm_libs all)

# Link against various libraries including LLVM, libionshared & libionir.
target_link_libraries("${PROJECT_NAME}" LLVM ionshared::ionshared ionir::ionir)

# TODO: Adding subdirs either way.
add_subdirectory(libs/googletest)
add_subdirectory(test)

# Setup unit testing using Google Test (GTest) if applicable. This binds the CMakeLists.txt on the test project.
option(BUILD_TESTS "Build tests" ON)

if (BUILD_TESTS)
    #    add_subdirectory(libs/googletest)
    #    add_subdirectory(test)
endif ()

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
    "install_${PROJECT_NAME}"
    "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
    DEPENDS "${PROJECT_NAME}"
    COMMENT "Installing ${PROJECT_NAME}"
)
