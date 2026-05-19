#include <Kokkos_Core.hpp>
#include <cstdio>

// The Complex Type Problem
// We MUST use Kokkos::complex because std::complex is not guaranteed 
// to work on GPUs (it lacks __device__ compiler tags).
using cplx = Kokkos::complex<double>;

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    { // Scope braces
        int N = 4;

        // Multi-Dimensional Views
        // The ** means this is a Rank-2 (2D) View.
        // NOTE: Kokkos automatically handles the memory layout:
        // - On CPU, it defaults to Row-Major (LayoutRight)
        // - On GPU, it defaults to Column-Major (LayoutLeft) for faster memory coalescing!
        Kokkos::View<cplx**> d_matrix("DeviceMatrix", N, N);

        // Filling a 2D View using a 1D loop
        // Since we are only filling the diagonal, we only need to loop from 0 to N-1.
        Kokkos::parallel_for("FillDiag", N, KOKKOS_LAMBDA(const int i) {
            // Accessing the 2D view uses parenthesis (i, j), NOT brackets [i][j]
            d_matrix(i, i) = cplx(1.0, 1.5); // Real = 1.0, Imag = 1.5
        });
        Kokkos::fence();

        // Remember Ex 004! To print it, we MUST copy it to the Host!
        auto h_matrix = Kokkos::create_mirror_view(d_matrix);
        Kokkos::deep_copy(h_matrix, d_matrix);

        printf("--- 4x4 Complex Diagonal Matrix ---\n");
        for(int i = 0; i < N; ++i) {
            for(int j = 0; j < N; ++j) {
                // Extract real and imaginary parts
                double r = h_matrix(i, j).real();
                double im = h_matrix(i, j).imag();

		// formatting print
                if (r == 0.0 && im == 0.0) {
                    printf("( 0.0,  0.0) ");
                } else {
                    printf("(%.1f, %.1f) ", r, im);
                }
            }
            printf("\n");
        }
        
        
    }
    Kokkos::finalize();
    return 0;
}
