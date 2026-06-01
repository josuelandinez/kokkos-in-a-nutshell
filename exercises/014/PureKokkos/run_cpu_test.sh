#!/bin/bash

# 1. Enforce Thread Binding (Affinity)
# 'cores' prevents OpenMP from putting two threads on the same physical core
# 'spread' spaces the threads out to maximize L3 cache usage
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

# 2. Prevent OpenMP from silently creating background threads
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false

# 3. Test different thread counts
for THREADS in 4 12 16; do
    echo "============================================="
    echo " TESTING WITH OMP_NUM_THREADS = $THREADS"
    echo "============================================="
    
    export OMP_NUM_THREADS=$THREADS
    
    # Delete the KTune cache so it actually tunes for this specific thread count
    rm -f ktune_cache.dat
    
    ./build_cpu/matvec_kokkos
    echo ""
done
