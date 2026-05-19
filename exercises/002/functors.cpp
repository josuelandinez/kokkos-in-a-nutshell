#include <Kokkos_Core.hpp>
#include <cstdio>

// A classical C++ Functor
// It's a struct that overloads operator() so it can be "called" like a function.
struct MultiplyFunctor {
    double factor; // This is the "State"

    // Constructor initializes the state
    MultiplyFunctor(double f) : factor(f) {} 

    // KOKKOS_INLINE_FUNCTION tags this so the compiler generates CPU/GPU code for it
    KOKKOS_INLINE_FUNCTION
    void operator()(const int i) const {
        printf("Functor thread %d: %d * %f = %f\n", i, i, factor, i * factor);
    }
};

int main(int argc, char* argv[]) {
  
  // Initialize Kokkos Execution
    Kokkos::initialize(argc, argv);
    { // <-- SCOPE BRACES: All Kokkos objects must be destroyed before Kokkos::finalize()
        double factor = 2.5;

        printf("--- Using an explicit Functor ---\n");
        // We instantiate the struct and pass it to Kokkos.
        // "FunctorLoop" is a string name used for debugging, profiling, and Auto-Tuning (KTune).
        MultiplyFunctor my_functor(factor);
        Kokkos::parallel_for("FunctorLoop", 5, my_functor);
        
        // A fence is a Kokkos barrier. It waits for the GPU/threads to finish asynchronous work.
        Kokkos::fence();

        printf("\n--- Using a Lambda ---\n");
        //A Lambda function doing the exact same thing!
        // KOKKOS_LAMBDA is a macro that expands to: [=] KOKKOS_INLINE_FUNCTION
        // The [=] forces 'factor' to be captured by VALUE. 
        // WARNING: Capturing by reference [&] and sending to a GPU will cause a Segmentation Fault!
        Kokkos::parallel_for("LambdaLoop", 5, KOKKOS_LAMBDA(const int i) {
            printf("Lambda thread %d: %d * %f = %f\n", i, i, factor, i * factor);
        });
        
        Kokkos::fence();
    }
    Kokkos::finalize();
    return 0;
}
