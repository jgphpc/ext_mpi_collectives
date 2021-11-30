#ifndef EXT_MPI_READ_H_

#define EXT_MPI_READ_H_

#define MAX_BUFFER_SIZE 100000000
#define CACHE_LINE_SIZE 64

#ifdef __cplusplus
extern "C" {
#endif

extern int ext_mpi_bit_identical;

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
  ememcpy,
  ememcp_,
  ereduce,
  ereduc_,
  esreduce,
  esreduc_,
  eirecv,
  eirec_,
  eisend,
  eisen_,
  ewaitall,
  eshmemp,
  esendbufp,
  erecvbufp,
  elocmemp,
  eshmempbuffer_offseto,
  eshmempbuffer_offsetcp,
  ereturn,
  enop,
  estage
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
  int ascii_in, ascii_out;
  int bit_identical;
  int locmem_max;
  int shmem_max;
  int *shmem_buffer_offset;
  int shmem_buffer_offset_max;
  int root;
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

int read_parameters(char *buffer_in, struct parameters_block **parameters);
int write_parameters(struct parameters_block *parameters, char *buffer_out);
int delete_parameters(struct parameters_block *parameters);
int read_algorithm(char *buffer_in, int *size_level0, int **size_level1,
                   struct data_line ***data, int ascii_in);
int write_algorithm(int size_level0, int *size_level1, struct data_line **data,
                    char *buffer_out, int ascii_out);
void delete_algorithm(int size_level0, int *size_level1,
                      struct data_line **data);
int read_line(char *buffer_in, char *line, int ascii);
int write_line(char *buffer_out, char *line, int ascii);
int write_eof(char *buffer_out, int ascii);
int write_assembler_line_s(char *buffer_out, enum eassembler_type string1,
                           int ascii);
int write_assembler_line_sd(char *buffer_out, enum eassembler_type string1,
                            int integer1, int ascii);
int write_assembler_line_sdsd(char *buffer_out, enum eassembler_type string1,
                              int integer1, enum eassembler_type string2,
                              int integer2, int ascii);
int write_assembler_line_ssdsdd(char *buffer_out, enum eassembler_type string1,
                                enum eassembler_type string2, int integer1,
                                enum eassembler_type string3, int integer2,
                                int integer3, int ascii);
int write_assembler_line_ssdddd(char *buffer_out, enum eassembler_type string1,
                                enum eassembler_type string2, int integer1,
                                int integer2, int integer3, int integer4,
                                int ascii);
int write_assembler_line_ssdsdddd(char *buffer_out,
                                  enum eassembler_type string1,
                                  enum eassembler_type string2, int integer1,
                                  enum eassembler_type string3, int integer2,
                                  int integer3, int integer4, int integer5,
                                  int ascii);
int write_assembler_line_ssdsdsdsdd(char *buffer_out,
                                    enum eassembler_type string1,
                                    enum eassembler_type string2, int integer1,
                                    enum eassembler_type string3, int integer2,
                                    enum eassembler_type string4, int integer3,
                                    enum eassembler_type string5, int integer4,
                                    int integer5, int ascii);
int read_assembler_line_s(char *buffer_in, enum eassembler_type *string1,
                          int ascii);
int read_assembler_line_sd(char *buffer_in, enum eassembler_type *string1,
                           int *integer1, int ascii);
int read_assembler_line_sdsd(char *buffer_in, enum eassembler_type *string1,
                             int *integer1, enum eassembler_type *string2,
                             int *integer2, int ascii);
int read_assembler_line_ssdsdd(char *buffer_in, enum eassembler_type *string1,
                               enum eassembler_type *string2, int *integer1,
                               enum eassembler_type *string3, int *integer2,
                               int *integer3, int ascii);
int read_assembler_line_ssdddd(char *buffer_in, enum eassembler_type *string1,
                               enum eassembler_type *string2, int *integer1,
                               int *integer2, int *integer3, int *integer4,
                               int ascii);
int read_assembler_line_ssdsdddd(char *buffer_in, enum eassembler_type *string1,
                                 enum eassembler_type *string2, int *integer1,
                                 enum eassembler_type *string3, int *integer2,
                                 int *integer3, int *integer4, int *integer5,
                                 int ascii);
int read_assembler_line_ssdsdsdsdd(char *buffer_in,
                                   enum eassembler_type *string1,
                                   enum eassembler_type *string2, int *integer1,
                                   enum eassembler_type *string3, int *integer2,
                                   enum eassembler_type *string4, int *integer3,
                                   enum eassembler_type *string5, int *integer4,
                                   int *integer5, int ascii);
int switch_to_ascii(char *buffer);

#ifdef __cplusplus
}
#endif

#endif