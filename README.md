# Kokkos-in-a-nutshell

This repository provides a compact, practical, and hands-on tutorial for learning **Kokkos**, the C++ performance portability programming ecosystem.

The goal of this project is to bridge the gap between basic parallel syntax and real-world high-performance computing (HPC). It demonstrates how to write single-source C++ code that respects the physical hardware limits of both modern multi-core (and hybrid) CPUs and high-throughput GPUs.

## Recommended Learning Path

Before diving into the code provided here, it is highly recommended that you familiarize yourself with the official Kokkos instructional materials:

* [Kokkos Tutorials and Examples (Wiki)](https://kokkos.org/kokkos-core-wiki/tutorials-and-examples.html)
* [Kokkos Tutorials (GitHub Repository)](https://github.com/kokkos/kokkos-tutorials)
* [Kokkos Video Lectures](https://kokkos.org/kokkos-core-wiki/tutorials-and-examples/video-lectures.html)

## Compiling and Running

Each exercise directory contains standalone CMake builds and helper scripts. 

* **Build Scripts (`build_cpu.sh` / `build_gpu.sh`):** These configure CMake for specific hardware backends (e.g., OpenMP for CPU, CUDA for NVIDIA GPUs). They automatically pull Kokkos via `FetchContent`.
* **Run Scripts (`run_cpu.sh` / `run_gpu.sh`):** These execute the compiled binaries, sometimes wrapping them in `mpirun` for the distributed exercises.

** Cluster Environments (Slurm):**
The provided run scripts are designed for local execution or interactive compute nodes. If you are running on an HPC cluster (like those at JSC), you cannot run these directly on the login nodes. You must adapt the script to build and execution commands into **Slurm batch scripts** and use `srun` (instead of `mpirun` or direct execution) to dispatch the jobs to the compute nodes.

## Repository Structure & Exercises

### Foundation Modules (`001` - `010`)
These introductory modules cover the core mechanics of the Kokkos API:
* Initialization and Finalization.
* `Kokkos::View` allocations and memory spaces (Host vs. Device).
* Execution patterns: `parallel_for`, `parallel_reduce`, and `parallel_scan`.
* Managing multi-dimensional data layouts (Row-major vs. Column-major).

### Advanced Distributed Memory (`011` - `013`)
These exercises tackle distributed-memory parallelism by combining Kokkos with MPI.
* **Note:** Exercises `011` and `013` focus specifically on network communication and boundary synchronization. They are heavily based on the official [MPI Halo Exchange Use Case](https://kokkos.org/kokkos-core-wiki/usecases/MPI-Halo-Exchange.html).
* They demonstrate how to efficiently pack, transfer, and unpack boundary data (halos) between discrete memory spaces while minimizing device-to-host transfer bottlenecks.

### Hardware Tuning & Auto-Optimization (`014` - `015`)
The final modules demonstrate how to improve hardware efficiency using the NuMeriQS `KTune` library.
* **KTune Presentation:** [Introduction to KTune (JSC KUG)](https://aniketsen.github.io/ktune-jsc-kug/)
* Benchmarking memory-bound kernels and measuring effective memory bandwidth.
* Using runtime search algorithms (Nelder-Mead) to automatically discover optimal hardware tile sizes.
* Combining KTune with asynchronous MPI transfers to create a production-ready HPC application.

## Essential Kokkos References
* **Main Documentation:** [Kokkos Core Wiki Index](https://kokkos.org/kokkos-core-wiki/index.html)
* **API Reference:** [Kokkos API Documentation](https://kokkos.org/kokkos-core-wiki/api-references.html)
* **Profiling & Debugging:** [Kokkos Tools (GitHub)](https://github.com/kokkos/kokkos-tools)