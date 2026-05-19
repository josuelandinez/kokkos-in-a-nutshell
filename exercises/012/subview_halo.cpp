#include <Kokkos_Core.hpp>
#include <cstdio>

// =========================================================================
// GENERIC 1D FUNCTION
// This function knows absolutely nothing about 2D grids, halos, or strides.
// It just takes a 1D View and applies math to it. This mimics an abstract
// physics routine or an MPI buffer packer.
// =========================================================================
template <typename View1D>
void apply_boundary_flux(View1D edge_buffer, double flux_value) {
    Kokkos::parallel_for("ApplyFlux", edge_buffer.extent(0), KOKKOS_LAMBDA(int i) {
        edge_buffer(i) += flux_value;
    });
}

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        int n = 8, halo = 1, N = n + 2 * halo;
        Kokkos::View<double**> grid("grid", N, N);

        // Initialize grid to 1.0
        Kokkos::parallel_for("Init", N * N, KOKKOS_LAMBDA(int i) {
            grid(i / N, i % N) = 1.0; 
        });
        Kokkos::fence();

        printf("--- Kokkos Subview Demonstration ---\n");

        // EXTRACTING A COLUMN (The Right Halo)
        // Kokkos::ALL means "take the entire dimension"
        auto right_halo = Kokkos::subview(grid, Kokkos::ALL, n);

        // EXTRACTING A ROW (The Bottom Halo)
        auto bottom_halo = Kokkos::subview(grid, n, Kokkos::ALL);

        // APPLYING THE ABSTRACTION
        // We pass our 1D subviews directly into the generic 1D function!
        // The compiler automatically handles the fact that 'right_halo' skips memory 
        // (strided) and 'bottom_halo' is contiguous.
        apply_boundary_flux(right_halo, 5.0);
        apply_boundary_flux(bottom_halo, 9.0);
        Kokkos::fence();

        // VERIFICATION
        auto h_grid = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), grid);
        
        printf("Value at right boundary  (4, %d) is: %f (Expected 1.0 + 5.0 = 6.0)\n", n, h_grid(4, n));
        printf("Value at bottom boundary (%d, 4) is: %f (Expected 1.0 + 9.0 = 10.0)\n", n, h_grid(n, 4));
        
        // Note: The corner overlaps!
        printf("Value at the corner      (%d, %d) is: %f (Expected 1.0 + 5.0 + 9.0 = 15.0)\n", n, n, h_grid(n, n));
    }
    Kokkos::finalize();
    return 0;
}
