#include <Kokkos_Core.hpp> // Core Kokkos library containing execution spaces and parallel patterns
#include <cstdio>          // Using cstdio because std::cout is not supported on most GPUs

int main(int argc, char* argv[]) {
  //Initialize the Kokkos execution environment.
  // This reads command-line arguments (like --kokkos-threads) and boots up the hardware (CPU/GPU).
  Kokkos::initialize(argc, argv);
  
  // The Scoping Block { ... }
  // This is a critical Kokkos practice not the best but practical.
  //We do it here for simplicity
  //All Kokkos objects (like Views) must be
  // destroyed BEFORE Kokkos::finalize() is called. Putting your math inside this scope 
  // ensures everything is automatically cleaned up when the scope ends.
  {
    // The Parallel Loop
    // Arg 1: "HelloWorld"  -> A string label used by Kokkos profiling tools (like NVTX or VTune).
    // Arg 2: 10            -> The iteration count (runs from index 0 to 9).
    // Arg 3: KOKKOS_LAMBDA -> A macro that makes the lambda portable. It automatically 
    //                         captures variables by value and marks the function so it can 
    //                         run on both the Host (CPU) and the Device (GPU).
    Kokkos::parallel_for("HelloWorld", 10, KOKKOS_LAMBDA(const int i) {
      
      // Inside a Kokkos kernel, always use printf. Standard C++ streams (std::cout) 
      // are not thread-safe and will fail to compile or crash on device architectures.
      printf("Hello from thread %d\n", i);
      
    });
  }
  
  // Tear down the Kokkos environment.
  // Safely shuts down the hardware execution spaces and frees internal memory.
  Kokkos::finalize();
  
  return 0;
}
