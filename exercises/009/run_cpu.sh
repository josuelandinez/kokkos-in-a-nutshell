#!/bin/bash
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

./build_cpu/fused_kernels
