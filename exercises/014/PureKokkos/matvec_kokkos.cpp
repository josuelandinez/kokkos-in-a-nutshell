#include <Kokkos_Core.hpp>
#include <cstdio>
#include <chrono>

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    
    {
        // Default size, can be overridden by command line arg if desired
        int N = 8192; 
        if (argc > 1) N = std::atoi(argv[1]);

        Kokkos::View<double**> A("A", N, N);
        Kokkos::View<double*> x("x", N);
        Kokkos::View<double*> y("y", N);

        Kokkos::parallel_for("Init", N, KOKKOS_LAMBDA(int i) {
            x(i) = 1.0;
            for(int j = 0; j < N; ++j) {
                A(i, j) = 1.0;
            }
        });
        Kokkos::fence();

        printf("--- Starting Pure Kokkos MatVec Benchmark (N=%d) ---\n", N);

        // Warmup Phase (Initializes JIT, caches, and page tables)
        int warmup_iters = 5;
        for(int k = 0; k < warmup_iters; ++k) {
            Kokkos::parallel_for("MatVec", Kokkos::RangePolicy<>(0, N), 
                KOKKOS_LAMBDA(const int i) {
                    double row_sum = 0.0;
                    for(int j = 0; j < N; ++j) row_sum += A(i, j) * x(j);
                    y(i) = row_sum;
                });
        }
        Kokkos::fence();

        // Benchmark Phase
        int bench_iters = 50;
        auto start_bench = std::chrono::high_resolution_clock::now();
        
        for(int k = 0; k < bench_iters; ++k) {
            Kokkos::parallel_for("MatVec", Kokkos::RangePolicy<>(0, N), 
                KOKKOS_LAMBDA(const int i) {
                    double row_sum = 0.0;
                    for(int j = 0; j < N; ++j) row_sum += A(i, j) * x(j);
                    y(i) = row_sum;
                });
        }
        Kokkos::fence();
        
        auto end_bench = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_bench = end_bench - start_bench;
        
        double avg_time = diff_bench.count() / bench_iters;
        
        // Memory Bandwidth Math: Read Matrix A (N*N) + Read Vector x (N) + Write Vector y (N)
        double bytes_moved = (double)(N * N + N + N) * sizeof(double); 
        double bandwidth = (bytes_moved / avg_time) / 1e9; // GB/s

        printf("-> Average MatVec Time:          %f seconds/call\n", avg_time);
        printf("-> Effective Memory Bandwidth:   %f GB/s\n\n", bandwidth);
    }
    
    Kokkos::finalize();
    return 0;
}
