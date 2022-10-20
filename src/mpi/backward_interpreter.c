#include "backward_interpreter.h"
#include "constants.h"
#include "read.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ext_mpi_generate_backward_interpreter(char *buffer_in, char *buffer_out,
                                  MPI_Comm comm_row) {
  int **values = NULL, **recv_values = NULL, max_lines;
  int nbuffer_out = 0, nbuffer_in = 0, i, j, k, l, size_level0 = 0,
      *size_level1 = NULL;
  struct data_line **data = NULL;
  struct parameters_block *parameters;
  MPI_Request *request = NULL;
  nbuffer_in += i = ext_mpi_read_parameters(buffer_in + nbuffer_in, &parameters);
  MPI_Allreduce(MPI_IN_PLACE, &i, 1, MPI_INT, MPI_MIN, comm_row);
  if (i < 0)
    goto error;
  nbuffer_out += ext_mpi_write_parameters(parameters, buffer_out + nbuffer_out);
  i = ext_mpi_read_algorithm(buffer_in + nbuffer_in, &size_level0, &size_level1, &data,
                             parameters->ascii_in);
  MPI_Allreduce(MPI_IN_PLACE, &i, 1, MPI_INT, MPI_MIN, comm_row);
  if (i == ERROR_MALLOC)
    goto error;
  if (i <= 0) {
    printf("error reading algorithm backward_interpreter\n");
    exit(2);
  }
  i = 0;
  values = (int **)malloc(sizeof(int *) * size_level0);
  if (!values)
    i = ERROR_MALLOC;
  MPI_Allreduce(MPI_IN_PLACE, &i, 1, MPI_INT, MPI_MIN, comm_row);
  if (i < 0)
    goto error;
  for (i = 0; i < size_level0; i++) {
    values[i] = NULL;
  }
  j = 0;
  for (i = 0; i < size_level0; i++) {
    values[i] = (int *)malloc(sizeof(int) * size_level1[i]);
    if (!values[i])
      j = ERROR_MALLOC;
  }
  MPI_Allreduce(MPI_IN_PLACE, &j, 1, MPI_INT, MPI_MIN, comm_row);
  if (j < 0)
    goto error;
  for (i = 0; i < size_level0; i++) {
    for (j = 0; j < size_level1[i]; j++) {
      values[i][j] = 0;
    }
  }
  for (i = 0; i < size_level1[size_level0 - 1]; i++) {
    for (j = 0; j < data[size_level0 - 1][i].to_max; j++) {
      if ((data[size_level0 - 1][i].to[j] == -1) &&
          (parameters->socket == parameters->root / parameters->socket_row_size)) {
        values[size_level0 - 1][i] = 1;
      }
    }
  }
  max_lines = 0;
  for (i = 0; i < size_level0; i++) {
    if (size_level1[i] > max_lines) {
      max_lines = size_level1[i];
    }
  }
  i = 0;
  request = (MPI_Request *)malloc(sizeof(MPI_Request) * max_lines * 2);
  if (!request)
    i = ERROR_MALLOC;
  MPI_Allreduce(MPI_IN_PLACE, &i, 1, MPI_INT, MPI_MIN, comm_row);
  if (i < 0)
    goto error;
  i = 0;
  recv_values = (int **)malloc(sizeof(int *) * max_lines);
  if (!recv_values)
    i = ERROR_MALLOC;
  MPI_Allreduce(MPI_IN_PLACE, &i, 1, MPI_INT, MPI_MIN, comm_row);
  if (i < 0)
    goto error;
  for (i = 0; i < max_lines; i++) {
    recv_values[i] = NULL;
  }
  j = 0;
  for (i = 0; i < max_lines; i++) {
    recv_values[i] = (int *)malloc(sizeof(int) * parameters->num_sockets);
    if (!recv_values[i])
      j = ERROR_MALLOC;
  }
  MPI_Allreduce(MPI_IN_PLACE, &j, 1, MPI_INT, MPI_MIN, comm_row);
  if (j < 0)
    goto error;
  for (i = size_level0 - 1; i >= 1; i--) {
    l = 0;
    for (j = size_level1[i - 1] - 1; j >= 0; j--) {
      for (k = 0; k < data[i - 1][j].to_max; k++) {
        if (data[i - 1][j].to[k] != parameters->socket) {
          MPI_Irecv(&recv_values[j][data[i - 1][j].to[k]], 1, MPI_INT,
                    data[i - 1][j].to[k] * parameters->socket_row_size +
                        parameters->socket_rank % parameters->socket_row_size,
                    0, comm_row, &request[l++]);
        }
      }
    }
    for (j = size_level1[i] - 1; j >= 0; j--) {
      for (k = 0; k < data[i][j].from_max; k++) {
        if (data[i][j].from_node[k] == parameters->socket) {
          if ((data[i][j].from_line[k] < j) && (data[i][j].from_line[k] >= 0)) {
            if (values[i][j]) {
              values[i][data[i][j].from_line[k]] = 1;
            }
          }
        } else {
          MPI_Isend(&values[i][j], 1, MPI_INT,
                    data[i][j].from_node[k] * parameters->socket_row_size +
                        parameters->socket_rank % parameters->socket_row_size,
                    0, comm_row, &request[l++]);
        }
      }
    }
    for (j = size_level1[i - 1] - 1; j >= 0; j--) {
      if (j < size_level1[i]) {
        if (values[i][j]) {
          values[i - 1][j] = 1;
        }
      }
    }
    MPI_Waitall(l, request, MPI_STATUSES_IGNORE);
    for (j = size_level1[i] - 1; j >= 0; j--) {
      for (k = 0; k < data[i][j].from_max; k++) {
        if (data[i][j].from_node[k] != parameters->socket) {
          if (!values[i][j]) {
            if (parameters->socket_row_size * parameters->socket_column_size == 1) {
              if (data[i][j].from_max == 1) {
                data[i][j].from_node[k] = parameters->socket;
                data[i][j].from_line[k] = j;
              } else {
                for (l = k; l < data[i][j].from_max - 1; l++) {
                  data[i][j].from_node[l] = data[i][j].from_node[l + 1];
                  data[i][j].from_line[l] = data[i][j].from_line[l + 1];
                }
                data[i][j].from_max--;
                k--;
              }
            } else {
              data[i][j].from_node[k] = -10 - data[i][j].from_node[k];
            }
          }
        }
      }
    }
    for (j = size_level1[i - 1] - 1; j >= 0; j--) {
      for (k = 0; k < data[i - 1][j].to_max; k++) {
        if (data[i - 1][j].to[k] != parameters->socket) {
          if (!recv_values[j][data[i - 1][j].to[k]]) {
            if (parameters->socket_row_size * parameters->socket_column_size == 1) {
              if (data[i - 1][j].to_max == 1) {
                data[i - 1][j].to[k] = parameters->socket;
              } else {
                for (l = k; l < data[i - 1][j].to_max - 1; l++) {
                  data[i - 1][j].to[l] = data[i - 1][j].to[l + 1];
                }
                data[i - 1][j].to_max--;
                k--;
              }
            } else {
              data[i - 1][j].to[k] = -10 - data[i - 1][j].to[k];
            }
          } else {
            values[i - 1][j] = 1;
          }
        }
      }
    }
  }
  nbuffer_out +=
      ext_mpi_write_algorithm(size_level0, size_level1, data, buffer_out + nbuffer_out,
                              parameters->ascii_out);
  nbuffer_out += ext_mpi_write_eof(buffer_out + nbuffer_out, parameters->ascii_out);
  free(request);
  for (i = max_lines - 1; i >= 0; i--) {
    free(recv_values[i]);
  }
  free(recv_values);
  for (i = size_level0 - 1; i >= 0; i--) {
    free(values[i]);
  }
  free(values);
  ext_mpi_delete_algorithm(size_level0, size_level1, data);
  ext_mpi_delete_parameters(parameters);
  return nbuffer_out;
error:
  free(request);
  if (recv_values) {
    for (i = max_lines - 1; i >= 0; i--) {
      free(recv_values[i]);
    }
  }
  free(recv_values);
  if (values) {
    for (i = size_level0 - 1; i >= 0; i--) {
      free(values[i]);
    }
  }
  free(values);
  ext_mpi_delete_algorithm(size_level0, size_level1, data);
  ext_mpi_delete_parameters(parameters);
  return ERROR_MALLOC;
}
