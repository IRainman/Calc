cmake_minimum_required(VERSION 3.22)

project(Calc VERSION 2.10.0 LANGUAGES CXX)

include(cmake/CPM.cmake)


CPMAddPackage(
  NAME fmt
  GIT_TAG main
  GITHUB_REPOSITORY IRainman/fmt
  DOWNLOAD_ONLY YES
)

CPMAddPackage(
  NAME zmij
  GIT_TAG main
  GITHUB_REPOSITORY IRainman/zmij
  DOWNLOAD_ONLY YES
)

CPMAddPackage(
  NAME fast_float
  GIT_TAG main
  GITHUB_REPOSITORY IRainman/fast_float
  DOWNLOAD_ONLY YES
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
# ---- Executable target ----
# Use WIN32 to set SubSystem:Windows like the original project.
add_executable(Calc WIN32 ${CALC_SOURCES})
target_include_directories(Calc PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Calc")

# Link third-party targets if available
if(TARGET fmt::fmt)
  target_link_libraries(Calc PRIVATE fmt::fmt)
endif()
if(TARGET zmij::zmij)
  target_link_libraries(Calc PRIVATE zmij::zmij)
endif()
if(TARGET fast_float::fast_float)
  target_link_libraries(Calc PRIVATE fast_float::fast_float)
endif()

# ---- C++ standard: prefer CMake property (maps to /std:c++latest on MSVC where supported) ----
# Use CMake standardized property rather than hard-coding /std:c++latest
target_compile_features(Calc PRIVATE cxx_std_23)
set_target_properties(Calc PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_precompile_headers(Calc PRIVATE "Calc/pch.hpp")

set_target_properties(Calc PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
  # CMAKE_VS_PLATFORM_NAME -> Win32 or x64
  set_target_properties(Calc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${CMAKE_VS_PLATFORM_NAME}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/out/${CMAKE_VS_PLATFORM_NAME}"
    OUTPUT_NAME "Calc"
  )
endif()

target_compile_definitions(Calc PRIVATE WIN32 _WINDOWS)
target_compile_definitions(Calc PRIVATE
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)
if(MSVC)
  set_property(TARGET Calc PROPERTY
    MSVC_RUNTIME_LIBRARY $<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<CONFIG:Release>:MultiThreadedDLL>
  )
  set_property(TARGET Calc PROPERTY INTERPROCEDURAL_OPTIMIZATION $<$<CONFIG:Release>:ON>)
  target_compile_options(Calc PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4>
    $<$<CONFIG:Debug>:/Zi; /Od; /MDd; /fp:precise, /fp:except->
    $<$<CONFIG:Release>:/O2; /Ob2; /Gy; /fp:precise, /fp:except->
  )
  target_compile_options(Calc PRIVATE /Zc:__cplusplus)
  target_link_options(Calc PRIVATE
    $<$<CONFIG:Release>:/DYNAMICBASE:NO>
    $<$<CONFIG:Release>:/FIXED>
    $<$<CONFIG:Debug>:/DEBUG>
  )
  target_compile_options(Calc PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/arch:SSE2>)
endif()
if(NOT MSVC)
  # Prefer CMake properties and features rather than hard-coded flags
  target_compile_options(Calc PRIVATE
    $<$<CONFIG:Debug>:-O0 -g -Wall -Wextra>
    $<$<CONFIG:Release>:-O3 -march=native -flto -Wall>
  )

  # Enable link-time optimization via CMake property where supported
  include(CheckIPOSupported)
  check_ipo_supported(RESULT IPO_SUPPORTED)
  if(IPO_SUPPORTED)
    set_property(TARGET Calc PROPERTY INTERPROCEDURAL_OPTIMIZATION $<$<CONFIG:Release>:TRUE>)
  endif()
endif()

target_include_directories(Calc PRIVATE
  $<$<BOOL:${CMAKE_BINARY_DIR}>:${CMAKE_BINARY_DIR}/_deps/zmij-src>
)
target_include_directories(Calc PRIVATE
  $<$<BOOL:${CMAKE_BINARY_DIR}>:${CMAKE_BINARY_DIR}/_deps/fast-float-src>
)
target_include_directories(Calc PRIVATE
  $<$<BOOL:${CMAKE_BINARY_DIR}>:${CMAKE_BINARY_DIR}/_deps/fmt-src>
)

install(TARGETS Calc RUNTIME DESTINATION bin)
