#include <Kokkos_Core.hpp>
#include <cstdio>

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        int N = 1000;
        Kokkos::View<double*> X("X", N);

        // Initialize with 1.0
        Kokkos::parallel_for("Init", N, KOKKOS_LAMBDA(const int i) {
            X(i) = 1.0;
        });
        Kokkos::fence();

        printf("--- Running Fused Kernel ---\n");

        double norm_sq = 0.0;

        // Fused Parallel Reduce
        // Instead of running two separate kernels:
        //   1. Update X(i) = 2.0 * X(i)
        //   2. Reduce sum += X(i) * X(i)
        // We do it all in one pass. This means X(i) is read once, 
        // updated in cache, and written back once.
        Kokkos::parallel_reduce("FusedUpdateNorm", N, 
            KOKKOS_LAMBDA(const int i, double& lsum) {
                // Perform the update
                X(i) = 2.0 * X(i);
                
                // Accumulate the norm using the updated value
                lsum += X(i) * X(i);
            }, 
        norm_sq);
        Kokkos::fence();

        printf("Fused Result: %f (Expected: %f)\n", norm_sq, (double)N * 4.0);

    }
    Kokkos::finalize();
    return 0;
}
