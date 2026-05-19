#include <Kokkos_Core.hpp>
#include <cstdio>

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        int N = 5; // A very small grid so we can print and see the wrap-around

        Kokkos::View<double*> in("Input", N);
        Kokkos::View<double*> out("Output", N);

        // Initialize the grid with sequential numbers: [0, 1, 2, 3, 4]
        Kokkos::parallel_for("Init", N, KOKKOS_LAMBDA(const int i) {
            in(i) = (double)i;
        });
        Kokkos::fence();

        printf("--- Applying 1D Periodic Stencil ---\n");

        // The Periodic Loop
        // Notice we are looping over the ENTIRE grid (0 to N-1) this time!
        Kokkos::parallel_for("PeriodicHop", N, KOKKOS_LAMBDA(const int i) {
            
            // --- The Magic Modulo Math ---
            // To go RIGHT (+1): Just add 1 and modulo N.
            // If i = 4 (the end), (4 + 1) % 5 = 0 (wraps to start).
            int right = (i + 1) % N;
            
            // To go LEFT (-1): Add (N - 1) and modulo N.
            // Why not just (i - 1) % N? Because in C++, the modulo of a negative 
            // number is negative! (e.g., -1 % 5 = -1). 
            // Adding N ensures the numerator is always strictly positive.
            int left  = (i - 1 + N) % N; 

            // Calculate the sum of the neighbors
            out(i) = in(left) + in(right);
        });
        Kokkos::fence();

        // Verify the wrap-around
        auto h_out = Kokkos::create_mirror_view(out);
        auto h_in  = Kokkos::create_mirror_view(in);
        Kokkos::deep_copy(h_out, out);
        Kokkos::deep_copy(h_in, in);

        printf("Input array:  [ ");
        for(int i=0; i<N; ++i) printf("%.1f ", h_in(i));
        printf("]\n");

        printf("Output array: [ ");
        for(int i=0; i<N; ++i) printf("%.1f ", h_out(i));
        printf("]\n\n");

        printf("Notice index 0: Left neighbor is 4.0, Right is 1.0. Sum = 5.0!\n");
        printf("Notice index 4: Left neighbor is 3.0, Right is 0.0. Sum = 3.0!\n");

        
    }
    Kokkos::finalize();
    return 0;
}
