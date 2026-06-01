#!/bin/bash
rm -rf build_cpu
mkdir build_cpu
cmake -B build_cpu -DKokkos_ENABLE_OPENMP=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build_cpu -j 8
