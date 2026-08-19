cmake_minimum_required(VERSION 3.30)

project(Calc
  VERSION 2.10.2
  LANGUAGES CXX
)

# ============================================================================
# Calc CMake build is Clang-only.
#
# Supported:
#   Linux   : clang++ + lld
#   Windows : clang-cl + lld-link
#
# MSVC and GCC are intentionally rejected.
# ============================================================================

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR
    "Calc CMake build requires Clang. "
    "Detected compiler: ${CMAKE_CXX_COMPILER_ID}"
  )
endif()

message(STATUS "Calc compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Calc compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Calc compiler: ${CMAKE_CXX_COMPILER}")

# ============================================================================
# Dependencies
#
# These are Calc's forks and are consumed as header/source-only dependencies.
# They are not linked as separate libraries.
# ============================================================================

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME fmt
  GIT_TAG main
  GITHUB_REPOSITORY irainman/fmt
)

CPMAddPackage(
  NAME zmij
  GIT_TAG main
  GITHUB_REPOSITORY irainman/zmij
)

CPMAddPackage(
  NAME fast_float
  GIT_TAG main
  GITHUB_REPOSITORY irainman/fast_float
)

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
  Calc/Calc.rc
)

# ============================================================================
# Target
# ============================================================================

add_executable(Calc WIN32
  ${CALC_SOURCES}
)

target_include_directories(Calc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Calc"
)

# CPM dependency source trees.
target_include_directories(Calc PRIVATE
  "${CMAKE_BINARY_DIR}/_deps/zmij-src"
  "${CMAKE_BINARY_DIR}/_deps/fast-float-src"
  "${CMAKE_BINARY_DIR}/_deps/fmt-src"
)

# ============================================================================
# C++ standard
# ============================================================================

target_compile_features(Calc PRIVATE
  cxx_std_23
)

set_target_properties(Calc PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF

  ARCHIVE_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/lib"

  LIBRARY_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/lib"

  RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/$<CONFIG>"

  OUTPUT_NAME
    "Calc"
)

# PCH contains Calc's direct header-only/source dependencies:
#
#   ../../zmij/zmij.cc
#   ../../fmt/include/fmt/compile.h
#   ../../fast_float/include/fast_float/fast_float.h
#
# In the CMake build these paths are supplied by the CPM dependency
# include/source tree.
target_precompile_headers(Calc PRIVATE
  "Calc/pch.hpp"
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
#   This includes:
#     SSE
#     SSE2
#     SSE3
#     SSSE3
#     SSE4.1
#     SSE4.2
#     POPCNT
#     SAHF
#     CRC32
#     CMPXCHG16B
#
#   It does NOT require AVX.
#
# 32-bit:
#
#   Prescott / Pentium 4 SSE3
#
#     MMX
#     SSE
#     SSE2
#     SSE3
#
#   No SSSE3, SSE4.1, SSE4.2, AVX, AVX2, FMA, etc.
#
# LLVM's "prescott" target is the normal -march target corresponding to
# FeaturesPrescott.
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
# LLVM LTO
# ============================================================================

include(CheckIPOSupported)

check_ipo_supported(
  RESULT CALC_IPO_SUPPORTED
  OUTPUT CALC_IPO_ERROR
)

if(CALC_IPO_SUPPORTED)

  message(STATUS
    "Calc: LLVM LTO/IPO supported"
  )

  set_property(
    TARGET Calc
    PROPERTY
      INTERPROCEDURAL_OPTIMIZATION
      $<$<CONFIG:Release>:TRUE>
  )

else()

  message(WARNING
    "Calc: LLVM LTO/IPO is not supported: ${CALC_IPO_ERROR}"
  )

endif()

# ============================================================================
# Windows
#
# clang-cl is still the compiler; this configuration must not fall back to
# MSVC's cl.exe.
#
# CMake will use the Windows SDK supplied by the environment and llvm-rc
# / lld-link provided by the Clang workflow.
# ============================================================================

if(WIN32)

  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")

    message(STATUS
      "Calc Windows compiler frontend: clang-cl"
    )

    # clang-cl accepts Clang-specific options through /clang:.
    #
    # Keep architecture and optimization options produced above available
    # to the Clang frontend.
    target_compile_options(Calc PRIVATE
      /clang:-Wno-microsoft
    )

  else()

    message(STATUS
      "Calc Windows compiler frontend: clang"
    )

  endif()

endif()

# ============================================================================
# Installation
# ============================================================================

install(
  TARGETS Calc
  RUNTIME DESTINATION bin
)cmake_minimum_required(VERSION 3.30)


include(cmake/CPM.cmake)

CPMAddPackage(
  NAME fmt
  GIT_TAG main
  GITHUB_REPOSITORY irainman/fmt
)

CPMAddPackage(
  NAME zmij
  GIT_TAG main
  GITHUB_REPOSITORY irainman/zmij
)

CPMAddPackage(
  NAME fast_float
  GIT_TAG main
  GITHUB_REPOSITORY irainman/fast_float
)

set(CALC_SOURCES
  Calc/gui.cpp
  Calc/formatter.cpp
  Calc/identifiers.cpp
  Calc/issue_manager.cpp
  Calc/lexer.cpp
  Calc/parser.cpp
  Calc/pch.cpp
  Calc/tests.cpp
    Calc/Calc.rc
  )
endif()

target_compile_definitions(Calc PRIVATE
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)

  )
  target_compile_options(Calc PRIVATE
  )
  target_link_options(Calc PRIVATE
  )
  target_compile_options(Calc PRIVATE
  )

endif()

)

