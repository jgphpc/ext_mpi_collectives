#ifndef EXT_MPI_BYTE_CODE_H_

#define EXT_MPI_BYTE_CODE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct header_byte_code {
  int size_to_return;
  int barrier_counter_socket;
  int barrier_counter_node;
  char *barrier_shmem_socket;
  int barrier_shmem_size;
  volatile char **barrier_shmem_node;
  int *shmemid;
  char *locmem;
  char **shmem;
  int node_num_cores_row;
  int node_num_cores_column;
  int num_cores;
  int socket_rank;
  int node_sockets;
  int tag;
#ifdef GPU_ENABLED
  char **shmem_gpu;
  char *gpu_byte_code;
#endif
};

int ext_mpi_generate_byte_code(char **shmem,
                               int barrier_shmem_size, int *barrier_shmemid,
                               char *buffer_in, char *sendbuf, char *recvbuf,
                               int my_size_shared_buf, int barriers_size, char *locmem,
                               int reduction_op, int *global_ranks,
                               char *code_out, int size_comm, int size_request, void *comm_row,
                               int node_num_cores_row, void *comm_column,
                               int node_num_cores_column,
                               char **shmem_gpu, int *gpu_byte_code_counter, int tag);

#ifdef __cplusplus
}
#endif

#endif
