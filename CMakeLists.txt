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

# Linux installation support.
include(GNUInstallDirs)

# Dependencies: ionshared.
find_package(ionshared QUIET)

# Download & build ionshared if it's not installed on
# the system, via CMake's FetchContent.
if(NOT ionshared_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        ionshared

        # TODO: Using dev. branch temporarily.
        URL https://github.com/ionlang/ionshared/archive/dev.tar.gz
    )

    # Has ionshared been downloaded yet?
    if(NOT ionshared_POPULATED)
        set(FETCHCONTENT_QUIET OFF)
        FetchContent_Populate(ionshared)

        # Do not build tests for it.
        set(IONSHARED_BUILD_TESTS OFF)

        # Build ionshared.
        add_subdirectory(${ionshared_SOURCE_DIR} ${ionshared_BINARY_DIR})
    endif()
endif()

# Dependencies: ionir.
find_package(ionir QUIET)

# Download & build ionshared if it's not installed on
# the system, via CMake's FetchContent.
if(NOT ionir_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        ionir

        # TODO: Using dev. branch temporarily.
        URL https://github.com/ionlang/ionir/archive/dev.tar.gz
    )

    # Has ionir been downloaded yet?
    if(NOT ionir_POPULATED)
        set(FETCHCONTENT_QUIET OFF)
        FetchContent_Populate(ionir)

        # Do not build tests for it.
        set(IONIR_BUILD_TESTS OFF)

        # Build ionir.
        add_subdirectory(${ionir_SOURCE_DIR} ${ionir_BINARY_DIR})
    endif()
endif()

# Configure and use LLVM. Also ensures LLVM 10.0.0 is installed.
find_package(LLVM 10.0.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM source directories (for lexical support, etc.). The SYSTEM symbol supposedly marks the directory to suppress warning(s).
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

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

option(IONLANG_BUILD_STATIC_LIB "Build static library" ON)

if (IONLANG_BUILD_STATIC_LIB)
    set(IONLANG_OPTION_LIB_TYPE STATIC)
else()
    set(IONLANG_OPTION_LIB_TYPE SHARED)
endif()

# Setup versioning.
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION "$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)")

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
add_library("${PROJECT_NAME}" ${IONLANG_OPTION_LIB_TYPE} ${SOURCES})

# Link .libs (same functionality handled by llvm_map_components_to_libnames().)
#target_link_libraries(${PROJECT_NAME} ${LIB_SOURCES})

# Provide include directories to be used in the build command. Position in file matters.
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

    # Comment out as it's causing cannot find included file '/project_name'.
    #$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

# Determine whether to link against LLVM static libraries
# or dynamic library (libLLVM.dll).
if(BUILD_SHARED_LIBS)
    # Find the libraries that correspond to the LLVM components
    # that we wish to use.
    llvm_map_components_to_libnames(llvm_libs all)
else()
    set(llvm_libs LLVM)
endif()

# Link against various libraries including LLVM, libionshared & libionir.
target_link_libraries("${PROJECT_NAME}" PUBLIC ${llvm_libs} ionshared ionir)

# Setup unit testing using Google Test (GTest) if applicable. This binds the CMakeLists.txt on the test project.
option(IONLANG_BUILD_TESTS "Build tests" OFF)

if(IONLANG_BUILD_TESTS)
    add_subdirectory(./libs/googletest)
    add_subdirectory(./test)
endif()

# Setup install target.
install(
    TARGETS "${PROJECT_NAME}"
    EXPORT ${PROJECT_NAME}-config LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR} # Destination is relative to ${CMAKE_INSTALL_PREFIX}.
)

install(
    EXPORT ${PROJECT_NAME}-config
#    NAMESPACE ${PROJECT_NAME}::
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
