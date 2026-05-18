# Kokkos-in-a-nutshell

This repository provides a compact, practical, and hands-on tutorial for learning **Kokkos**, the C++ performance portability programming ecosystem.

The goal of this project is to bridge the gap between basic parallel syntax and real-world high-performance computing (HPC). It demonstrates how to write single-source C++ code that respects the physical hardware limits of both modern multi-core (and hybrid) CPUs and high-throughput GPUs.

## 📖 Recommended Learning Path

Before diving into the code provided here, it is highly recommended that you familiarize yourself with the official Kokkos instructional materials. This repository is meant to supplement, not replace, the core Kokkos documentation.

New users should begin with the **Kokkos Short and Intermediate Tutorials**:

* [Kokkos Tutorials and Examples (Wiki)](https://kokkos.org/kokkos-core-wiki/tutorials-and-examples.html)
* [Kokkos Tutorials (GitHub Repository)](https://github.com/kokkos/kokkos-tutorials)
* [Kokkos Video Lectures](https://kokkos.org/kokkos-core-wiki/tutorials-and-examples/video-lectures.html)

## 📂 Repository Structure & Exercises

This tutorial is structured as a series of incrementally complex exercises. Each directory contains standalone CMake builds and scripts designed to test specific features of the Kokkos ecosystem.

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

### Hardware Tuning & Auto-Optimization (`014`)

The final module demonstrates how to extract maximum hardware efficiency using auto-tuning libraries (like NuMeriQS `KTune`).

* Benchmarking memory-bound kernels (e.g., Matrix-Vector Multiplication).
* Comparing raw memory bandwidth between system RAM (DDR4/DDR5) and GPU VRAM (GDDR6).
* Configuring thread affinity and OpenMP binding to bypass performance traps on modern asymmetric/hybrid CPUs (P-cores vs. E-cores).
* Writing aggressive CMake Release configurations for `gcc` and `nvcc`.

## 🔗 Essential Kokkos References

For deep dives into specific API calls, performance tools, or advanced asynchronous programming, refer to the following official resources:

* **Main Documentation:** [Kokkos Core Wiki Index](https://kokkos.org/kokkos-core-wiki/index.html)
* **API Reference:** [Kokkos API Documentation](https://kokkos.org/kokkos-core-wiki/api-references.html)
* **Profiling & Debugging:** [Kokkos Tools (GitHub)](https://github.com/kokkos/kokkos-tools)
* **Advanced DAGs:** [Tasking Use Case](https://kokkos.org/kokkos-core-wiki/usecases/Tasking.html)