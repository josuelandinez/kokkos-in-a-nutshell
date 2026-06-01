cmake_minimum_required(VERSION 3.22)
project(KokkosMatVec)

# Silence the timestamp warning from FetchContent
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# Route all downloads to the shared 'deps' folder two levels up
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../deps CACHE PATH "" FORCE)

# Fetch Kokkos
FetchContent_Declare(
  Kokkos
  URL https://github.com/kokkos/kokkos/archive/refs/tags/5.1.0.zip
)
FetchContent_MakeAvailable(Kokkos)


# Build the executable
add_executable(matvec_kokkos matvec_kokkos.cpp)

# Link ONLY Kokkos 
target_link_libraries(matvec_kokkos PRIVATE Kokkos::kokkos)

# Add the KTune include paths (This is all a header-only library needs)
target_include_directories(matvec_kokkos PRIVATE 
    ${ktune_SOURCE_DIR}/include
    ${ktune_BINARY_DIR}/include
    ${ktune_BINARY_DIR} 
)
