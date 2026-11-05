cmake_minimum_required(VERSION 4.3.0)

project(Calc
  VERSION 2026.11.06.3
  DESCRIPTION "Fast and power efficient scientific calculator"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)

# Calc intentionally exposes only Debug and Release configurations.
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
    "Supported Calc configurations" FORCE)
endif()

# -----------------------------------------------------------------------------
# Supported platforms and compilers
# -----------------------------------------------------------------------------

if(WIN32)
  # The Windows build must use cl.exe/link.exe. Do not silently accept clang-cl,
  # MinGW, or another Windows compiler.
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR
      "The Windows Calc build requires Microsoft MSVC (cl.exe/link.exe).\n"
      "Detected compiler ID: ${CMAKE_CXX_COMPILER_ID}\n"
      "Compiler: ${CMAKE_CXX_COMPILER}")
  endif()

  enable_language(RC)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
    message(FATAL_ERROR
      "The Linux Calc build supports GCC and Clang.\n"
      "Detected compiler ID: ${CMAKE_CXX_COMPILER_ID}\n"
      "Compiler: ${CMAKE_CXX_COMPILER}")
  endif()

  if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES
      "^(x86_64|amd64|AMD64|i[3-6]86|x86)$")
    message(FATAL_ERROR
      "The Linux Calc build supports x86 and x86-64 only.\n"
      "Detected processor: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()

else()
  message(FATAL_ERROR
    "Calc supports Windows (MSVC) and Linux (GCC/Clang).\n"
    "Detected system: ${CMAKE_SYSTEM_NAME}")
endif()

# -----------------------------------------------------------------------------
# Local dependency layout
#
# pch.hpp includes these dependencies using repository-relative paths:
#   ../../fast_float/include/fast_float/fast_float.h
#   ../../fmt/include/fmt/compile.h
#   ../../zmij/zmij.cc
# Therefore the dependency repositories must be siblings of this repository.
# -----------------------------------------------------------------------------

set(CALC_DEPENDENCY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(CALC_FAST_FLOAT_ROOT "${CALC_DEPENDENCY_ROOT}/fast_float")
set(CALC_FMT_ROOT "${CALC_DEPENDENCY_ROOT}/fmt")
set(CALC_ZMIJ_ROOT "${CALC_DEPENDENCY_ROOT}/zmij")

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
      "Expected sibling directory layout:\n"
      "  ${CMAKE_CURRENT_SOURCE_DIR}\n"
      "  ${CALC_FAST_FLOAT_ROOT}\n"
      "  ${CALC_FMT_ROOT}\n"
      "  ${CALC_ZMIJ_ROOT}")
  endif()
endforeach()

message(STATUS "Calc dependencies:")
message(STATUS "  fast_float: ${CALC_FAST_FLOAT_ROOT}")
message(STATUS "  fmt       : ${CALC_FMT_ROOT}")
message(STATUS "  zmij      : ${CALC_ZMIJ_ROOT}")

# -----------------------------------------------------------------------------
# Target architecture
# -----------------------------------------------------------------------------

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(CALC_TARGET_ARCH "x86-64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(CALC_TARGET_ARCH "x86")
else()
  message(FATAL_ERROR
    "Unsupported target pointer size: ${CMAKE_SIZEOF_VOID_P} bytes")
endif()

set(CALC_ARCH_FLAGS)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if(CALC_TARGET_ARCH STREQUAL "x86-64")
    # x86-64-v2: SSE3, SSSE3, SSE4.1 and SSE4.2.
    set(CALC_ARCH_FLAGS -march=x86-64-v2)
    set(CALC_ARCH_DESCRIPTION "x86-64-v2 / SSE4.2")
  else()
    # 32-bit Prescott baseline: SSE3. The presets add -m32.
    set(CALC_ARCH_FLAGS -march=prescott)
    set(CALC_ARCH_DESCRIPTION "x86 / Prescott / SSE3")
  endif()
else()
  if(CALC_TARGET_ARCH STREQUAL "x86-64")
    # SSE2 is part of the MSVC x64 ABI baseline. MSVC uses the x64 calling
    # convention and does not support the x86-only /Gr or /Oy switches.
    set(CALC_ARCH_DESCRIPTION "x64 / SSE2 baseline")
  else()
    # Match Calc.vcxproj's Win32 enhanced-instruction-set setting.
    set(CALC_ARCH_FLAGS /arch:SSE2)
    set(CALC_ARCH_DESCRIPTION "Win32 / SSE2")
  endif()
endif()

message(STATUS "Target architecture: ${CALC_ARCH_DESCRIPTION}")

# -----------------------------------------------------------------------------
# Sources and executable
# -----------------------------------------------------------------------------

# This list follows the current Calc.vcxproj source list. The Windows resource
# script is added only on Windows.
set(CALC_SOURCES
  Calc/gui.cpp
  Calc/formatter.cpp
  Calc/identifiers.cpp
  Calc/lexer.cpp
  Calc/parser.cpp
  Calc/pch.cpp
  Calc/tests.cpp
)

if(WIN32)
  list(APPEND CALC_SOURCES Calc/Calc.rc)
  add_executable(Calc WIN32 ${CALC_SOURCES})
else()
  add_executable(Calc ${CALC_SOURCES})
endif()

set_target_properties(Calc PROPERTIES
  OUTPUT_NAME "Calc"
  CXX_STANDARD 23
  C_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  C_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  C_EXTENSIONS OFF
  CXX_SCAN_FOR_MODULES OFF
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)

target_include_directories(Calc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Calc"
  "${CALC_FAST_FLOAT_ROOT}/include"
  "${CALC_FMT_ROOT}/include"
  "${CALC_ZMIJ_ROOT}"
)

target_precompile_headers(Calc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Calc/pch.hpp")

target_compile_definitions(Calc PRIVATE
  "$<$<CONFIG:Debug>:_DEBUG>"
  "$<$<CONFIG:Release>:NDEBUG>"
)

# -----------------------------------------------------------------------------
# Windows / MSVC compile and link options
# -----------------------------------------------------------------------------

if(MSVC)
  target_compile_definitions(Calc PRIVATE WIN32 _WINDOWS)

  # Keep these options scoped to C++ files; Calc.rc is compiled by rc.exe.
  target_compile_options(Calc PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:/Wall>"
    "$<$<COMPILE_LANGUAGE:CXX>:/MP>"
    "$<$<COMPILE_LANGUAGE:CXX>:/FS>"
    "$<$<COMPILE_LANGUAGE:CXX>:/std:c++latest>"
    "$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>"
    "$<$<COMPILE_LANGUAGE:CXX>:/Zc:preprocessor>"
    "$<$<COMPILE_LANGUAGE:CXX>:/Zc:wchar_t->"
    "$<$<COMPILE_LANGUAGE:CXX>:/GR->"
    "$<$<COMPILE_LANGUAGE:CXX>:/EHsc>"
    "$<$<COMPILE_LANGUAGE:CXX>:/fp:precise>"

    # Requested compiler hardening/mitigation settings: disabled in both
    # configurations. Spectre mitigation is off by default; no /Qspectre flag
    # is passed.
    "$<$<COMPILE_LANGUAGE:CXX>:/GS->"
    "$<$<COMPILE_LANGUAGE:CXX>:/sdl->"
    "$<$<COMPILE_LANGUAGE:CXX>:/guard:cf->"
    "$<$<COMPILE_LANGUAGE:CXX>:/guard:ehcont->"

    # Debug.
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:/Od>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:/ZI>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:/RTC1>"

    # Release, matching Calc.vcxproj.
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/O2>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Oi>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Gy>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Gw>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/GA>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Qpar>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/GT>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Zi>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/FAs>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/GL>"
  )

  if(CALC_ARCH_FLAGS)
    target_compile_options(Calc PRIVATE
      "$<$<COMPILE_LANGUAGE:CXX>:${CALC_ARCH_FLAGS}>")
  endif()

    target_compile_options(Calc PRIVATE
      "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:/Oy>")

  target_link_options(Calc PRIVATE
    /DEBUG

    # Disable requested PE hardening and mitigation flags.
    /DYNAMICBASE:NO
    /FIXED
    /NXCOMPAT:NO
    /CETCOMPAT:NO
    /GUARD:NO
    /MANIFESTUAC:NO

    "$<$<CONFIG:Debug>:/INCREMENTAL>"
    "$<$<CONFIG:Release>:/INCREMENTAL:NO>"
    "$<$<CONFIG:Release>:/LTCG>"
    "$<$<CONFIG:Release>:/OPT:REF>"
    "$<$<CONFIG:Release>:/OPT:ICF>"
    "$<$<CONFIG:Release>:/RELEASE>"
    "$<$<CONFIG:Release>:/LARGEADDRESSAWARE:NO>"
  )

  # High Entropy VA is only valid for 64-bit PE images.
  if(CALC_TARGET_ARCH STREQUAL "x86-64")
    target_link_options(Calc PRIVATE /HIGHENTROPYVA:NO)
  endif()

  target_link_options(Calc PRIVATE /SAFESEH:NO)

  # Keep both the linker PDB and compiler PDB beside the executable,
  # separated by configuration.
  set_target_properties(Calc PROPERTIES
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
  )

# -----------------------------------------------------------------------------
# Linux / GCC and Clang compile and link options
# -----------------------------------------------------------------------------

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_options(Calc PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:-Wall>"
    "$<$<COMPILE_LANGUAGE:CXX>:-Wextra>"
    "$<$<COMPILE_LANGUAGE:CXX>:-Wno-unknown-pragmas>"
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>"

    # Requested compiler hardening/mitigation settings: disabled in both
    # configurations. -U_FORTIFY_SOURCE also overrides distro-wide defines.
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-stack-protector>"
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-stack-clash-protection>"
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-stack-check>"
    "$<$<COMPILE_LANGUAGE:CXX>:-fcf-protection=none>"
    "$<$<COMPILE_LANGUAGE:CXX>:-mno-shstk>"
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-pie>"
    "$<$<COMPILE_LANGUAGE:CXX>:-U_FORTIFY_SOURCE>"

    # Debug.
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:-O0>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:-g3>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:-fno-omit-frame-pointer>"

    # Release.
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:-O3>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:-fomit-frame-pointer>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:-ffunction-sections>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:-fdata-sections>"
    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:-flto>"
    "${CALC_ARCH_FLAGS}"
  )

  target_link_options(Calc PRIVATE
    # Requested ELF hardening/mitigation settings: disabled in both configs.
    -no-pie
    -Wl,-z,norelro
    -Wl,-z,execstack
    -Wl,-z,lazy

    "$<$<CONFIG:Release>:-flto>"
    "$<$<CONFIG:Release>:-Wl,--gc-sections>"
  )

  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # The Clang presets select LLD. Safe ICF is supported by LLD.
    target_link_options(Calc PRIVATE
      "$<$<CONFIG:Release>:-Wl,--icf=safe>")
  endif()
endif()

install(TARGETS Calc RUNTIME DESTINATION bin)
