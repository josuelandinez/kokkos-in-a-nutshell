cmake_minimum_required(VERSION 3.22)
project(HaloExchangeKTune)

# Silence the timestamp warning from FetchContent
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# Shared deps
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps CACHE PATH "" FORCE)

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
  GIT_TAG main
)
FetchContent_MakeAvailable(KTune)

# Find MPI
find_package(MPI REQUIRED)

# Build the executable
add_executable(halo_ktune halo_ktune.cpp)

# Link Kokkos and MPI
target_link_libraries(halo_ktune PRIVATE Kokkos::kokkos MPI::MPI_CXX)

# Add KTune Headers
target_include_directories(halo_ktune PRIVATE 
    ${ktune_SOURCE_DIR}/include
    ${ktune_BINARY_DIR}/include
    ${ktune_BINARY_DIR} 
)
