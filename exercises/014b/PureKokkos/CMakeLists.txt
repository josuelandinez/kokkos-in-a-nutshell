cmake_minimum_required(VERSION 3.22)
project(KokkosMPIMatVec)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# Find MPI
find_package(MPI REQUIRED)

include(FetchContent)

# Fetch Kokkos
FetchContent_Declare(
  Kokkos
  URL https://github.com/kokkos/kokkos/archive/refs/tags/5.1.0.zip
)
FetchContent_MakeAvailable(Kokkos)

# Build Pure Kokkos Executable
add_executable(matvec_kokkos matvec_kokkos.cpp)
target_link_libraries(matvec_kokkos PRIVATE Kokkos::kokkos MPI::MPI_CXX)
