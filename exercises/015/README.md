# Exercise 015: Auto-Tuning at Scale (KTune + MPI)

In **Exercise 011**, you built a distributed memory Halo Exchange using MPI and Kokkos. In **Exercise 014**, you learned how to use NuMeriQS `KTune` to automatically discover the optimal hardware tile sizes for a local kernel.

Now, we combine them to measure actual HPC application performance.

## The Problem with Small Grids
If you tune an 8x8 grid, you will see zero performance improvement. The latency of communicating over the network via MPI completely hides the computation time of the GPU. To see the benefits of hardware tuning, we must scale up our physics problem so that the interior computation becomes heavily **memory-bound**.

## Your Task
Write `halo_ktune.cpp` by adapting your previous MPI Halo Exchange code. You must implement the following:

1. **Scale Up:** Increase the interior grid size to at least `n = 4096`. 
2. **Integrate KTune:** Initialize KTune and replace your interior `Kokkos::parallel_for` with `KTune::parallel_for`. Make sure you give it a unique label (e.g., `"Tuned_InternalCompute"`).
3. **The Benchmark Loop:** Wrap the entire timestep (Pack -> Transfer -> MPI_Isend -> Compute -> Unpack) in a `for` loop that runs 50 times.
4. **Time It:** Use `Kokkos::Timer` to wrap the benchmark loop and print the average time per step.

## Running the Benchmark
Because the first run includes KTune's search phase, you must run your compiled code **twice**. The first run generates `ktune_cache.dat`. The second run represents your true, optimized steady-state physics performance.

**CPU Test:**
```bash
./build_cpu.sh
mpirun -n 2 ./build_cpu/halo_ktune # Run 1: Tunes and creates cache
mpirun -n 2 ./build_cpu/halo_ktune # Run 2: Fast benchmark