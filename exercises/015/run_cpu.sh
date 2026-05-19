#!/bin/bash

# 'cores' prevents OpenMP from putting two threads on the same physical core
# 'spread' spaces the threads out to maximize L3 cache usage
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

# Prevent OpenMP from silently creating background threads
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false

export OMP_NUM_THREADS=12
rm -rf ktune_cache.dat
   
./build_cpu/halo_ktune
