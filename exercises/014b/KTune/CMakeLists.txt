cmake_minimum_required(VERSION 3.22)
project(KtuneMPIMatVec)

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

# Fetch KTune
FetchContent_Declare(
  KTune
  GIT_REPOSITORY https://gitlab.jsc.fz-juelich.de/numeriqs-b2/ktune.git
  GIT_TAG        release/1.0.0
)
FetchContent_MakeAvailable(KTune)

# Build KTune Executable
add_executable(matvec_ktune matvec_ktune.cpp)
target_link_libraries(matvec_ktune PRIVATE Kokkos::kokkos MPI::MPI_CXX)
target_include_directories(matvec_ktune PRIVATE 
    ${ktune_SOURCE_DIR}/include
    ${ktune_BINARY_DIR}/include
    ${ktune_BINARY_DIR} 
)
