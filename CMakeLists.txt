cmake_minimum_required(VERSION 3.14...3.22)

# ---- Project ----

project(
  Calc
  VERSION 2.10.0
  LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

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
  NAME zmij
  GIT_TAG main
  GITHUB_REPOSITORY IRainman/fast_float
  DOWNLOAD_ONLY YES
)
