cmake_minimum_required(VERSION 3.15)

project(kwargscpp LANGUAGES CXX)

# Read the version.txt file
file(READ "${CMAKE_SOURCE_DIR}/version" VERSION_CONTENTS)

# Extract the major, minor, and patch versions
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" VERSION_MATCHES "${VERSION_CONTENTS}")
set(PROJECT_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(PROJECT_VERSION_MINOR "${CMAKE_MATCH_2}")
set(PROJECT_VERSION_PATCH "${CMAKE_MATCH_3}")

# Print the version numbers
message(STATUS "Project version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Set the project version
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Enable fPIC for pybind11

# Use ccache if available
find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
  message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

if(SKBUILD)
  message(STATUS "SKBUILD Detected. Installing to ${SKBUILD_PLATLIB_DIR}/${SKBUILD_PROJECT_NAME}")
  message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
  message(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")
  message(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
  message(STATUS "CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")

  set(CMAKE_INSTALL_PYTHONPACKAGE_PREFIX "${SKBUILD_PROJECT_NAME}")

  set(CMAKE_INSTALL_INCLUDEDIR "${SKBUILD_PROJECT_NAME}/include")
  set(CMAKE_INSTALL_LIBDIR "${SKBUILD_PROJECT_NAME}/lib")
  set(CMAKE_INSTALL_BINDIR "${SKBUILD_PROJECT_NAME}/bin")
else()
  set(CMAKE_INSTALL_PYTHONPACKAGE_PREFIX ${CMAKE_INSTALL_PREFIX}/site-packages/${CMAKE_PROJECT_NAME})
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
    "Release"
    CACHE STRING "Default build type: Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLE "Build example" OFF)
option(BUILD_PYTHON "Build Python bindings" OFF)
option(ADD_CONDA_TO_RPATH "Add conda to rpath" OFF)

if(DEFINED ENV{CONDA_PREFIX})
  set(CONDA_PATH $ENV{CONDA_PREFIX})
  message(STATUS "ADD_CONDA_TO_RPATH: ${ADD_CONDA_TO_RPATH}")
  message(NOTICE "[CONDA] Environment detected: ${CONDA_PATH}")

  # If CMAKE_INSTALL_PREFIX is not set, set it to CONDA_PATH
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CONDA_PATH})
    message(NOTICE "[CONDA] Setting CMAKE_INSTALL_PREFIX to ${CONDA_PATH}")
  elseif(ADD_CONDA_TO_RPATH)
    # Add conda to rpath
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CONDA_PATH}/lib")
    message(NOTICE "[CONDA] Setting CMAKE_INSTALL_RPATH to ${CMAKE_INSTALL_RPATH}")
  endif()

  set(CMAKE_PREFIX_PATH
    "${CONDA_PATH};${CONDA_PATH}/lib/cmake;${CMAKE_PREFIX_PATH}")
  include_directories(${CONDA_PATH}/include)
  message(NOTICE "[CONDA] Setting CMAKE_PREFIX_PATH to ${CMAKE_PREFIX_PATH}")
endif()

# Define the library, only with include directories
add_library(kwargscpp INTERFACE)
target_include_directories(kwargscpp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if(BUILD_PYTHON)
  message(STATUS "Building Python bindings")

  if(CMAKE_VERSION VERSION_LESS 3.18)
    set(DEV_MODULE Development)
  else()
    set(DEV_MODULE Development.Module)
  endif()

  find_package(Python 3.8 COMPONENTS Interpreter ${DEV_MODULE} ${SKBUILD_SABI_COMPONENT} REQUIRED)

  # Detect the installed nanobind package and import it into CMake
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)

  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE pybind11_ROOT)

  message(STATUS "nanobind_ROOT: ${nanobind_ROOT}")
  message(STATUS "pybind11_ROOT: ${pybind11_ROOT}")

  find_package(nanobind CONFIG)
  find_package(pybind11 CONFIG)

  if(nanobind_FOUND)
    message(STATUS "nanobind Found")
  else()
    message(WARNING "nanobind not Found")
  endif()

  if(pybind11_FOUND)
    message(STATUS "pybind11 Found")
  else()
    message(WARNING "pybind11 not Found")
  endif()
endif()

if(BUILD_TESTS)
  message(STATUS "Building tests")
  enable_testing()
  find_package(doctest REQUIRED)

  add_subdirectory(tests)
endif()

# Install the library target
install(TARGETS kwargscpp
  EXPORT ${CMAKE_PROJECT_NAME}Targets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# install headers to include directory, keeping the same directory structure
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Generate and install the package configuration files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  cmake/${CMAKE_PROJECT_NAME}Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
  PATH_VARS CMAKE_INSTALL_LIBDIR
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
)

# Install the export set
install(
  EXPORT ${CMAKE_PROJECT_NAME}Targets
  FILE ${CMAKE_PROJECT_NAME}Targets.cmake
  NAMESPACE "${CMAKE_PROJECT_NAME}::"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
