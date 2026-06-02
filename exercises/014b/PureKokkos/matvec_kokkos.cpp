#include <mpi.h>
#include <Kokkos_Core.hpp>
#include <cstdio>
#include <chrono>

int main(int argc, char* argv[]) {
    // MPI initializes first
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Kokkos::initialize(argc, argv);
    {
        int global_N = 32768; 
        if (argc > 1) global_N = std::atoi(argv[1]);

        // Calculate local workload per rank
        int local_N = global_N / size; 

        if (rank == 0) {
            printf("--- Seamless MPI Kokkos MatVec ---\n");
            printf("Global N: %d | Ranks: %d | Local Rows/Rank: %d\n", global_N, size, local_N);
        }

        using MemSpace = Kokkos::DefaultExecutionSpace::memory_space;

        Kokkos::View<double**, MemSpace> A_local("A", local_N, global_N);
        Kokkos::View<double*, MemSpace> x_global("x", global_N);
        Kokkos::View<double*, MemSpace> y_local("y", local_N);

        Kokkos::parallel_for("Init", local_N, KOKKOS_LAMBDA(const int i) {
            for(int j = 0; j < global_N; ++j) {
                A_local(i, j) = 1.0;
                x_global(j) = 1.0; 
            }
        });
        Kokkos::fence();
        MPI_Barrier(MPI_COMM_WORLD);

        int bench_iters = 50;
        
        // WARMUP
        for(int k = 0; k < 5; ++k) { 
            Kokkos::parallel_for("MatVec", local_N, KOKKOS_LAMBDA(const int i) {
                double row_sum = 0.0;
                for(int j = 0; j < global_N; ++j) row_sum += A_local(i, j) * x_global(j);
                y_local(i) = row_sum;
            });
        }
        Kokkos::fence();
        MPI_Barrier(MPI_COMM_WORLD);

        // BENCHMARK
        auto start_bench = std::chrono::high_resolution_clock::now();
        
        for(int k = 0; k < bench_iters; ++k) {
            Kokkos::parallel_for("MatVec", local_N, KOKKOS_LAMBDA(const int i) {
                double row_sum = 0.0;
                for(int j = 0; j < global_N; ++j) row_sum += A_local(i, j) * x_global(j);
                y_local(i) = row_sum;
            });
        }
        Kokkos::fence(); 
        MPI_Barrier(MPI_COMM_WORLD); 
        
        auto end_bench = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_bench = end_bench - start_bench;
        double avg_time = diff_bench.count() / bench_iters;
        
        // Math: local elements read/written
        double local_bytes = (double)(local_N * global_N + global_N + local_N) * sizeof(double); 
        double local_bw = (local_bytes / avg_time) / 1e9; 

        // Aggregate across all ranks
        double global_bw = 0.0;
        MPI_Reduce(&local_bw, &global_bw, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("-> Average Time:             %f seconds/call\n", avg_time);
            printf("-> Aggregate Node Bandwidth: %f GB/s\n\n", global_bw);
        }
    }
    
    Kokkos::finalize();
    MPI_Finalize();
    return 0;
}
