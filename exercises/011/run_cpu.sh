#!/bin/bash
export OMP_PROC_BIND=true
export OMP_PROC_BIND=spread
export OMP_NUM_THREADS=3
mpirun -np 4 ./build_cpu/halo_exchange
