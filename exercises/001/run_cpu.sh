#!/bin/bash
#SBATCH --account=training2542
#SBATCH --partition=dc-cpu-devel
#SBATCH --output=run001.out
#SBATCH --error=run001.err

export OMP_PROC_BIND=true
export OMP_PROC_BIND=spread

srun ./build_cpu/hello
