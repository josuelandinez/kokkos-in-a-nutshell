#!/bin/bash
cmake -B build_cpu \
  -DCMAKE_BUILD_TYPE=Release \
  -DKokkos_ENABLE_OPENMP=ON \
  -DKokkos_ENABLE_SERIAL=ON
cmake --build build_cpu -j$(nproc)
