#include <Kokkos_Core.hpp>
#include <cstdio>

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        int N = 100000;

        printf("Allocating 1D Views of size %d...\n", N);

        // The Kokkos::View
        // A View is a multidimensional smart pointer.
        // By default, it allocates memory in the DefaultExecutionSpace's memory space.
        // If compiled for GPU, these arrays live in VRAM. If CPU, they live in RAM.
        // The string "A" is a label used for memory profiling and debugging.
        Kokkos::View<double*> A("A", N);
        Kokkos::View<double*> B("B", N);
        Kokkos::View<double*> C("C", N);

        printf("Initializing data...\n");

        // Parallel Initialization
        // We use a KOKKOS_LAMBDA to initialize the arrays in parallel.
        Kokkos::parallel_for("InitArrays", N, KOKKOS_LAMBDA(const int i) {
            A(i) = 1.0;
            B(i) = 2.0;
        });
        Kokkos::fence(); // Wait for initialization to finish

        printf("Performing Vector Addition...\n");

        // Parallel Vector Addition
        Kokkos::parallel_for("VectorAdd", N, KOKKOS_LAMBDA(const int i) {
            C(i) = A(i) + B(i);
        });
        Kokkos::fence(); // Wait for math to finish

        printf("Math complete!\n");

        // --- THE CLIFFHANGER ---
        // You might be tempted to do this:
        // printf("C[0] = %f\n", C(0)); 
        //
        // WARNING: If you compile this for a GPU, uncommenting that printf 
        // will cause a Segmentation Fault! 
        // Why? Because 'C' lives in GPU VRAM, and the 'printf' is being called 
        // by the CPU (Host), which cannot directly access GPU memory.
        // We will solve this in Exercise 004!

    } // Scope braces ensure Views are deallocated before finalize
    Kokkos::finalize();
    return 0;
}
