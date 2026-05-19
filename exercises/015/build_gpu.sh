#!/bin/bash
rm -rf build_gpu
cmake -B build_gpu -DCMAKE_BUILD_TYPE=Release -DKokkos_ENABLE_CUDA=ON -DKokkos_ARCH_AMPERE86=ON
cmake --build build_gpu -j$(nproc)
