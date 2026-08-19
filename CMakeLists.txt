cmake_minimum_required(VERSION 3.30)

project(Calc
  VERSION 2.10.2
  LANGUAGES CXX
)

# ============================================================================
# Compiler
#
# The CMake build is intentionally Clang-only.
#
# Linux:
#   clang++ + lld
#
# Windows:
#   clang++ targeting Windows/MSVC ABI + lld-link
#
# The Visual Studio/MSVC workflow is separate and uses Calc.sln.
# ============================================================================

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR
    "Calc CMake build requires Clang. "
    "Detected compiler: ${CMAKE_CXX_COMPILER_ID}"
  )
endif()

message(STATUS "Calc compiler:")
message(STATUS "  ID      : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  version : ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  CXX     : ${CMAKE_CXX_COMPILER}")
message(STATUS "  target  : ${CMAKE_CXX_COMPILER_TARGET}")

# ============================================================================
# Header-only/source dependencies
#
# Calc/pch.hpp intentionally uses these relative paths:
#
#   ../../zmij/zmij.cc
#   ../../fmt/include/fmt/compile.h
#   ../../fast_float/include/fast_float/fast_float.h
#
# Therefore the repositories must be siblings of the Calc checkout.
#
#   <parent>/
#       Calc/
#       fast_float/
#       fmt/
#       zmij/
#
# The CI workflow performs these checkouts explicitly.
# ============================================================================

set(CALC_DEPENDENCY_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(CALC_ZMIJ_ROOT
    "${CALC_DEPENDENCY_ROOT}/zmij")

set(CALC_FMT_ROOT
    "${CALC_DEPENDENCY_ROOT}/fmt")

set(CALC_FAST_FLOAT_ROOT
    "${CALC_DEPENDENCY_ROOT}/fast_float")

set(CALC_REQUIRED_DEPENDENCY_FILES
    "${CALC_ZMIJ_ROOT}/zmij.cc"
    "${CALC_FMT_ROOT}/include/fmt/compile.h"
    "${CALC_FAST_FLOAT_ROOT}/include/fast_float/fast_float.h"
)

foreach(CALC_DEPENDENCY_FILE IN LISTS CALC_REQUIRED_DEPENDENCY_FILES)

  if(NOT EXISTS "${CALC_DEPENDENCY_FILE}")
    message(FATAL_ERROR
      "Calc dependency is missing:\n"
      "  ${CALC_DEPENDENCY_FILE}\n\n"
      "Expected sibling dependency layout:\n"
      "  ${CALC_DEPENDENCY_ROOT}/Calc\n"
      "  ${CALC_DEPENDENCY_ROOT}/fast_float\n"
      "  ${CALC_DEPENDENCY_ROOT}/fmt\n"
      "  ${CALC_DEPENDENCY_ROOT}/zmij"
    )
  endif()

endforeach()

message(STATUS "Calc dependencies:")
message(STATUS "  zmij       : ${CALC_ZMIJ_ROOT}")
message(STATUS "  fmt        : ${CALC_FMT_ROOT}")
message(STATUS "  fast_float : ${CALC_FAST_FLOAT_ROOT}")

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

# Windows resource file exists only for the Windows target.
if(WIN32)

  list(APPEND CALC_SOURCES
    Calc/Calc.rc
  )

endif()

# ============================================================================
# Target
# ============================================================================

if(WIN32)

  add_executable(Calc WIN32
    ${CALC_SOURCES}
  )

else()

  add_executable(Calc
    ${CALC_SOURCES}
  )

endif()

# ============================================================================
# Includes
# ============================================================================

target_include_directories(Calc PRIVATE

  "${CMAKE_CURRENT_SOURCE_DIR}/Calc"

  "${CALC_ZMIJ_ROOT}"

  "${CALC_FMT_ROOT}/include"

  "${CALC_FAST_FLOAT_ROOT}/include"

)

# ============================================================================
# Language
# ============================================================================

target_compile_features(Calc PRIVATE
  cxx_std_23
)

set_target_properties(Calc PROPERTIES

  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF

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
#
# pch.hpp directly includes:
#
#   ../../zmij/zmij.cc
#   ../../fmt/include/fmt/compile.h
#   ../../fast_float/include/fast_float/fast_float.h
#
# The sibling dependency layout above therefore must exist before this target
# is compiled.
# ============================================================================

target_precompile_headers(Calc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Calc/pch.hpp"
)

# ============================================================================
# Definitions
# ============================================================================

if(WIN32)

  target_compile_definitions(Calc PRIVATE
    WIN32
    _WINDOWS
  )

endif()

target_compile_definitions(Calc PRIVATE

  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>

)

# ============================================================================
# Architecture
#
# 64-bit:
#
#   x86-64-v2
#
# Includes the SSE4.2 generation and other x86-64-v2 requirements.
# No AVX is required.
#
# 32-bit:
#
#   Prescott
#
# LLVM's "prescott" target corresponds to:
#
#   MMX
#   SSE
#   SSE2
#   SSE3
#
# and does not enable SSSE3/SSE4.x/AVX.
# ============================================================================

if(CMAKE_SIZEOF_VOID_P EQUAL 8)

  message(STATUS
    "Calc architecture: x86-64-v2"
  )

  target_compile_options(Calc PRIVATE
    -march=x86-64-v2
  )

elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)

  message(STATUS
    "Calc architecture: 32-bit x86 / Prescott / SSE3"
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
# Optimization
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
# Link-time optimization
# ============================================================================

target_link_options(Calc PRIVATE

  $<$<CONFIG:Release>:-flto>

)

# ============================================================================
# Windows
#
# clang++ targets the Windows MSVC ABI:
#
#   x86_64-pc-windows-msvc
#   i686-pc-windows-msvc
#
# The compiler is still Clang.
# The linker is LLVM lld-link.
# ============================================================================

if(WIN32)

  if(NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")

    message(FATAL_ERROR
      "Calc Windows CMake build expects clang++ GNU-style frontend. "
      "Detected frontend: "
      "${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}"
    )

  endif()

endif()

# ============================================================================
# Installation
# ============================================================================

install(
  TARGETS Calc
  RUNTIME DESTINATION bin
)
