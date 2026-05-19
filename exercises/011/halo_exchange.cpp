#include <Kokkos_Core.hpp>
#include <mpi.h>
#include <iostream>

typedef Kokkos::View<double**> ViewType;
typedef Kokkos::View<double*>  BufferType;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    Kokkos::ScopeGuard kokkos(argc, argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n    = 8;
    int halo = 1;
    int N    = n + 2 * halo;

    ViewType grid("grid", N, N);

    // Initialize interior with rank value
    Kokkos::parallel_for("Init",
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({halo, halo}, {N-halo, N-halo}),
        KOKKOS_LAMBDA(int i, int j) {
            grid(i, j) = static_cast<double>(rank);
        });
    Kokkos::fence();

    // GPU buffers for packing/unpacking
    BufferType send_buf("send_buf", N);
    BufferType recv_buf("recv_buf", N);

    // Pack right boundary column into contiguous GPU buffer
    Kokkos::parallel_for("PackSend", N, KOKKOS_LAMBDA(int i) {
        send_buf(i) = grid(i, n);
    });
    Kokkos::fence();

    // Mirror to host — works with non-CUDA-aware MPI
    auto send_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), send_buf);
    auto recv_host = Kokkos::create_mirror_view(Kokkos::HostSpace(), recv_buf);

    int neighbor_left  = (rank == 0)      ? MPI_PROC_NULL : rank - 1;
    int neighbor_right = (rank == size-1) ? MPI_PROC_NULL : rank + 1;

    MPI_Request requests[2];
    // MPI talks to host memory — safe with any OpenMPI build
    MPI_Irecv(recv_host.data(), N, MPI_DOUBLE, neighbor_left,  0, MPI_COMM_WORLD, &requests[0]);
    MPI_Isend(send_host.data(), N, MPI_DOUBLE, neighbor_right, 0, MPI_COMM_WORLD, &requests[1]);

    // Overlap: compute on interior while MPI works
    Kokkos::parallel_for("InternalCompute",
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({halo, halo}, {N-halo, N-halo}),
        KOKKOS_LAMBDA(int i, int j) {
            grid(i, j) *= 2.0;
        });

    MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);

    // Copy received host buffer back to GPU, then unpack into ghost column
    Kokkos::deep_copy(recv_buf, recv_host);
    Kokkos::parallel_for("UnpackRecv", N, KOKKOS_LAMBDA(int i) {
        grid(i, 0) = recv_buf(i);
    });
    Kokkos::fence();

    // Verification
    auto grid_host = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), grid);
    if (rank == 0) {
        std::cout << "Rank " << rank << ": left ghost col[1] = " << grid_host(1, 0)
                  << " (expected 0, no left neighbor)" << std::endl;
    } else {
        std::cout << "Rank " << rank << ": left ghost col[1] = " << grid_host(1, 0)
                  << " (received from rank " << neighbor_left << ")" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
