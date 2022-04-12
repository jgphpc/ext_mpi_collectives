#ifndef EXT_MPI_READ_H_

#define EXT_MPI_READ_H_

#define MAX_BUFFER_SIZE 100000000
//#define CACHE_LINE_SIZE 64
#define CACHE_LINE_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

enum ecollective_type {
  collective_type_allgatherv,
  collective_type_reduce_scatter,
  collective_type_allreduce,
  collective_type_allreduce_group,
  collective_type_allreduce_short
};
enum edata_type {
  data_type_char,
  data_type_int,
  data_type_long_int,
  data_type_float,
  data_type_double
};
enum eassembler_type {
  eset_num_cores,
  eset_node_rank,
  enode_barrier,
  enode_cycl_barrier,
  eset_node_barrier,
  ewait_node_barrier,
  enext_node_barrier,
  eset_mem,
  eunset_mem,
  ememcpy,
  ememcp_,
  esmemcpy,
  esmemcp_,
  ereduce,
  ereduc_,
  esreduce,
  esreduc_,
  eirecv,
  eirec_,
  eisend,
  eisen_,
  ewaitall,
  ewaitany,
  eattached,
  esendbufp,
  erecvbufp,
  elocmemp,
  eshmemo,
  ecpbuffer_offseto,
  ecp,
  ereturn,
  enop,
  estage,
  estart
};

struct parameters_block {
  enum ecollective_type collective_type;
  int node;
  int num_nodes;
  int node_rank;
  int node_row_size;
  int node_column_size;
  int *counts;
  int counts_max;
  int *num_ports;
  int num_ports_max;
  int *groups;
  int groups_max;
  int *message_sizes;
  int message_sizes_max;
  int *rank_perm;
  int rank_perm_max;
  int *iocounts;
  int iocounts_max;
  int copy_method;
  enum edata_type data_type;
  int verbose;
  int in_place;
  int ascii_in, ascii_out;
  int bit_identical;
  int locmem_max;
  int shmem_max;
  int *shmem_buffer_offset;
  int shmem_buffer_offset_max;
  int root;
  int blocking;
  int on_gpu;
};

struct data_line {
  int frac;
  int source;
  int to_max;
  int *to;
  int from_max;
  int *from_node;
  int *from_line;
};

struct line_irecv_isend {
  enum eassembler_type type;
  enum eassembler_type buffer_type;
  int buffer_number;
  int is_offset;
  int offset_number;
  int offset;
  int size, partner, tag;
};

struct line_memcpy_reduce {
  enum eassembler_type type;
  enum eassembler_type buffer_type1;
  int buffer_number1;
  int is_offset1;
  int offset_number1;
  int offset1;
  enum eassembler_type buffer_type2;
  int buffer_number2;
  int is_offset2;
  int offset_number2;
  int offset2;
  int size;
};

int ext_mpi_read_parameters(char *buffer_in,
                            struct parameters_block **parameters);
int ext_mpi_write_parameters(struct parameters_block *parameters,
                             char *buffer_out);
int ext_mpi_delete_parameters(struct parameters_block *parameters);
int ext_mpi_read_algorithm(char *buffer_in, int *size_level0, int **size_level1,
                           struct data_line ***data, int ascii_in);
int ext_mpi_write_algorithm(int size_level0, int *size_level1,
                            struct data_line **data, char *buffer_out,
                            int ascii_out);
void ext_mpi_delete_algorithm(int size_level0, int *size_level1,
                              struct data_line **data);
int ext_mpi_read_line(char *buffer_in, char *line, int ascii);
int ext_mpi_write_line(char *buffer_out, char *line, int ascii);
int ext_mpi_write_eof(char *buffer_out, int ascii);
int ext_mpi_write_assembler_line_s(char *buffer_out,
                                   enum eassembler_type string1, int ascii);
int ext_mpi_write_assembler_line_sd(char *buffer_out,
                                    enum eassembler_type string1, int integer1,
                                    int ascii);
int ext_mpi_write_assembler_line_sdsd(char *buffer_out,
                                      enum eassembler_type string1,
                                      int integer1,
                                      enum eassembler_type string2,
                                      int integer2, int ascii);
int ext_mpi_write_assembler_line_sddsd(char *buffer_out,
                                       enum eassembler_type string1,
                                       int integer1, int integer2,
                                       enum eassembler_type string2,
                                       int integer3, int ascii);
int ext_mpi_write_assembler_line_ssd(char *buffer_out,
                                     enum eassembler_type string1,
                                     enum eassembler_type string2,
                                     int integer1, int ascii);
int ext_mpi_write_assembler_line_ssdsd(char *buffer_out,
                                       enum eassembler_type string1,
                                       enum eassembler_type string2,
                                       int integer1,
                                       enum eassembler_type string3,
                                       int integer2, int ascii);
int ext_mpi_write_assembler_line_ssdsdd(char *buffer_out,
                                        enum eassembler_type string1,
                                        enum eassembler_type string2,
                                        int integer1,
                                        enum eassembler_type string3,
                                        int integer2, int integer3, int ascii);
int ext_mpi_write_assembler_line_ssdddd(char *buffer_out,
                                        enum eassembler_type string1,
                                        enum eassembler_type string2,
                                        int integer1, int integer2,
                                        int integer3, int integer4, int ascii);
int ext_mpi_write_assembler_line_ssddd(char *buffer_out,
                                       enum eassembler_type string1,
                                       enum eassembler_type string2,
                                       int integer1, int integer2, int integer3,
                                       int ascii);
int ext_mpi_write_assembler_line_ssdsdddd(
    char *buffer_out, enum eassembler_type string1,
    enum eassembler_type string2, int integer1, enum eassembler_type string3,
    int integer2, int integer3, int integer4, int integer5, int ascii);
int ext_mpi_write_assembler_line_ssdsdsdsdd(
    char *buffer_out, enum eassembler_type string1,
    enum eassembler_type string2, int integer1, enum eassembler_type string3,
    int integer2, enum eassembler_type string4, int integer3,
    enum eassembler_type string5, int integer4, int integer5, int ascii);
int ext_mpi_write_assembler_line_ssdsdsdd(
    char *buffer_out, enum eassembler_type string1,
    enum eassembler_type string2, int integer1, enum eassembler_type string3,
    int integer2, enum eassembler_type string4, int integer3, int integer4, int ascii);
int ext_mpi_read_assembler_line_s(char *buffer_in,
                                  enum eassembler_type *string1, int ascii);
int ext_mpi_read_assembler_line_sd(char *buffer_in,
                                   enum eassembler_type *string1, int *integer1,
                                   int ascii);
int ext_mpi_read_assembler_line_sdsd(char *buffer_in,
                                     enum eassembler_type *string1,
                                     int *integer1,
                                     enum eassembler_type *string2,
                                     int *integer2, int ascii);
int ext_mpi_read_assembler_line_sddsd(char *buffer_in,
                                      enum eassembler_type *string1,
                                      int *integer1, int *integer2,
                                      enum eassembler_type *string2,
                                      int *integer3, int ascii);
int ext_mpi_read_assembler_line_ssd(char *buffer_in,
                                    enum eassembler_type *string1,
                                    enum eassembler_type *string2,
                                    int *integer1, int ascii);
int ext_mpi_read_assembler_line_ssdsd(char *buffer_in,
                                      enum eassembler_type *string1,
                                      enum eassembler_type *string2,
                                      int *integer1,
                                      enum eassembler_type *string3,
                                      int *integer2, int ascii);
int ext_mpi_read_assembler_line_ssdsdd(char *buffer_in,
                                       enum eassembler_type *string1,
                                       enum eassembler_type *string2,
                                       int *integer1,
                                       enum eassembler_type *string3,
                                       int *integer2, int *integer3, int ascii);
int ext_mpi_read_assembler_line_ssdddd(char *buffer_in,
                                       enum eassembler_type *string1,
                                       enum eassembler_type *string2,
                                       int *integer1, int *integer2,
                                       int *integer3, int *integer4, int ascii);
int ext_mpi_read_assembler_line_ssddd(char *buffer_in,
                                      enum eassembler_type *string1,
                                      enum eassembler_type *string2,
                                      int *integer1, int *integer2,
                                      int *integer3, int ascii);

int ext_mpi_read_assembler_line_ssdsdddd(
    char *buffer_in, enum eassembler_type *string1,
    enum eassembler_type *string2, int *integer1, enum eassembler_type *string3,
    int *integer2, int *integer3, int *integer4, int *integer5, int ascii);
int ext_mpi_read_assembler_line_ssdsdsdd(
    char *buffer_in, enum eassembler_type *string1,
    enum eassembler_type *string2, int *integer1, enum eassembler_type *string3,
    int *integer2, enum eassembler_type *string4, int *integer3, int *integer4, int ascii);
int ext_mpi_read_assembler_line_ssdsdsdsdd(
    char *buffer_in, enum eassembler_type *string1,
    enum eassembler_type *string2, int *integer1, enum eassembler_type *string3,
    int *integer2, enum eassembler_type *string4, int *integer3,
    enum eassembler_type *string5, int *integer4, int *integer5, int ascii);
int ext_mpi_switch_to_ascii(char *buffer);

#ifdef __cplusplus
}
#endif

#endif
