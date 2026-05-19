# Exercise 013: The Capstone Halo Exchange (MPI + Subviews)

In **Exercise 011**, you learned how to use MPI to exchange boundary data between different hardware nodes.
In **Exercise 012**, you learned how `Kokkos::subview` allows you to extract 1D slices (like rows or strided columns) from a 2D grid without duplicating memory.

This exercise is your capstone challenge: **Combine them.**

## The Problem
MPI requires contiguous memory buffers to send data over the network. If you want to send the right-most column of a Row-Major 2D grid, that memory is strided (fragmented). You cannot pass it directly to `MPI_Send`. 

## Your Task
Write a Kokkos application (`halo_exchange.cpp`) that performs a 2D distributed halo exchange. You must implement the following pipeline:

1. **Allocate:** Create a 2D Device grid and 1D Device/Host staging buffers.
2. **Extract:** Use `Kokkos::subview` to grab the 1D boundaries of your grid.
3. **Pack:** Copy the strided 2D boundary data into a contiguous 1D Device buffer.
4. **Transfer:** Use `Kokkos::deep_copy` to move only the packed 1D buffer across the PCIe bus to the Host.
5. **Communicate:** Use `MPI_Isend` and `MPI_Irecv` to exchange the Host buffers with neighboring MPI ranks.
6. **Overlap (Bonus):** Compute the interior grid points on the GPU *while* the CPU handles the MPI network traffic.
7. **Unpack:** Move the received data back to the Device and unpack it into the ghost cells.

**Hint:** Rely heavily on the `auto` keyword when creating your subviews so you don't have to manually type out the complex `LayoutStride` template parameters!

## Compiling and Running
Once you have written your implementation, build and run it using the standard scripts:

**To build for the CPU:**
```bash
./build_cpu.sh
mpirun -n 2 ./build_cpu/halo_exchange