#include <Kokkos_Core.hpp>
#include <cstdio>

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        int N = 10;

        // Allocate 2D Views for the input and output grids
        Kokkos::View<double**> in("InputGrid", N, N);
        Kokkos::View<double**> out("OutputGrid", N, N);

        // Initialize the grid with a 1D loop (just for setup)
        Kokkos::parallel_for("Init", N * N, KOKKOS_LAMBDA(const int i) {
            int x = i % N;
            int y = i / N;
            in(x, y) = 1.0;
        });
        Kokkos::fence();

        printf("--- Applying 2D Laplace Stencil ---\n");

        // The MDRangePolicy
        // We define a 2D Rank policy. 
        // We set the start bounds to {1, 1} and the end bounds to {N-1, N-1}.
        // This forces the loop to ONLY run on the "interior" of the grid, 
        // preventing Out-Of-Bounds memory errors!
        using MDRank2 = Kokkos::MDRangePolicy<Kokkos::Rank<2>>;
        MDRank2 policy({1, 1}, {N - 1, N - 1});

        // The 2D Parallel For
        // Notice the KOKKOS_LAMBDA now takes TWO arguments: (x, y)
        Kokkos::parallel_for("Laplace2D", policy, 
            KOKKOS_LAMBDA(const int x, const int y) {
                
                // 5-Point discrete Laplace operator
                double center = in(x, y);
                double up     = in(x, y + 1);
                double down   = in(x, y - 1);
                double left   = in(x - 1, y);
                double right  = in(x + 1, y);

                out(x, y) = up + down + left + right - (4.0 * center);
            }
        );
        Kokkos::fence();

        // Verify a single point
        auto h_out = Kokkos::create_mirror_view(out);
        Kokkos::deep_copy(h_out, out);
        
        printf("Laplace at (5,5): %f (Expected 0.0 since grid is uniform)\n", h_out(5,5));
        
    }
    Kokkos::finalize();
    return 0;
}
