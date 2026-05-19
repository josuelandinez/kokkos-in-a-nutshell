#include <Kokkos_Core.hpp>
#include <Kokkos_Timer.hpp>
#include <cstdio>

// A computationally heavy kernel so we can actually measure the time difference
struct ComputationalKernel {
    Kokkos::View<double*> data;
    
    ComputationalKernel(Kokkos::View<double*> d) : data(d) {}

    KOKKOS_INLINE_FUNCTION
    void operator()(const int i) const {
        double sum = 0.0;
        // Artificial work loop
        for(int j = 0; j < 50000; ++j) {
            sum += Kokkos::sin((double)(i + j));
        }
        data(i) = sum;
    }
};

int main(int argc, char* argv[]) {
    Kokkos::initialize(argc, argv);
    {
        int N = 10000;
        Kokkos::View<double*> data1("data1", N);
        Kokkos::View<double*> data2("data2", N);

        // Create TWO distinct Execution Space Instances.
        // On GPUs (CUDA/HIP), these map to independent hardware streams!
        Kokkos::DefaultExecutionSpace streamA;
        Kokkos::DefaultExecutionSpace streamB;

        Kokkos::Timer timer;

        printf("--- Testing Execution Space Instances (Streams) ---\n\n");

        // =======================================================
        // TEST 1: SEQUENTIAL EXECUTION (Same Stream)
        // =======================================================
        timer.reset();
        
        // Launch both on streamA. Kernel 2 must wait for Kernel 1 to finish.
        Kokkos::parallel_for("Seq_1", Kokkos::RangePolicy<>(streamA, 0, N), ComputationalKernel(data1));
        Kokkos::parallel_for("Seq_2", Kokkos::RangePolicy<>(streamA, 0, N), ComputationalKernel(data2));
        
        streamA.fence(); // Wait for streamA to finish
        
        double time_seq = timer.seconds();
        printf("Sequential Execution Time : %f seconds\n", time_seq);


        // =======================================================
        // TEST 2: CONCURRENT EXECUTION (Different Streams)
        // =======================================================
        timer.reset();
        
        // Launch on streamA and streamB. They will execute at the exact same time!
        Kokkos::parallel_for("Async_1", Kokkos::RangePolicy<>(streamA, 0, N), ComputationalKernel(data1));
        Kokkos::parallel_for("Async_2", Kokkos::RangePolicy<>(streamB, 0, N), ComputationalKernel(data2));
        
        // Fence both streams
        streamA.fence();
        streamB.fence();
        
        double time_async = timer.seconds();
        printf("Concurrent Execution Time : %f seconds\n", time_async);

        // =======================================================
        
        printf("\nSpeedup: %f x\n", time_seq / time_async);
    }
    Kokkos::finalize();
    return 0;
}
