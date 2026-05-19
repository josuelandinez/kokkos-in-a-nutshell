#!/bin/bash
export OMP_PROC_BIND=true
export OMP_PROC_BIND=spread
export OMP_NUM_THREADS=12
./build_cpu/subview_halo
