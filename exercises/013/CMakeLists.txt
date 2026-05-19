cmake_minimum_required(VERSION 3.22)
project(HaloExchange)

# Shared deps — same pattern as other exercises
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps CACHE PATH "" FORCE)

include(FetchContent)
FetchContent_Declare(
  Kokkos
  URL https://github.com/kokkos/kokkos/archive/refs/tags/5.1.0.zip
)
FetchContent_MakeAvailable(Kokkos)

# MPI is required
find_package(MPI REQUIRED)

add_executable(halo_exchange_subview halo_exchange_subview.cpp)
target_link_libraries(halo_exchange_subview Kokkos::kokkos MPI::MPI_CXX)
