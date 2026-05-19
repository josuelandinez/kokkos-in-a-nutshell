#!/bin/bash
rm -rf build_cpu
mkdir build_cpu
cmake -B build_cpu -DKokkos_ENABLE_OPENMP=ON
cmake --build build_cpu -j 8
