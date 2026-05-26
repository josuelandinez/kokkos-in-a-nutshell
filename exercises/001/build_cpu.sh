#!/bin/bash
#SBATCH --account=training2542
#SBATCH --partition=dc-cpu-devel
#SBATCH --output=build.out
#SBATCH --error=build.err

module --force purge
module load Stages/2025 \
            GCC/13.3.0 \
            OpenMPI/5.0.5 \
            CUDA/12 \
            CMake/3.29.3 \

rm -rf build_cpu
mkdir build_cpu
#cpu build, openmp
cmake -B build_cpu \
  -DCMAKE_BUILD_TYPE=Release \
  -DKokkos_ENABLE_OPENMP=ON \
  -DKokkos_ENABLE_SERIAL=ON
cmake --build build_cpu -j 8 
