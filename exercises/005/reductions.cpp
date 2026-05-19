#include <Kokkos_Core.hpp>
#include <cstdio>

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    { // Scope braces
        int N = 1000;

        Kokkos::View<double*> A("A", N);
        Kokkos::View<double*> B("B", N);

        // Initialize data
        Kokkos::parallel_for("InitArrays", N, KOKKOS_LAMBDA(const int i) {
            A(i) = 2.0;
            B(i) = 3.0;
        });
        Kokkos::fence();

        // The Global Variable (lives on the CPU / Host)
        double global_sum = 0.0;

        printf("Calculating Dot Product...\n");

        // Parallel Reduce
        // Notice the Lambda signature: it takes the index 'i' AND a reference 
        // to a thread-local accumulator 'lsum'.
        // Kokkos gives every thread its own safe 'lsum', and then 
        // combines them all into 'global_sum' at the very end.
        Kokkos::parallel_reduce("DotProduct", N, 
            KOKKOS_LAMBDA(const int i, double& lsum) {
                lsum += A(i) * B(i);
            }, 
        global_sum); // The final answer drops in here!
        
        Kokkos::fence();

        printf("Dot Product: %f\n", global_sum);
        printf("Expected:    %f\n", (double)N * 2.0 * 3.0);

    }
    Kokkos::finalize();
    return 0;
}
