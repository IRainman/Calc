name: CMake Linux

on:
  push:
    branches:
      - master

  pull_request:
    branches:
      - master

  workflow_dispatch:

concurrency:
  group: cmake-linux-${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true

permissions:
  contents: read

jobs:
  build:
    name: ${{ matrix.compiler_name }} ${{ matrix.architecture_name }}

    runs-on: cachyos

    strategy:
      fail-fast: false

      matrix:
        include:

          # ==================================================================
          # Clang / x86-64-v2
          # ==================================================================

          - compiler: clang
            compiler_name: Clang
            architecture: x64
            architecture_name: x86-64-v2
            build_dir: build/clang-x64

          # ==================================================================
          # Clang / 32-bit Prescott / SSE3
          # ==================================================================

          - compiler: clang
            compiler_name: Clang
            architecture: x86
            architecture_name: x86-Prescott-SSE3
            build_dir: build/clang-x86

          # ==================================================================
          # GCC / x86-64-v2
          # ==================================================================

          - compiler: gcc
            compiler_name: GCC
            architecture: x64
            architecture_name: x86-64-v2
            build_dir: build/gcc-x64

          # ==================================================================
          # GCC / 32-bit Prescott / SSE3
          # ==================================================================

          - compiler: gcc
            compiler_name: GCC
            architecture: x86
            architecture_name: x86-Prescott-SSE3
            build_dir: build/gcc-x86

    steps:

      # ======================================================================
      # Checkout Calc
      # ======================================================================

      - name: Checkout Calc
        uses: actions/checkout@v6

      # ======================================================================
      # Verify CachyOS
      # ======================================================================

      - name: Verify CachyOS
        shell: bash
        run: |
          set -eux

          echo "========================================"
          echo "Operating system"
          echo "========================================"

          cat /etc/os-release

          if ! grep -qi "cachyos" /etc/os-release; then
            echo "ERROR: GitHub runner is not CachyOS."
            exit 1
          fi

          echo
          echo "========================================"
          echo "Kernel / host"
          echo "========================================"

          uname -a
          uname -m

      # ======================================================================
      # Enable multilib for 32-bit builds
      # ======================================================================

      - name: Enable multilib
        if: matrix.architecture == 'x86'
        shell: bash
        run: |
          set -eux

          if ! grep -q '^\[multilib\]' /etc/pacman.conf; then
            printf '\n[multilib]\nInclude = /etc/pacman.d/mirrorlist\n' \
              | sudo tee -a /etc/pacman.conf
          fi

          sudo pacman -Sy --noconfirm

      # ======================================================================
      # Install Clang
      # ======================================================================

      - name: Install Clang toolchain
        if: matrix.compiler == 'clang'
        shell: bash
        run: |
          set -eux

          sudo pacman -Syu --noconfirm --needed \
            clang \
            llvm \
            lld \
            cmake \
            ninja \
            git

      # ======================================================================
      # Install GCC
      # ======================================================================

      - name: Install GCC toolchain
        if: matrix.compiler == 'gcc'
        shell: bash
        run: |
          set -eux

          sudo pacman -Syu --noconfirm --needed \
            gcc \
            cmake \
            ninja \
            git

      # ======================================================================
      # Install 32-bit Clang support
      # ======================================================================

      - name: Install 32-bit Clang support
        if: matrix.compiler == 'clang' && matrix.architecture == 'x86'
        shell: bash
        run: |
          set -eux

          sudo pacman -S --noconfirm --needed \
            lib32-glibc \
            lib32-clang \
            lib32-llvm

      # ======================================================================
      # Install 32-bit GCC support
      # ======================================================================

      - name: Install 32-bit GCC support
        if: matrix.compiler == 'gcc' && matrix.architecture == 'x86'
        shell: bash
        run: |
          set -eux

          sudo pacman -S --noconfirm --needed \
            lib32-glibc \
            lib32-gcc-libs

      # ======================================================================
      # Verify Clang
      # ======================================================================

      - name: Verify Clang
        if: matrix.compiler == 'clang'
        shell: bash
        run: |
          set -eux

          echo "========================================"
          echo "Clang"
          echo "========================================"

          clang --version
          clang++ --version

          echo
          echo "========================================"
          echo "LLVM"
          echo "========================================"

          llvm-config --version

          echo
          echo "========================================"
          echo "LLD"
          echo "========================================"

          ld.lld --version

      # ======================================================================
      # Verify GCC
      # ======================================================================

      - name: Verify GCC
        if: matrix.compiler == 'gcc'
        shell: bash
        run: |
          set -eux

          echo "========================================"
          echo "GCC"
          echo "========================================"

          gcc --version
          g++ --version

      # ======================================================================
      # Verify CMake and Ninja
      # ======================================================================

      - name: Verify CMake and Ninja
        shell: bash
        run: |
          set -eux

          echo "========================================"
          echo "CMake"
          echo "========================================"

          cmake --version

          echo
          echo "========================================"
          echo "Ninja"
          echo "========================================"

          ninja --version

      # ======================================================================
      # Verify 32-bit compiler
      # ======================================================================

      - name: Verify 32-bit compiler
        if: matrix.architecture == 'x86'
        shell: bash
        run: |
          set -eux

          if [ "${{ matrix.compiler }}" = "clang" ]; then

            clang++ \
              -m32 \
              -march=prescott \
              -x c++ \
              -c \
              /dev/null \
              -o /tmp/calc-x86-test.o

          else

            g++ \
              -m32 \
              -march=prescott \
              -x c++ \
              -c \
              /dev/null \
              -o /tmp/calc-x86-test.o

          fi

          file /tmp/calc-x86-test.o

          rm -f /tmp/calc-x86-test.o

      # ======================================================================
      # Checkout sibling dependencies
      #
      # Required by Calc/pch.hpp:
      #
      #   ../../fast_float/include/fast_float/fast_float.h
      #   ../../fmt/include/fmt/compile.h
      #   ../../zmij/zmij.cc
      # ======================================================================

      - name: Checkout dependencies
        shell: bash
        run: |
          set -eux

          PARENT="$(dirname "$GITHUB_WORKSPACE")"

          echo "Calc:"
          echo "  $GITHUB_WORKSPACE"

          echo
          echo "Dependencies:"
          echo "  $PARENT/fast_float"
          echo "  $PARENT/fmt"
          echo "  $PARENT/zmij"

          rm -rf \
            "$PARENT/fast_float" \
            "$PARENT/fmt" \
            "$PARENT/zmij"

          git clone \
            --depth 1 \
            --branch main \
            https://github.com/IRainman/fast_float.git \
            "$PARENT/fast_float"

          git clone \
            --depth 1 \
            --branch main \
            https://github.com/IRainman/fmt.git \
            "$PARENT/fmt"

          git clone \
            --depth 1 \
            --branch main \
            https://github.com/IRainman/zmij.git \
            "$PARENT/zmij"

      - name: Verify dependency layout
        shell: bash
        run: |
          set -eux

          PARENT="$(dirname "$GITHUB_WORKSPACE")"

          test -f \
            "$PARENT/fast_float/include/fast_float/fast_float.h"

          test -f \
            "$PARENT/fmt/include/fmt/compile.h"

          test -f \
            "$PARENT/zmij/zmij.cc"

          echo
          echo "Dependency layout verified."

      # ======================================================================
      # Configure Clang x86-64-v2
      # ======================================================================

      - name: Configure Clang x86-64-v2
        if: matrix.compiler == 'clang' && matrix.architecture == 'x64'
        shell: bash
        env:
          BUILD_DIR: ${{ matrix.build_dir }}
        run: |
          set -eux

          cmake \
            -S . \
            -B "$BUILD_DIR" \
            -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_C_COMPILER=clang \
            -DCMAKE_CXX_COMPILER=clang++ \
            -DCMAKE_C_STANDARD=23 \
            -DCMAKE_C_STANDARD_REQUIRED=ON \
            -DCMAKE_C_EXTENSIONS=OFF \
            -DCMAKE_CXX_STANDARD=23 \
            -DCMAKE_CXX_STANDARD_REQUIRED=ON \
            -DCMAKE_CXX_EXTENSIONS=OFF \
            -DCMAKE_C_FLAGS="-march=x86-64-v2" \
            -DCMAKE_CXX_FLAGS="-march=x86-64-v2" \
            -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld"

      # ======================================================================
      # Configure Clang x86 Prescott / SSE3
      # ======================================================================

      - name: Configure Clang x86 Prescott SSE3
        if: matrix.compiler == 'clang' && matrix.architecture == 'x86'
        shell: bash
        env:
          BUILD_DIR: ${{ matrix.build_dir }}
        run: |
          set -eux

          cmake \
            -S . \
            -B "$BUILD_DIR" \
            -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_C_COMPILER=clang \
            -DCMAKE_CXX_COMPILER=clang++ \
            -DCMAKE_C_STANDARD=23 \
            -DCMAKE_C_STANDARD_REQUIRED=ON \
            -DCMAKE_C_EXTENSIONS=OFF \
            -DCMAKE_CXX_STANDARD=23 \
            -DCMAKE_CXX_STANDARD_REQUIRED=ON \
            -DCMAKE_CXX_EXTENSIONS=OFF \
            -DCMAKE_C_FLAGS="-m32 -march=prescott" \
            -DCMAKE_CXX_FLAGS="-m32 -march=prescott" \
            -DCMAKE_EXE_LINKER_FLAGS="-m32 -fuse-ld=lld"

      # ======================================================================
      # Configure GCC x86-64-v2
      # ======================================================================

      - name: Configure GCC x86-64-v2
        if: matrix.compiler == 'gcc' && matrix.architecture == 'x64'
        shell: bash
        env:
          BUILD_DIR: ${{ matrix.build_dir }}
        run: |
          set -eux

          cmake \
            -S . \
            -B "$BUILD_DIR" \
            -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_C_COMPILER=gcc \
            -DCMAKE_CXX_COMPILER=g++ \
            -DCMAKE_C_STANDARD=23 \
            -DCMAKE_C_STANDARD_REQUIRED=ON \
            -DCMAKE_C_EXTENSIONS=OFF \
            -DCMAKE_CXX_STANDARD=23 \
            -DCMAKE_CXX_STANDARD_REQUIRED=ON \
            -DCMAKE_CXX_EXTENSIONS=OFF \
            -DCMAKE_C_FLAGS="-march=x86-64-v2" \
            -DCMAKE_CXX_FLAGS="-march=x86-64-v2"

      # ======================================================================
      # Configure GCC x86 Prescott / SSE3
      # ======================================================================

      - name: Configure GCC x86 Prescott SSE3
        if: matrix.compiler == 'gcc' && matrix.architecture == 'x86'
        shell: bash
        env:
          BUILD_DIR: ${{ matrix.build_dir }}
        run: |
          set -eux

          cmake \
            -S . \
            -B "$BUILD_DIR" \
            -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_C_COMPILER=gcc \
            -DCMAKE_CXX_COMPILER=g++ \
            -DCMAKE_C_STANDARD=23 \
            -DCMAKE_C_STANDARD_REQUIRED=ON \
            -DCMAKE_C_EXTENSIONS=OFF \
            -DCMAKE_CXX_STANDARD=23 \
            -DCMAKE_CXX_STANDARD_REQUIRED=ON \
            -DCMAKE_CXX_EXTENSIONS=OFF \
            -DCMAKE_C_FLAGS="-m32 -march=prescott" \
            -DCMAKE_CXX_FLAGS="-m32 -march=prescott" \
            -DCMAKE_EXE_LINKER_FLAGS="-m32"

      # ======================================================================
      # Verify CMake configuration
      # ======================================================================

      - name: Verify CMake configuration
        shell: bash
        env:
          BUILD_DIR: ${{ matrix.build_dir }}
          COMPILER: ${{ matrix.compiler }}
          ARCHITECTURE: ${{ matrix.architecture }}
        run: |
          set -eux

          CACHE="$BUILD_DIR/CMakeCache.txt"

          test -f "$CACHE"

          echo "========================================"
          echo "CMake compiler"
          echo "========================================"

          grep \
            'CMAKE_CXX_COMPILER_ID:' \
            "$CACHE"

          grep \
            'CMAKE_CXX_COMPILER:' \
            "$CACHE"

          echo
          echo "========================================"
          echo "C standard"
          echo "========================================"

          grep \
            'CMAKE_C_STANDARD:' \
            "$CACHE" || true

          echo
          echo "========================================"
          echo "C++ standard"
          echo "========================================"

          grep \
            'CMAKE_CXX_STANDARD:' \
            "$CACHE" || true

          echo
          echo "========================================"
          echo "Pointer size"
          echo "========================================"

          grep \
            'CMAKE_SIZEOF_VOID_P:' \
            "$CACHE"

          if [ "$COMPILER" = "clang" ]; then

            if ! grep -q \
                'CMAKE_CXX_COMPILER_ID:.*Clang' \
                "$CACHE"; then
              echo "ERROR: Expected Clang."
              exit 1
            fi

          else

            if ! grep -q \
                'CMAKE_CXX_COMPILER_ID:.*GNU' \
                "$CACHE"; then
              echo "ERROR: Expected GCC/GNU."
              exit 1
            fi

          fi

          if [ "$ARCHITECTURE" = "x64" ]; then

            if ! grep -q \
                'CMAKE_SIZEOF_VOID_P:INTERNAL=8' \
                "$CACHE"; then
              echo "ERROR: Expected 64-bit build."
              exit 1
            fi

          else

            if ! grep -q \
                'CMAKE_SIZEOF_VOID_P:INTERNAL=4' \
                "$CACHE"; then
              echo "ERROR: Expected 32-bit build."
              exit 1
            fi

          fi

      # ======================================================================
      # Build
      # ======================================================================

      - name: Build
        shell: bash
        env:
          BUILD_DIR: ${{ matrix.build_dir }}
        run: |
          set -eux

          cmake \
            --build "$BUILD_DIR" \
            --parallel

      # ======================================================================
      # Verify executable
      # ======================================================================

      - name: Verify executable
        shell: bash
        env:
          BUILD_DIR: ${{ matrix.build_dir }}
          ARCHITECTURE: ${{ matrix.architecture }}
        run: |
          set -eux

          executable="$BUILD_DIR/Release/Calc"

          if [ ! -f "$executable" ]; then
            echo "ERROR: Calc executable was not produced:"
            echo "  $executable"

            echo
            echo "Build tree:"
            find "$BUILD_DIR" \
              -maxdepth 4 \
              -type f \
              -print

            exit 1
          fi

          echo "========================================"
          echo "Calc executable"
          echo "========================================"

          file "$executable"

          echo
          echo "Architecture check:"

          if [ "$ARCHITECTURE" = "x64" ]; then
            file "$executable" | grep -E "ELF 64-bit"
          else
            file "$executable" | grep -E "ELF 32-bit"
          fi

      # ======================================================================
      # Upload artifact
      # ======================================================================

      - name: Upload executable
        uses: actions/upload-artifact@v6
        with:
          name: Calc-CMake-${{ matrix.compiler }}-Linux-${{ matrix.architecture }}
          path: |
            ${{ matrix.build_dir }}/Release/Calc
          if-no-files-found: error
          retention-days: 14

      # ======================================================================
      # Diagnostics
      # ======================================================================

      - name: Upload CMake diagnostics
        if: failure()
        uses: actions/upload-artifact@v6
        with:
          name: Calc-CMake-Diagnostics-${{ matrix.compiler }}-${{ matrix.architecture }}
          path: |
            ${{ matrix.build_dir }}/CMakeCache.txt
            ${{ matrix.build_dir }}/CMakeFiles/CMakeError.log
            ${{ matrix.build_dir }}/CMakeFiles/CMakeOutput.log
          if-no-files-found: ignore
          retention-days: 14
