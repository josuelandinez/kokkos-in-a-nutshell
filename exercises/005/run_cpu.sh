#!/bin/bash
export OMP_PROC_BIND=true
export OMP_PROC_BIND=spread


./build_cpu/reductions
