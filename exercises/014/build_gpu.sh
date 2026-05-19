#!/bin/bash
rm -rf build_gpu
mkdir build_gpu
cmake -B build_gpu \
  -DKokkos_ENABLE_CUDA=ON \
  -DKokkos_ARCH_AMPERE86=ON \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build_gpu -j 8
