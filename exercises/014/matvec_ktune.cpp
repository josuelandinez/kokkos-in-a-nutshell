#include <Kokkos_Core.hpp>
#include <numeric>

// =========================================================================
// KTUNE AUTO-TUNING SETUP
// KTune requires a version hash so it knows if the underlying code has
// changed since the last time it generated a cache file. We define this
// macro before including the header.
// =========================================================================
#define KTUNE_GIT_COMMIT_HASH "Standalone"
#include <KTune/KTune.hpp>

#include <cstdio>
#include <chrono>

int main(int argc, char* argv[]) {
    // Boot up both frameworks
    Kokkos::initialize(argc, argv);
    
    // Initialize the auto-tuning library. It looks for 'ktune_cache.dat'
    // in the current directory. If not found, it prepares to tune.
    KTune::initialize(); // <-- FIXED: Takes zero arguments

    {
        int N = 8192; 
        Kokkos::View<double**> A("A", N, N);
        Kokkos::View<double*> x("x", N);
        Kokkos::View<double*> y("y", N);

        // Initialize data
        Kokkos::parallel_for("Init", N, KOKKOS_LAMBDA(int i) {
            x(i) = 1.0;
            for(int j = 0; j < N; ++j) {
                A(i, j) = 1.0;
            }
        });
        Kokkos::fence();

        printf("--- Starting MatVec KTune Benchmark ---\n");

        // =========================================================================
        // THE SEARCH PHASE (The 1st Call)
        // =========================================================================
        // The very first time KTune sees the label "Tuned_MatVec", it triggers the
        // search algorithm. It runs the kernel dozens of times in the background
        // using different tile sizes/thread counts to find the hardware limits. 
        // This first call will take significantly longer than a normal execution!
        
        auto start_tune = std::chrono::high_resolution_clock::now();
        
        KTune::parallel_for("Tuned_MatVec", Kokkos::RangePolicy<>(0, N), 
            KOKKOS_LAMBDA(const int i) {
                double row_sum = 0.0;
                for(int j = 0; j < N; ++j) {
                    row_sum += A(i, j) * x(j);
                }
                y(i) = row_sum; 
            });
        Kokkos::fence();
        
        auto end_tune = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_tune = end_tune - start_tune;
        printf("KTune Search Time (1st call): %f seconds\n", diff_tune.count());


        // =========================================================================
        // WARMUP PHASE
        // =========================================================================
        // Now that KTune has cached the optimal parameters, subsequent calls are
        // fast. We run a few un-timed iterations to "wake up" the CPU/GPU, ensuring
        // clock speeds are maxed out and hardware caches are fully populated.
        
        int warmup_iters = 5;
        auto start_warm = std::chrono::high_resolution_clock::now();
        
        for(int k = 0; k < warmup_iters; ++k) {
            KTune::parallel_for("Tuned_MatVec", Kokkos::RangePolicy<>(0, N), 
                KOKKOS_LAMBDA(const int i) {
                    double row_sum = 0.0;
                    for(int j = 0; j < N; ++j) row_sum += A(i, j) * x(j);
                    y(i) = row_sum;
                });
        }
        Kokkos::fence();
        
        auto end_warm = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_warm = end_warm - start_warm;
        printf("Warmup Time (%d calls):       %f seconds\n", warmup_iters, diff_warm.count());


        // =========================================================================
        // BENCHMARK PHASE
        // =========================================================================
        // Finally, we measure the steady-state performance of the optimized kernel.
        
        int bench_iters = 50;
        auto start_bench = std::chrono::high_resolution_clock::now();
        
        for(int k = 0; k < bench_iters; ++k) {
            KTune::parallel_for("Tuned_MatVec", Kokkos::RangePolicy<>(0, N), 
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

        printf("Benchmark Time (%d calls):    %f seconds\n", bench_iters, diff_bench.count());
        printf("======================================\n");
        printf("-> Average MatVec Time:          %f seconds/call\n", avg_time);
        printf("-> Effective Memory Bandwidth:   %f GB/s\n\n", bandwidth);
    }
    
    // Shut down Kokkos (KTune cleans itself up automatically)
    Kokkos::finalize();
    return 0;
}
