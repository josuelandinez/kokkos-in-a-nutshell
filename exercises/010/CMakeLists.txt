cmake_minimum_required(VERSION 3.22)
project(KokkosOverlap)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../deps CACHE PATH "" FORCE)

FetchContent_Declare(
  Kokkos
  URL https://github.com/kokkos/kokkos/archive/refs/tags/5.1.0.zip
)
FetchContent_MakeAvailable(Kokkos)

add_executable(overlap_streams overlap_streams.cpp)
target_link_libraries(overlap_streams Kokkos::kokkos)
