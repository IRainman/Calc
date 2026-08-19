cmake_minimum_required(VERSION 3.30)

project(Calc
  VERSION 2.10.3
  LANGUAGES C CXX
)

# ============================================================================
# Linux only
# ============================================================================

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR
    "Calc CMake build supports Linux only. "
    "Detected system: ${CMAKE_SYSTEM_NAME}"
  )
endif()

# ============================================================================
# Supported compilers
#
#   Clang
#   GNU / GCC
# ============================================================================

if(NOT CMAKE_C_COMPILER_ID MATCHES "^(Clang|GNU)$")
  message(FATAL_ERROR
    "Calc CMake build supports Clang and GCC for C. "
    "Detected compiler: ${CMAKE_C_COMPILER_ID}"
  )
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
  message(FATAL_ERROR
    "Calc CMake build supports Clang and GCC for C++. "
    "Detected compiler: ${CMAKE_CXX_COMPILER_ID}"
  )
endif()

# ============================================================================
# Language standards
# ============================================================================

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Configuration information
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "Calc configuration")
message(STATUS "========================================")
message(STATUS "System:")
message(STATUS "  ${CMAKE_SYSTEM_NAME}")

message(STATUS "")
message(STATUS "C compiler:")
message(STATUS "  ${CMAKE_C_COMPILER_ID}")
message(STATUS "  ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  ${CMAKE_C_COMPILER}")

message(STATUS "")
message(STATUS "C++ compiler:")
message(STATUS "  ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  ${CMAKE_CXX_COMPILER}")

message(STATUS "")
message(STATUS "C standard:")
message(STATUS "  C23")

message(STATUS "")
message(STATUS "C++ standard:")
message(STATUS "  C++23")

message(STATUS "")
message(STATUS "Pointer size:")
message(STATUS "  ${CMAKE_SIZEOF_VOID_P}")

message(STATUS "")

# ============================================================================
# Header-only/source dependencies
#
# Calc/pch.hpp expects:
#
#   ../../fast_float/include/fast_float/fast_float.h
#   ../../fmt/include/fmt/compile.h
#   ../../zmij/zmij.cc
#
# Therefore:
#
#   <parent>/
#       Calc/
#       fast_float/
#       fmt/
#       zmij/
# ============================================================================

set(CALC_DEPENDENCY_ROOT
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

set(CALC_FAST_FLOAT_ROOT
  "${CALC_DEPENDENCY_ROOT}/fast_float"
)

set(CALC_FMT_ROOT
  "${CALC_DEPENDENCY_ROOT}/fmt"
)

set(CALC_ZMIJ_ROOT
  "${CALC_DEPENDENCY_ROOT}/zmij"
)

set(CALC_REQUIRED_DEPENDENCY_FILES
  "${CALC_FAST_FLOAT_ROOT}/include/fast_float/fast_float.h"
  "${CALC_FMT_ROOT}/include/fmt/compile.h"
  "${CALC_ZMIJ_ROOT}/zmij.cc"
)

foreach(CALC_DEPENDENCY_FILE IN LISTS CALC_REQUIRED_DEPENDENCY_FILES)
  if(NOT EXISTS "${CALC_DEPENDENCY_FILE}")
    message(FATAL_ERROR
      "Required Calc dependency file is missing:\n"
      "  ${CALC_DEPENDENCY_FILE}\n\n"
      "Expected sibling dependency layout:\n"
      "  ${CALC_DEPENDENCY_ROOT}/Calc\n"
      "  ${CALC_DEPENDENCY_ROOT}/fast_float\n"
      "  ${CALC_DEPENDENCY_ROOT}/fmt\n"
      "  ${CALC_DEPENDENCY_ROOT}/zmij"
    )
  endif()
endforeach()

message(STATUS "Dependencies:")
message(STATUS "  fast_float : ${CALC_FAST_FLOAT_ROOT}")
message(STATUS "  fmt        : ${CALC_FMT_ROOT}")
message(STATUS "  zmij       : ${CALC_ZMIJ_ROOT}")
message(STATUS "")

# ============================================================================
# Sources
# ============================================================================

set(CALC_SOURCES
  Calc/gui.cpp
  Calc/formatter.cpp
  Calc/identifiers.cpp
  Calc/issue_manager.cpp
  Calc/lexer.cpp
  Calc/parser.cpp
  Calc/pch.cpp
  Calc/tests.cpp
)

# ============================================================================
# Target
# ============================================================================

add_executable(Calc
  ${CALC_SOURCES}
)

target_include_directories(Calc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Calc"
  "${CALC_FAST_FLOAT_ROOT}/include"
  "${CALC_FMT_ROOT}/include"
  "${CALC_ZMIJ_ROOT}"
)

# ============================================================================
# Language standards
# ============================================================================

target_compile_features(Calc PRIVATE
  c_std_23
  cxx_std_23
)

set_property(
  TARGET Calc
  PROPERTY C_STANDARD 23
)

set_property(
  TARGET Calc
  PROPERTY C_STANDARD_REQUIRED ON
)

set_property(
  TARGET Calc
  PROPERTY C_EXTENSIONS OFF
)

set_property(
  TARGET Calc
  PROPERTY CXX_STANDARD 23
)

set_property(
  TARGET Calc
  PROPERTY CXX_STANDARD_REQUIRED ON
)

set_property(
  TARGET Calc
  PROPERTY CXX_EXTENSIONS OFF
)

# ============================================================================
# Output
# ============================================================================

set_target_properties(Calc PROPERTIES
  OUTPUT_NAME
    "Calc"

  ARCHIVE_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/lib"

  LIBRARY_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/lib"

  RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/$<CONFIG>"
)

# ============================================================================
# Precompiled header
# ============================================================================

target_precompile_headers(Calc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Calc/pch.hpp"
)

# ============================================================================
# Definitions
# ============================================================================

target_compile_definitions(Calc PRIVATE
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)

# ============================================================================
# Optimization / warnings
# ============================================================================

target_compile_options(Calc PRIVATE

  # Debug
  $<$<CONFIG:Debug>:-O0>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-Wall>
  $<$<CONFIG:Debug>:-Wextra>

  # Release
  $<$<CONFIG:Release>:-O3>
  $<$<CONFIG:Release>:-flto>
  $<$<CONFIG:Release>:-Wall>
)

# ============================================================================
# Architecture
#
# 64-bit:
#   x86-64-v2
#
# 32-bit:
#   Prescott / SSE3
# ============================================================================

if(CMAKE_SIZEOF_VOID_P EQUAL 8)

  message(STATUS
    "Target architecture: x86-64-v2"
  )

  target_compile_options(Calc PRIVATE
    -march=x86-64-v2
  )

elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)

  message(STATUS
    "Target architecture: x86 / Prescott / SSE3"
  )

  target_compile_options(Calc PRIVATE
    -march=prescott
  )

else()

  message(FATAL_ERROR
    "Unsupported target pointer size: ${CMAKE_SIZEOF_VOID_P}"
  )

endif()

# ============================================================================
# LTO
#
# Do not run CheckIPOSupported().
# Release builds simply use -flto.
# ============================================================================

target_link_options(Calc PRIVATE
  $<$<CONFIG:Release>:-flto>
)

# ============================================================================
# Installation
# ============================================================================

install(
  TARGETS Calc
  RUNTIME DESTINATION bin
)
