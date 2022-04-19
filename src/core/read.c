#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "ports_groups.h"
#include "read.h"

static int read_single_line(char *buffer_in, char *line) {
  int i;
  for (i = 0; (buffer_in[i] != '\n') && (buffer_in[i] != '\0'); i++) {
    line[i] = buffer_in[i];
  }
  line[i] = '\0';
  if (buffer_in[i] == '\0') {
    return i;
  } else {
    return i + 1;
  }
}

static int read_int_series(char *string_in, int **integers_out) {
  char *string_in_new;
  int i, i_max;
  if (!string_in) {
    return (-1);
  }
  while (*string_in == ' ') {
    string_in++;
  }
  string_in_new = string_in;
  i = 1;
  i_max = 0;
  while ((*string_in_new != '\0') && i) {
    while ((*string_in_new != ' ') && (*string_in_new != '\0') && i) {
      if (((*string_in_new >= '0') && (*string_in_new <= '9')) ||
          (*string_in_new == '-')) {
        string_in_new++;
      } else {
        i = 0;
      }
    }
    if (i) {
      while (*string_in_new == ' ') {
        string_in_new++;
      }
    } else {
      i_max--;
    }
    i_max++;
  }
  *integers_out = (int *)malloc((i_max + 1) * sizeof(int));
  if (!*integers_out)
    return ERROR_MALLOC;
  string_in_new = string_in;
  for (i = 0; i < i_max; i++) {
    if (sscanf(string_in_new, "%d", &(*integers_out)[i]) != 1) {
      return (-1);
    }
    while ((*string_in_new != ' ') && (*string_in_new != '\0')) {
      string_in_new++;
    }
    while (*string_in_new == ' ') {
      string_in_new++;
    }
  }
  (*integers_out)[i_max] = 0;
  return (i_max);
}

int ext_mpi_read_parameters(char *buffer_in, struct parameters_block **parameters) {
  char string1[100], string2[100], *buffer_in_new, *buffer_in_copy,
      *buffer_in_pcopy = NULL;
  int nbuffer_in = 0, integer1, flag;
  *parameters =
      (struct parameters_block *)malloc(sizeof(struct parameters_block));
  if (!*parameters)
    return ERROR_MALLOC;
  (*parameters)->ascii_in = (buffer_in[0] != '\0');
  if (!(*parameters)->ascii_in) {
    nbuffer_in++;
    memcpy(*parameters, buffer_in + nbuffer_in, sizeof(**parameters));
    nbuffer_in += sizeof(**parameters);
    (*parameters)->ascii_in = 0;
    (*parameters)->counts = NULL;
    (*parameters)->num_ports = NULL;
    (*parameters)->groups = NULL;
    (*parameters)->message_sizes = NULL;
    (*parameters)->rank_perm = NULL;
    (*parameters)->iocounts = NULL;
    (*parameters)->shmem_buffer_offset = NULL;
    if ((*parameters)->counts_max) {
      (*parameters)->counts =
          (int *)malloc(sizeof(int) * ((*parameters)->counts_max + 1));
      if (!(*parameters)->counts)
        goto error;
      memcpy((*parameters)->counts, buffer_in + nbuffer_in,
             sizeof(int) * (*parameters)->counts_max);
      (*parameters)->counts[(*parameters)->counts_max] = 0;
      nbuffer_in += sizeof(int) * (*parameters)->counts_max;
    }
    if ((*parameters)->num_ports_max) {
      (*parameters)->num_ports =
          (int *)malloc(sizeof(int) * ((*parameters)->num_ports_max + 1));
      if (!(*parameters)->num_ports)
        goto error;
      memcpy((*parameters)->num_ports, buffer_in + nbuffer_in,
             sizeof(int) * (*parameters)->num_ports_max);
      (*parameters)->num_ports[(*parameters)->num_ports_max] = 0;
      nbuffer_in += sizeof(int) * (*parameters)->num_ports_max;
    }
    if ((*parameters)->groups_max) {
      (*parameters)->groups =
          (int *)malloc(sizeof(int) * ((*parameters)->groups_max + 1));
      if (!(*parameters)->groups)
        goto error;
      memcpy((*parameters)->groups, buffer_in + nbuffer_in,
             sizeof(int) * (*parameters)->groups_max);
      (*parameters)->groups[(*parameters)->groups_max] = 0;
      nbuffer_in += sizeof(int) * (*parameters)->groups_max;
    }
    if ((*parameters)->message_sizes_max) {
      (*parameters)->message_sizes =
          (int *)malloc(sizeof(int) * ((*parameters)->message_sizes_max + 1));
      if (!(*parameters)->message_sizes)
        goto error;
      memcpy((*parameters)->message_sizes, buffer_in + nbuffer_in,
             sizeof(int) * (*parameters)->message_sizes_max);
      (*parameters)->message_sizes[(*parameters)->message_sizes_max] = 0;
      nbuffer_in += sizeof(int) * (*parameters)->message_sizes_max;
    }
    if ((*parameters)->rank_perm_max) {
      (*parameters)->rank_perm =
          (int *)malloc(sizeof(int) * ((*parameters)->rank_perm_max + 1));
      if (!(*parameters)->rank_perm)
        goto error;
      memcpy((*parameters)->rank_perm, buffer_in + nbuffer_in,
             sizeof(int) * (*parameters)->rank_perm_max);
      (*parameters)->rank_perm[(*parameters)->rank_perm_max] = 0;
      nbuffer_in += sizeof(int) * (*parameters)->rank_perm_max;
    }
    if ((*parameters)->iocounts_max) {
      (*parameters)->iocounts =
          (int *)malloc(sizeof(int) * ((*parameters)->iocounts_max + 1));
      if (!(*parameters)->iocounts)
        goto error;
      memcpy((*parameters)->iocounts, buffer_in + nbuffer_in,
             sizeof(int) * (*parameters)->iocounts_max);
      (*parameters)->iocounts[(*parameters)->iocounts_max] = 0;
      nbuffer_in += sizeof(int) * (*parameters)->iocounts_max;
    }
    if ((*parameters)->shmem_buffer_offset_max) {
      (*parameters)->shmem_buffer_offset = (int *)malloc(
          sizeof(int) * ((*parameters)->shmem_buffer_offset_max + 1));
      if (!(*parameters)->shmem_buffer_offset)
        goto error;
      memcpy((*parameters)->shmem_buffer_offset, buffer_in + nbuffer_in,
             sizeof(int) * (*parameters)->shmem_buffer_offset_max);
      (*parameters)
          ->shmem_buffer_offset[(*parameters)->shmem_buffer_offset_max] = 0;
      nbuffer_in += sizeof(int) * (*parameters)->shmem_buffer_offset_max;
    }
    return nbuffer_in;
  }
  buffer_in_pcopy = strdup(buffer_in);
  if (!buffer_in_pcopy)
    return ERROR_MALLOC;
  buffer_in_copy = buffer_in_pcopy;
  (*parameters)->counts = NULL;
  (*parameters)->counts_max = 0;
  (*parameters)->num_ports = NULL;
  (*parameters)->num_ports_max = 0;
  (*parameters)->groups = NULL;
  (*parameters)->groups_max = 0;
  (*parameters)->message_sizes = NULL;
  (*parameters)->message_sizes_max = 0;
  (*parameters)->rank_perm = NULL;
  (*parameters)->rank_perm_max = 0;
  (*parameters)->iocounts = NULL;
  (*parameters)->iocounts_max = 0;
  (*parameters)->collective_type = collective_type_allreduce_group;
  (*parameters)->node = 0;
  (*parameters)->num_nodes = 0;
  (*parameters)->node_rank = 0;
  (*parameters)->node_row_size = 1;
  (*parameters)->node_column_size = 1;
  (*parameters)->copy_method = 0;
  (*parameters)->data_type = data_type_char;
  (*parameters)->verbose = 0;
  (*parameters)->bit_identical = 0;
  (*parameters)->ascii_out = 0;
  (*parameters)->locmem_max = -1;
  (*parameters)->shmem_max = -1;
  (*parameters)->shmem_buffer_offset = NULL;
  (*parameters)->shmem_buffer_offset_max = 0;
  (*parameters)->root = -1;
  (*parameters)->in_place = 0;
  (*parameters)->blocking = 0;
  (*parameters)->on_gpu = 0;
  do {
    memset(string1, 0, 100);
    memset(string2, 0, 100);
    if (sscanf(buffer_in_copy, "%99s %99s %d", string1, string2, &integer1) >
        0) {
      buffer_in_new = strchr(buffer_in_copy, '\n');
      if (buffer_in_new) {
        *buffer_in_new = '\0';
      }
      flag = 1;
      if (strcmp(string1, "PARAMETER") == 0) {
        if (strcmp(string2, "VERBOSE") == 0) {
          (*parameters)->verbose = 1;
        }
        if (strcmp(string2, "BLOCKING") == 0) {
          (*parameters)->blocking = 1;
        }
        if (strcmp(string2, "IN_PLACE") == 0) {
          (*parameters)->in_place = 1;
        }
#ifdef GPU_ENABLED
        if (strcmp(string2, "ON_GPU") == 0) {
          (*parameters)->on_gpu = 1;
        }
#endif
        if (strcmp(string2, "ASCII") == 0) {
          (*parameters)->ascii_out = 1;
        }
        if (strcmp(string2, "BIT_IDENTICAL") == 0) {
          (*parameters)->bit_identical = 1;
        }
        if (strcmp(string2, "ROOT") == 0) {
          (*parameters)->root = integer1;
        }
        if (strcmp(string2, "NODE") == 0) {
          (*parameters)->node = integer1;
        }
        if (strcmp(string2, "NUM_NODES") == 0) {
          (*parameters)->num_nodes = integer1;
        }
        if (strcmp(string2, "NODE_RANK") == 0) {
          (*parameters)->node_rank = integer1;
        }
        if (strcmp(string2, "NODE_ROW_SIZE") == 0) {
          (*parameters)->node_row_size = integer1;
        }
        if (strcmp(string2, "NODE_COLUMN_SIZE") == 0) {
          (*parameters)->node_column_size = integer1;
        }
        if (strcmp(string2, "COPY_METHOD") == 0) {
          (*parameters)->copy_method = integer1;
        }
        if (strcmp(string2, "LOCMEM_MAX") == 0) {
          (*parameters)->locmem_max = integer1;
        }
        if (strcmp(string2, "SHMEM_MAX") == 0) {
          (*parameters)->shmem_max = integer1;
        }
        if (strcmp(string2, "SHMEM_BUFFER_OFFSET") == 0) {
          free((*parameters)->shmem_buffer_offset);
          while ((*buffer_in_copy < '0' || *buffer_in_copy > '9') &&
                 (*buffer_in_copy != '-') && (*buffer_in_copy != '\0')) {
            buffer_in_copy++;
          }
          (*parameters)->shmem_buffer_offset_max = read_int_series(
              buffer_in_copy, &((*parameters)->shmem_buffer_offset));
          if ((*parameters)->shmem_buffer_offset_max < 0)
            goto error;
        }
        if (strcmp(string2, "COUNTS") == 0) {
          free((*parameters)->counts);
          while ((*buffer_in_copy < '0' || *buffer_in_copy > '9') &&
                 (*buffer_in_copy != '-') && (*buffer_in_copy != '\0')) {
            buffer_in_copy++;
          }
          (*parameters)->counts_max =
              read_int_series(buffer_in_copy, &((*parameters)->counts));
          if ((*parameters)->counts_max < 0)
            goto error;
        }
        if (strcmp(string2, "NUM_PORTS") == 0) {
          free((*parameters)->num_ports);
          free((*parameters)->groups);
          while ((*buffer_in_copy < '0' || *buffer_in_copy > '9') &&
                 (*buffer_in_copy != '-') && (*buffer_in_copy != '\0')) {
            buffer_in_copy++;
          }
          if (ext_mpi_scan_ports_groups(buffer_in_copy, &(*parameters)->num_ports, &(*parameters)->groups)<0)
            goto error;
          for ((*parameters)->num_ports_max = 0; (*parameters)->num_ports[(*parameters)->num_ports_max];
               (*parameters)->num_ports_max++);
          (*parameters)->groups_max = (*parameters)->num_ports_max;
        }
        if (strcmp(string2, "MESSAGE_SIZE") == 0) {
          free((*parameters)->message_sizes);
          while ((*buffer_in_copy < '0' || *buffer_in_copy > '9') &&
                 (*buffer_in_copy != '-') && (*buffer_in_copy != '\0')) {
            buffer_in_copy++;
          }
          (*parameters)->message_sizes_max =
              read_int_series(buffer_in_copy, &((*parameters)->message_sizes));
          if ((*parameters)->message_sizes_max < 0)
            goto error;
        }
        if (strcmp(string2, "RANK_PERM") == 0) {
          free((*parameters)->rank_perm);
          while ((*buffer_in_copy < '0' || *buffer_in_copy > '9') &&
                 (*buffer_in_copy != '-') && (*buffer_in_copy != '\0')) {
            buffer_in_copy++;
          }
          (*parameters)->rank_perm_max =
              read_int_series(buffer_in_copy, &((*parameters)->rank_perm));
          if ((*parameters)->rank_perm_max < 0)
            goto error;
        }
        if (strcmp(string2, "IOCOUNTS") == 0) {
          free((*parameters)->iocounts);
          while ((*buffer_in_copy < '0' || *buffer_in_copy > '9') &&
                 (*buffer_in_copy != '-') && (*buffer_in_copy != '\0')) {
            buffer_in_copy++;
          }
          (*parameters)->iocounts_max =
              read_int_series(buffer_in_copy, &((*parameters)->iocounts));
          if ((*parameters)->iocounts_max < 0)
            goto error;
        }
        if (strcmp(string2, "COLLECTIVE_TYPE") == 0) {
          if (sscanf(buffer_in_copy, "%*s %*s %99s", string2) > 0) {
            if (strcmp(string2, "ALLGATHERV") == 0) {
              (*parameters)->collective_type = collective_type_allgatherv;
            }
            if (strcmp(string2, "REDUCE_SCATTER") == 0) {
              (*parameters)->collective_type = collective_type_reduce_scatter;
            }
            if (strcmp(string2, "ALLREDUCE") == 0) {
              (*parameters)->collective_type = collective_type_allreduce;
            }
            if (strcmp(string2, "ALLREDUCE_GROUP") == 0) {
              (*parameters)->collective_type = collective_type_allreduce_group;
            }
            if (strcmp(string2, "ALLREDUCE_SHORT") == 0) {
              (*parameters)->collective_type = collective_type_allreduce_short;
            }
          }
        }
        if (strcmp(string2, "DATA_TYPE") == 0) {
          if (sscanf(buffer_in_copy, "%*s %*s %99s", string2) > 0) {
            if (strcmp(string2, "INT") == 0) {
              (*parameters)->data_type = data_type_int;
            }
            if (strcmp(string2, "LONG_INT") == 0) {
              (*parameters)->data_type = data_type_long_int;
            }
            if (strcmp(string2, "FLOAT") == 0) {
              (*parameters)->data_type = data_type_float;
            }
            if (strcmp(string2, "DOUBLE") == 0) {
              (*parameters)->data_type = data_type_double;
            }
          }
        }
      } else {
        if (string1[0] != '#') {
          flag = 0;
        }
      }
      if (flag) {
        if (buffer_in_new) {
          buffer_in_copy = buffer_in_new + 1;
        }
        flag = (buffer_in_new != NULL);
      }
    } else {
      flag = 0;
    }
  } while (flag);
  integer1 = buffer_in_copy - buffer_in_pcopy;
  free(buffer_in_pcopy);
  return integer1;
error:
  free(buffer_in_pcopy);
  ext_mpi_delete_parameters(*parameters);
  *parameters = NULL;
  return ERROR_MALLOC;
}

int ext_mpi_write_parameters(struct parameters_block *parameters, char *buffer_out) {
  int nbuffer_out = 0, i;
  char *str;
  if (!parameters->ascii_out) {
    buffer_out[0] = '\0';
    nbuffer_out++;
    memcpy(buffer_out + nbuffer_out, parameters, sizeof(*parameters));
    nbuffer_out += sizeof(*parameters);
    if (parameters->counts_max) {
      memcpy(buffer_out + nbuffer_out, parameters->counts,
             sizeof(int) * parameters->counts_max);
      nbuffer_out += sizeof(int) * parameters->counts_max;
    }
    if (parameters->num_ports_max) {
      memcpy(buffer_out + nbuffer_out, parameters->num_ports,
             sizeof(int) * parameters->num_ports_max);
      nbuffer_out += sizeof(int) * parameters->num_ports_max;
    }
    if (parameters->groups_max) {
      memcpy(buffer_out + nbuffer_out, parameters->groups,
             sizeof(int) * parameters->groups_max);
      nbuffer_out += sizeof(int) * parameters->groups_max;
    }
    if (parameters->message_sizes_max) {
      memcpy(buffer_out + nbuffer_out, parameters->message_sizes,
             sizeof(int) * parameters->message_sizes_max);
      nbuffer_out += sizeof(int) * parameters->message_sizes_max;
    }
    if (parameters->rank_perm_max) {
      memcpy(buffer_out + nbuffer_out, parameters->rank_perm,
             sizeof(int) * parameters->rank_perm_max);
      nbuffer_out += sizeof(int) * parameters->rank_perm_max;
    }
    if (parameters->iocounts_max) {
      memcpy(buffer_out + nbuffer_out, parameters->iocounts,
             sizeof(int) * parameters->iocounts_max);
      nbuffer_out += sizeof(int) * parameters->iocounts_max;
    }
    if (parameters->shmem_buffer_offset_max) {
      memcpy(buffer_out + nbuffer_out, parameters->shmem_buffer_offset,
             sizeof(int) * parameters->shmem_buffer_offset_max);
      nbuffer_out += sizeof(int) * parameters->shmem_buffer_offset_max;
    }
    return nbuffer_out;
  }
  if (parameters->locmem_max >= 0) {
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER LOCMEM_MAX %d\n",
                parameters->locmem_max);
  }
  if (parameters->shmem_max >= 0) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out,
                           " PARAMETER SHMEM_MAX %d\n", parameters->shmem_max);
  }
  if (parameters->shmem_buffer_offset_max > 0) {
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER SHMEM_BUFFER_OFFSET");
    for (i = 0; i < parameters->shmem_buffer_offset_max; i++) {
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " %d",
                             parameters->shmem_buffer_offset[i]);
    }
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  }
  switch (parameters->collective_type) {
  case collective_type_allgatherv:
    nbuffer_out += sprintf(buffer_out + nbuffer_out,
                           " PARAMETER COLLECTIVE_TYPE ALLGATHERV\n");
    break;
  case collective_type_reduce_scatter:
    nbuffer_out += sprintf(buffer_out + nbuffer_out,
                           " PARAMETER COLLECTIVE_TYPE REDUCE_SCATTER\n");
    break;
  case collective_type_allreduce:
    nbuffer_out += sprintf(buffer_out + nbuffer_out,
                           " PARAMETER COLLECTIVE_TYPE ALLREDUCE\n");
    break;
  case collective_type_allreduce_group:
    nbuffer_out += sprintf(buffer_out + nbuffer_out,
                           " PARAMETER COLLECTIVE_TYPE ALLREDUCE_GROUP\n");
    break;
  case collective_type_allreduce_short:
    nbuffer_out += sprintf(buffer_out + nbuffer_out,
                           " PARAMETER COLLECTIVE_TYPE ALLREDUCE_SHORT\n");
    break;
  }
  nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER NODE %d\n",
                         parameters->node);
  nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER NUM_NODES %d\n",
                         parameters->num_nodes);
  nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER NODE_RANK %d\n",
                         parameters->node_rank);
  nbuffer_out +=
      sprintf(buffer_out + nbuffer_out, " PARAMETER NODE_ROW_SIZE %d\n",
              parameters->node_row_size);
  nbuffer_out +=
      sprintf(buffer_out + nbuffer_out, " PARAMETER NODE_COLUMN_SIZE %d\n",
              parameters->node_column_size);
  nbuffer_out +=
      sprintf(buffer_out + nbuffer_out, " PARAMETER COPY_METHOD %d\n",
              parameters->copy_method);
  if ((parameters->root >= 0) || (parameters->root <= -10)) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER ROOT %d\n",
                           parameters->root);
  }
  if (parameters->counts_max > 0) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER COUNTS");
    for (i = 0; i < parameters->counts_max; i++) {
      nbuffer_out +=
          sprintf(buffer_out + nbuffer_out, " %d", parameters->counts[i]);
    }
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  }
  if (parameters->num_ports_max > 0) {
    str = ext_mpi_print_ports_groups(parameters->num_ports, parameters->groups);
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER NUM_PORTS %s\n", str);
    free(str);
  }
  if (parameters->message_sizes_max > 0) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER MESSAGE_SIZE");
    for (i = 0; i < parameters->message_sizes_max; i++) {
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " %d",
                             parameters->message_sizes[i]);
    }
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  }
  if (parameters->rank_perm_max > 0) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER RANK_PERM");
    for (i = 0; i < parameters->rank_perm_max; i++) {
      nbuffer_out +=
          sprintf(buffer_out + nbuffer_out, " %d", parameters->rank_perm[i]);
    }
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  }
  if (parameters->iocounts_max > 0) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER IOCOUNTS");
    for (i = 0; i < parameters->iocounts_max; i++) {
      nbuffer_out +=
          sprintf(buffer_out + nbuffer_out, " %d", parameters->iocounts[i]);
    }
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  }
  switch (parameters->data_type) {
  case data_type_char:
    break;
  case data_type_int:
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER DATA_TYPE INT\n");
    break;
  case data_type_long_int:
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER DATA_TYPE LONG_INT\n");
    break;
  case data_type_float:
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER DATA_TYPE FLOAT\n");
    break;
  case data_type_double:
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER DATA_TYPE DOUBLE\n");
    break;
  }
  if (parameters->bit_identical) {
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER BIT_IDENTICAL\n");
  }
  if (parameters->in_place) {
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER IN_PLACE\n");
  }
  if (parameters->blocking) {
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER BLOCKING\n");
  }
#ifdef GPU_ENABLED
  if (parameters->on_gpu) {
    nbuffer_out +=
        sprintf(buffer_out + nbuffer_out, " PARAMETER ON_GPU\n");
  }
#endif
  if (parameters->verbose) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER VERBOSE\n");
  }
  if (parameters->ascii_out) {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, " PARAMETER ASCII\n");
  }
  return nbuffer_out;
}

int ext_mpi_delete_parameters(struct parameters_block *parameters) {
  free(parameters->shmem_buffer_offset);
  free(parameters->iocounts);
  free(parameters->counts);
  free(parameters->num_ports);
  free(parameters->groups);
  free(parameters->message_sizes);
  free(parameters);
  return 0;
}

static int read_int_tuple_series(char *string_in, int **integers_out_1st,
                                 int **integers_out_2nd) {
  char *string_in_new, *string_in__ = NULL;
  int i, i_max, j;
  *integers_out_1st = *integers_out_2nd = NULL;
  if (!string_in) {
    return (-1);
  }
  while (*string_in == ' ') {
    string_in++;
  }
  string_in__ = strdup(string_in);
  if (!string_in__)
    goto error;
  string_in_new = string_in__;
  while (*string_in_new != '\0') {
    if (*string_in_new == '|') {
      *string_in_new = ' ';
    }
    string_in_new++;
  }
  string_in_new = string_in__;
  i = 1;
  i_max = 0;
  while ((*string_in_new != '\0') && i) {
    while ((*string_in_new != ' ') && (*string_in_new != '\0') && i) {
      if (((*string_in_new >= '0') && (*string_in_new <= '9')) ||
          (*string_in_new == '-')) {
        string_in_new++;
      } else {
        i = 0;
      }
    }
    if (i) {
      while (*string_in_new == ' ') {
        string_in_new++;
      }
    } else {
      i_max--;
    }
    i_max++;
  }
  i_max /= 2;
  *integers_out_1st = (int *)malloc(i_max * sizeof(int));
  if (!*integers_out_1st)
    goto error;
  *integers_out_2nd = (int *)malloc(i_max * sizeof(int));
  if (!*integers_out_2nd)
    goto error;
  string_in_new = string_in__;
  for (i = 0; i < i_max; i++) {
    if (sscanf(string_in_new, "%d %d", &(*integers_out_1st)[i],
               &(*integers_out_2nd)[i]) != 2) {
      free(string_in__);
      return (-1);
    }
    for (j = 0; j < 2; j++) {
      while ((*string_in_new != ' ') && (*string_in_new != '\0')) {
        string_in_new++;
      }
      while (*string_in_new == ' ') {
        string_in_new++;
      }
    }
  }
  free(string_in__);
  return i_max;
error:
  free(*integers_out_1st);
  *integers_out_1st = NULL;
  free(*integers_out_2nd);
  *integers_out_2nd = NULL;
  free(string_in__);
  return ERROR_MALLOC;
}

int ext_mpi_read_stage_line(char *string_in, struct data_line *data) {
  char string_stage[100], string_frac[100], string_source[100], string_to[100];
  int stage, frac, source;
  if (sscanf(string_in, "%99s %d %99s %d %99s %d %99s", string_stage, &stage,
             string_frac, &frac, string_source, &source, string_to) != 7) {
    return ERROR_SYNTAX;
  }
  if (strcmp(string_stage, "STAGE") || strcmp(string_frac, "FRAC") ||
      strcmp(string_source, "SOURCE") || strcmp(string_to, "TO")) {
    return ERROR_SYNTAX;
  }
  data->frac = frac;
  data->source = source;
  if (!string_in) {
    return ERROR_SYNTAX;
  }
  string_in = strstr(string_in, "TO ");
  if (!string_in) {
    return ERROR_SYNTAX;
  }
  data->to_max = read_int_series(string_in + strlen("TO "), &data->to);
  if (data->to_max < 0)
    return (data->to_max);
  string_in = strstr(string_in, "FROM ");
  if (!string_in) {
    return ERROR_SYNTAX;
  }
  data->from_max = read_int_tuple_series(string_in + strlen("FROM "),
                                         &data->from_node, &data->from_line);
  if (data->from_max < 0) {
    free(data->to);
    return data->from_max;
  }
  return 0;
}

void ext_mpi_delete_stage_line(struct data_line data) {
  free(data.to);
  free(data.from_node);
  free(data.from_line);
}

int ext_mpi_read_algorithm(char *buffer_in, int *size_level0, int **size_level1,
                           struct data_line ***data, int ascii_in) {
  char *line = NULL;
  int nbuffer_in = 0, stage, flag, i, j, stage_old, err;
  enum eassembler_type estring1;
  *size_level1 = NULL;
  *data = NULL;
  line = (char *)malloc(100000);
  if (!line)
    goto error;
  if (!ascii_in) {
    memcpy(size_level0, buffer_in + nbuffer_in, sizeof(*size_level0));
    nbuffer_in += sizeof(*size_level0);
    *size_level1 = (int *)malloc(sizeof(int) * (*size_level0));
    if (!(*size_level1))
      goto error;
    memcpy(*size_level1, buffer_in + nbuffer_in, sizeof(int) * (*size_level0));
    nbuffer_in += sizeof(int) * (*size_level0);
    *data = (struct data_line **)malloc(sizeof(struct data_line *) *
                                        (*size_level0));
    if (!*data)
      goto error;
    for (i = 0; i < *size_level0; i++) {
      (*data)[i] = NULL;
    }
    for (i = 0; i < *size_level0; i++) {
      (*data)[i] = (struct data_line *)malloc((*size_level1)[i] *
                                              sizeof(struct data_line));
      if (!(*data)[i])
        goto error;
      for (j = 0; j < (*size_level1)[i]; j++) {
        memcpy(&(*data)[i][j], buffer_in + nbuffer_in,
               sizeof(struct data_line));
        nbuffer_in += sizeof(struct data_line);
        (*data)[i][j].to = (int *)malloc(sizeof(int) * (*data)[i][j].to_max);
        if (!(*data)[i][j].to)
          goto error;
        memcpy((*data)[i][j].to, buffer_in + nbuffer_in,
               sizeof(int) * (*data)[i][j].to_max);
        nbuffer_in += sizeof(int) * (*data)[i][j].to_max;
        (*data)[i][j].from_node =
            (int *)malloc(sizeof(int) * (*data)[i][j].from_max);
        if (!(*data)[i][j].from_node)
          goto error;
        memcpy((*data)[i][j].from_node, buffer_in + nbuffer_in,
               sizeof(int) * (*data)[i][j].from_max);
        nbuffer_in += sizeof(int) * (*data)[i][j].from_max;
        (*data)[i][j].from_line =
            (int *)malloc(sizeof(int) * (*data)[i][j].from_max);
        if (!(*data)[i][j].from_line)
          goto error;
        memcpy((*data)[i][j].from_line, buffer_in + nbuffer_in,
               sizeof(int) * (*data)[i][j].from_max);
        nbuffer_in += sizeof(int) * (*data)[i][j].from_max;
      }
    }
  } else {
    *size_level0 = -1;
    do {
      nbuffer_in += flag = read_single_line(buffer_in + nbuffer_in, line);
      if (flag && (ext_mpi_read_assembler_line_sd(line, &estring1, &stage, 1) >= 0)) {
        if (estring1 == estage) {
          if (stage > *size_level0) {
            *size_level0 = stage;
          }
        } else {
          if (estring1 != enop) {
            flag = 0;
          }
        }
      }
    } while (flag);
    (*size_level0)++;
    *size_level1 = (int *)malloc(sizeof(int) * (*size_level0));
    if (!*size_level1)
      goto error;
    *data = (struct data_line **)malloc(sizeof(struct data_line *) *
                                        (*size_level0));
    for (i = 0; i < *size_level0; i++) {
      (*data)[i] = NULL;
    }
    if (!*data)
      goto error;
    for (i = 0; i < *size_level0; i++) {
      (*size_level1)[i] = 0;
    }
    nbuffer_in = 0;
    do {
      nbuffer_in += flag = read_single_line(buffer_in + nbuffer_in, line);
      if (flag && (ext_mpi_read_assembler_line_sd(line, &estring1, &stage, 1) >= 0)) {
        if (estring1 == estage) {
          ((*size_level1)[stage])++;
        } else {
          if (estring1 != enop) {
            flag = 0;
          }
        }
      }
    } while (flag);
    for (i = 0; i < *size_level0; i++) {
      (*data)[i] = (struct data_line *)malloc((*size_level1)[i] *
                                              sizeof(struct data_line));
      if (!(*data)[i])
        goto error;
    }
    nbuffer_in = 0;
    stage_old = -1;
    do {
      nbuffer_in += flag = read_single_line(buffer_in + nbuffer_in, line);
      if (flag && (ext_mpi_read_assembler_line_sd(line, &estring1, &stage, 1) >= 0)) {
        if (estring1 == estage) {
          if (stage != stage_old) {
            i = 0;
            stage_old = stage;
          }
          err = ext_mpi_read_stage_line(line, &(*data)[stage][i]);
          if (err < 0)
            return err;
          i++;
        } else {
          if (estring1 != enop) {
            nbuffer_in -= flag;
            flag = 0;
          }
        }
      }
    } while (flag);
  }
  free(line);
  return nbuffer_in;
error:
  ext_mpi_delete_algorithm(*size_level0, *size_level1, *data);
  *size_level0 = 0;
  *size_level1 = NULL;
  *data = NULL;
  free(line);
  return ERROR_MALLOC;
}

int ext_mpi_write_algorithm(int size_level0, int *size_level1, struct data_line **data,
                            char *buffer_out, int ascii_out) {
  int nbuffer_out = 0, i, j, k;
  if (!ascii_out) {
    memcpy(buffer_out + nbuffer_out, &size_level0, sizeof(size_level0));
    nbuffer_out += sizeof(size_level0);
    memcpy(buffer_out + nbuffer_out, size_level1, sizeof(int) * size_level0);
    nbuffer_out += sizeof(int) * size_level0;
    for (i = 0; i < size_level0; i++) {
      for (j = 0; j < size_level1[i]; j++) {
        memcpy(buffer_out + nbuffer_out, &data[i][j], sizeof(struct data_line));
        nbuffer_out += sizeof(struct data_line);
        memcpy(buffer_out + nbuffer_out, data[i][j].to,
               sizeof(int) * data[i][j].to_max);
        nbuffer_out += sizeof(int) * data[i][j].to_max;
        memcpy(buffer_out + nbuffer_out, data[i][j].from_node,
               sizeof(int) * data[i][j].from_max);
        nbuffer_out += sizeof(int) * data[i][j].from_max;
        memcpy(buffer_out + nbuffer_out, data[i][j].from_line,
               sizeof(int) * data[i][j].from_max);
        nbuffer_out += sizeof(int) * data[i][j].from_max;
      }
    }
  } else {
    for (i = 0; i < size_level0; i++) {
      for (j = 0; j < size_level1[i]; j++) {
        nbuffer_out +=
            sprintf(buffer_out + nbuffer_out, " STAGE %d FRAC %d SOURCE %d TO",
                    i, data[i][j].frac, data[i][j].source);
        for (k = 0; k < data[i][j].to_max; k++) {
          nbuffer_out +=
              sprintf(buffer_out + nbuffer_out, " %d", data[i][j].to[k]);
        }
        nbuffer_out += sprintf(buffer_out + nbuffer_out, " FROM");
        for (k = 0; k < data[i][j].from_max; k++) {
          nbuffer_out +=
              sprintf(buffer_out + nbuffer_out, " %d|%d",
                      data[i][j].from_node[k], data[i][j].from_line[k]);
        }
        nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
      }
      nbuffer_out += sprintf(buffer_out + nbuffer_out, "#\n");
    }
  }
  return nbuffer_out;
}

void ext_mpi_delete_algorithm(int size_level0, int *size_level1,
                              struct data_line **data) {
  int i, j;
  for (i = 0; i < size_level0; i++) {
    if (data) {
      if (data[i]) {
        for (j = 0; j < size_level1[i]; j++) {
          ext_mpi_delete_stage_line(data[i][j]);
        }
      }
      free(data[i]);
    }
  }
  free(data);
  free(size_level1);
}

static int write_eassembler_type(char *buffer_out, enum eassembler_type string1,
                                 int ascii) {
  int nbuffer_out = 0;
  if (!ascii) {
    memcpy(buffer_out, &string1, sizeof(string1));
    nbuffer_out += sizeof(string1);
  } else {
    switch (string1) {
    case eset_num_cores:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SET_NUM_CORES");
      break;
    case eset_node_rank:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SET_NODE_RANK");
      break;
    case enode_barrier:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " NODE_BARRIER");
      break;
    case enode_cycl_barrier:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " NODE_CYCL_BARRIER");
      break;
    case eset_node_barrier:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SET_NODE_BARRIER");
      break;
    case ewait_node_barrier:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " WAIT_NODE_BARRIER");
      break;
    case enext_node_barrier:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " NEXT_NODE_BARRIER");
      break;
    case ememcpy:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " MEMCPY");
      break;
    case ememcp_:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " MEMCP_");
      break;
    case esmemcpy:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SMEMCPY");
      break;
    case esmemcp_:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SMEMCP_");
      break;
    case ereduce:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " REDUCE");
      break;
    case ereduc_:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " REDUC_");
      break;
    case esreduce:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SREDUCE");
      break;
    case esreduc_:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SREDUC_");
      break;
    case eirecv:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " IRECV");
      break;
    case eirec_:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " IREC_");
      break;
    case eisend:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " ISEND");
      break;
    case eisen_:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " ISEN_");
      break;
    case ewaitall:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " WAITALL");
      break;
    case ewaitany:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " WAITANY");
      break;
    case eattached:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " ATTACHED");
      break;
    case esendbufp:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SENDBUF+");
      break;
    case erecvbufp:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " RECVBUF+");
      break;
    case elocmemp:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " LOCMEM+");
      break;
    case eshmemo:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SHMEM[");
      break;
    case ecpbuffer_offseto:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " ]+BUFFER_OFFSET[");
      break;
    case ecp:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " ]+");
      break;
    case ereturn:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " RETURN");
      break;
    case enop:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " NOP");
      break;
    case estage:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " STAGE");
      break;
    case eset_mem:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " SET_MEM");
      break;
    case eunset_mem:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " UNSET_MEM");
      break;
    case estart:
      nbuffer_out += sprintf(buffer_out + nbuffer_out, " START");
      break;
    }
  }
  return nbuffer_out;
}

static int write_integer(char *buffer_out, int integer1, int ascii) {
  if (!ascii) {
    memcpy(buffer_out, &integer1, sizeof(integer1));
    return sizeof(integer1);
  } else {
    return sprintf(buffer_out, " %d", integer1);
  }
}

static int write_assembler_string(char *buffer_out, enum eassembler_type string1, int ascii){
  int nbuffer_out = 0;
  if (!ascii)
    buffer_out[nbuffer_out++] = 0;
  nbuffer_out +=
      write_eassembler_type(buffer_out + nbuffer_out, string1, ascii);
  return nbuffer_out;
}

static int write_assembler_integer(char *buffer_out, int integer1, int ascii){
  int nbuffer_out = 0;
  if (!ascii)
    buffer_out[nbuffer_out++] = 1;
  nbuffer_out +=
      write_integer(buffer_out + nbuffer_out, integer1, ascii);
  return nbuffer_out;
}

int ext_mpi_write_assembler_line(char *buffer_out, int ascii, char *types, ...) {
  va_list valist;
  int num, nbuffer_out = 0, i;
  if (!ascii)
    nbuffer_out += sizeof(int);
  for (num = 0; types[num]; num++);
  va_start(valist, num);
  for (i = 0; i < num; i++) {
    switch (types[i]) {
      case 's':
        nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, va_arg(valist, enum eassembler_type), ascii);
        break;
      case 'd':
        nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, va_arg(valist, int), ascii);
        break;
      default:
        printf("error in ext_mpi_write_assembler_line\n");
        exit(1);
    }
  }
  va_end(valist);
  if (!ascii) {
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  } else {
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  }
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_s(char *buffer_out, enum eassembler_type string1,
                                   int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_sd(char *buffer_out, enum eassembler_type string1,
                                    int integer1, int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_sdsd(char *buffer_out, enum eassembler_type string1,
                                      int integer1, enum eassembler_type string2,
                                      int integer2, int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_sddsd(char *buffer_out, enum eassembler_type string1,
                                       int integer1, int integer2, enum eassembler_type string2,
                                       int integer3, int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer3, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssd(char *buffer_out, enum eassembler_type string1,
                                     enum eassembler_type string2, int integer1,
                                     int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssdsd(char *buffer_out, enum eassembler_type string1,
                                       enum eassembler_type string2, int integer1,
                                       enum eassembler_type string3, int integer2,
                                       int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssdsdd(char *buffer_out, enum eassembler_type string1,
                                        enum eassembler_type string2, int integer1,
                                        enum eassembler_type string3, int integer2,
                                        int integer3, int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer3, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssdddd(char *buffer_out, enum eassembler_type string1,
                                        enum eassembler_type string2, int integer1,
                                        int integer2, int integer3, int integer4,
                                        int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer4, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssddd(char *buffer_out, enum eassembler_type string1,
                                       enum eassembler_type string2, int integer1,
                                       int integer2, int integer3,
                                       int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer3, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssdsdddd(char *buffer_out,
                                          enum eassembler_type string1,
                                          enum eassembler_type string2, int integer1,
                                          enum eassembler_type string3, int integer2,
                                          int integer3, int integer4, int integer5,
                                          int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer4, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer5, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssdsdsdd(char *buffer_out,
                                          enum eassembler_type string1,
                                          enum eassembler_type string2, int integer1,
                                          enum eassembler_type string3, int integer2,
                                          enum eassembler_type string4, int integer3,
                                          int integer4, int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string4, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer4, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

int ext_mpi_write_assembler_line_ssdsdsdsdd(char *buffer_out,
                                            enum eassembler_type string1,
                                            enum eassembler_type string2, int integer1,
                                            enum eassembler_type string3, int integer2,
                                            enum eassembler_type string4, int integer3,
                                            enum eassembler_type string5, int integer4,
                                            int integer5, int ascii) {
  int nbuffer_out = 0;
  if (!ascii)
    nbuffer_out += sizeof(int);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string2, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer1, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string3, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer2, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string4, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer3, ascii);
  nbuffer_out += write_assembler_string(buffer_out + nbuffer_out, string5, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer4, ascii);
  nbuffer_out += write_assembler_integer(buffer_out + nbuffer_out, integer5, ascii);
  if (!ascii)
    memcpy(buffer_out, &nbuffer_out, sizeof(nbuffer_out));
  if (ascii)
    nbuffer_out += sprintf(buffer_out + nbuffer_out, "\n");
  return nbuffer_out;
}

static enum eassembler_type read_assembler_type(char *cstring1) {
  if (strcmp(cstring1, "SET_NUM_CORES") == 0) {
    return eset_num_cores;
  }
  if (strcmp(cstring1, "SET_NODE_RANK") == 0) {
    return eset_node_rank;
  }
  if (strcmp(cstring1, "NODE_BARRIER") == 0) {
    return enode_barrier;
  }
  if (strcmp(cstring1, "NODE_CYCL_BARRIER") == 0) {
    return enode_cycl_barrier;
  }
  if (strcmp(cstring1, "SET_NODE_BARRIER") == 0) {
    return eset_node_barrier;
  }
  if (strcmp(cstring1, "WAIT_NODE_BARRIER") == 0) {
    return ewait_node_barrier;
  }
  if (strcmp(cstring1, "NEXT_NODE_BARRIER") == 0) {
    return enext_node_barrier;
  }
  if (strcmp(cstring1, "MEMCPY") == 0) {
    return ememcpy;
  }
  if (strcmp(cstring1, "MEMCP_") == 0) {
    return ememcp_;
  }
  if (strcmp(cstring1, "SMEMCPY") == 0) {
    return esmemcpy;
  }
  if (strcmp(cstring1, "SMEMCP_") == 0) {
    return esmemcp_;
  }
  if (strcmp(cstring1, "REDUCE") == 0) {
    return ereduce;
  }
  if (strcmp(cstring1, "REDUC_") == 0) {
    return ereduc_;
  }
  if (strcmp(cstring1, "SREDUCE") == 0) {
    return esreduce;
  }
  if (strcmp(cstring1, "SREDUC_") == 0) {
    return esreduc_;
  }
  if (strcmp(cstring1, "IRECV") == 0) {
    return eirecv;
  }
  if (strcmp(cstring1, "IREC_") == 0) {
    return eirec_;
  }
  if (strcmp(cstring1, "ISEND") == 0) {
    return eisend;
  }
  if (strcmp(cstring1, "ISEN_") == 0) {
    return eisen_;
  }
  if (strcmp(cstring1, "WAITALL") == 0) {
    return ewaitall;
  }
  if (strcmp(cstring1, "SENDBUF+") == 0) {
    return esendbufp;
  }
  if (strcmp(cstring1, "RECVBUF+") == 0) {
    return erecvbufp;
  }
  if (strcmp(cstring1, "LOCMEM+") == 0) {
    return elocmemp;
  }
  if (strcmp(cstring1, "SHMEM[") == 0) {
    return eshmemo;
  }
  if (strcmp(cstring1, "]+BUFFER_OFFSET[") == 0) {
    return ecpbuffer_offseto;
  }
  if (strcmp(cstring1, "]+") == 0) {
    return ecp;
  }
  if (strcmp(cstring1, "RETURN") == 0) {
    return ereturn;
  }
  if (strcmp(cstring1, "STAGE") == 0) {
    return estage;
  }
  if (strcmp(cstring1, "WAITANY") == 0) {
    return ewaitany;
  }
  if (strcmp(cstring1, "ATTACHED") == 0) {
    return eattached;
  }
  if (strcmp(cstring1, "SET_MEM") == 0) {
    return eset_mem;
  }
  if (strcmp(cstring1, "UNSET_MEM") == 0) {
    return eunset_mem;
  }
  if (strcmp(cstring1, "START") == 0) {
    return estart;
  }
  return (enop);
}

static int read_assembler_string_bin(char *buffer_in, int n, enum eassembler_type *string1, int *i){
  if (buffer_in[(*i)++] != 0)
    return -11;
  if (*i < n)
    memcpy(string1, buffer_in + *i, sizeof(*string1));
  *i += sizeof(*string1);
  return 0;
}

static int read_assembler_integer_bin(char *buffer_in, int n, int *integer1, int *i){
  if (buffer_in[(*i)++] != 1)
    return -11;
  if (*i < n)
    memcpy(integer1, buffer_in + *i, sizeof(*integer1));
  *i += sizeof(*integer1);
  return 0;
}

int ext_mpi_read_assembler_line(char *buffer_in, int ascii, char *types, ...) {
  va_list valist;
  char cstring1[100], cstring2[100];
  int num, nbuffer_in = 0, n, i, j;
  for (num = 0; types[num]; num++);
  va_start(valist, num);
  if (!ascii) {
    memcpy(&n, buffer_in, sizeof(n));
    j = sizeof(n);
    for (i = 0; i < num; i++) {
      switch (types[i]) {
        case 's':
          if ((nbuffer_in += read_assembler_string_bin(buffer_in + nbuffer_in, n, va_arg(valist, enum eassembler_type*), &j)) < 0) return -11;
          break;
        case 'd':
          if ((nbuffer_in += read_assembler_integer_bin(buffer_in + nbuffer_in, n, va_arg(valist, int*), &j)) < 0) return -11;
          break;
        default:
          printf("error in ext_mpi_read_assembler_line\n");
          exit(1);
      }
    }
    va_end(valist);
    if (j == n) {
      return n;
    } else if (j < n) {
      return 0;
    } else {
      return -11;
    }
  } else {
    for (i = 0; i < num; i++) {
      for (; buffer_in[nbuffer_in] == ' '; nbuffer_in++);
      switch (types[i]) {
        case 's':
          n = sscanf(buffer_in + nbuffer_in, "%99s %99s", cstring1, cstring2);
          *va_arg(valist, enum eassembler_type*) = read_assembler_type(cstring1);
          break;
        case 'd':
          n = sscanf(buffer_in + nbuffer_in, "%d %99s", va_arg(valist, int*), cstring2);
          break;
        default:
          printf("error in ext_mpi_read_assembler_line\n");
          exit(1);
      }
      if (n < 0) {
        va_end(valist);
        return -11;
      } else if (n < 1) {
        va_end(valist);
        return 0;
      }
      for (; buffer_in[nbuffer_in] != '\n'; nbuffer_in++);
    }
    va_end(valist);
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_s(char *buffer_in, enum eassembler_type *string1,
                                  int ascii) {
  char cstring1[100], cstring2[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s", cstring1, cstring2);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n == 2) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_sd(char *buffer_in, enum eassembler_type *string1,
                                   int *integer1, int ascii) {
  char cstring1[100], cstring2[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %d %99s", cstring1, integer1, cstring2);
    if (n < 1) {
      return -1;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -2;
    }
    if (n == 3) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_sdsd(char *buffer_in, enum eassembler_type *string1,
                                     int *integer1, enum eassembler_type *string2,
                                     int *integer2, int ascii) {
  char cstring1[100], cstring2[100], cstring3[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %d %99s %d %99s", cstring1, integer1, cstring2,
               integer2, cstring3);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 3) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 4) {
      return -11;
    }
    if (n == 5) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_sddsd(char *buffer_in, enum eassembler_type *string1,
                                      int *integer1, int *integer2, enum eassembler_type *string2,
                                      int *integer3, int ascii) {
  char cstring1[100], cstring2[100], cstring3[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer3, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %d %d %99s %d %99s", cstring1, integer1, integer2, cstring2,
               integer3, cstring3);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 4) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 5) {
      return -11;
    }
    if (n == 6) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_ssd(char *buffer_in, enum eassembler_type *string1,
                                    enum eassembler_type *string2, int *integer1,
                                    int ascii) {
  char cstring1[100], cstring2[100], cstring3[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s %d %99s", cstring1, cstring2, integer1, cstring3);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 4) {
      return -11;
    }
    if (n == 3) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_ssdsd(char *buffer_in, enum eassembler_type *string1,
                                      enum eassembler_type *string2, int *integer1,
                                      enum eassembler_type *string3, int *integer2,
                                      int ascii) {
  char cstring1[100], cstring2[100], cstring3[100], cstring4[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s %d %99s %d %99s", cstring1, cstring2,
               integer1, cstring3, integer2, cstring4);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 4) {
      return -11;
    }
    *string3 = read_assembler_type(cstring3);
    if (n == 6) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_ssdsdd(char *buffer_in, enum eassembler_type *string1,
                                       enum eassembler_type *string2, int *integer1,
                                       enum eassembler_type *string3, int *integer2,
                                       int *integer3, int ascii) {
  char cstring1[100], cstring2[100], cstring3[100], cstring4[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer3, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s %d %99s %d %d %99s", cstring1, cstring2,
               integer1, cstring3, integer2, integer3, cstring4);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 4) {
      return -11;
    }
    *string3 = read_assembler_type(cstring3);
    if (n < 6) {
      return -11;
    }
    if (n == 7) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_ssdddd(char *buffer_in, enum eassembler_type *string1,
                                       enum eassembler_type *string2, int *integer1,
                                       int *integer2, int *integer3, int *integer4,
                                       int ascii) {
  char cstring1[100], cstring2[100], cstring3[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer4, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s %d %d %d %d %99s", cstring1, cstring2,
               integer1, integer2, integer3, integer4, cstring3);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 6) {
      return -11;
    }
    if (n == 7) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_ssdsdddd(char *buffer_in, enum eassembler_type *string1,
                                         enum eassembler_type *string2, int *integer1,
                                         enum eassembler_type *string3, int *integer2,
                                         int *integer3, int *integer4, int *integer5,
                                         int ascii) {
  char cstring1[100], cstring2[100], cstring3[100], cstring4[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer4, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer5, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s %d %99s %d %d %d %d %99s", cstring1,
               cstring2, integer1, cstring3, integer2, integer3, integer4,
               integer5, cstring4);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 4) {
      return -11;
    }
    *string3 = read_assembler_type(cstring3);
    if (n < 8) {
      return -11;
    }
    if (n == 9) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_ssdsdsdd(char *buffer_in,
                                         enum eassembler_type *string1,
                                         enum eassembler_type *string2, int *integer1,
                                         enum eassembler_type *string3, int *integer2,
                                         enum eassembler_type *string4, int *integer3,
                                         int *integer4, int ascii) {
  char cstring1[100], cstring2[100], cstring3[100], cstring4[100],
      cstring5[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string4, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer4, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s %d %99s %d %99s %d %d %99s",
               cstring1, cstring2, integer1, cstring3, integer2, cstring4,
               integer3, integer4, cstring5);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 4) {
      return -11;
    }
    *string3 = read_assembler_type(cstring3);
    if (n < 6) {
      return -11;
    }
    *string4 = read_assembler_type(cstring4);
    if (n < 8) {
      return -11;
    }
    if (n == 9) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_read_assembler_line_ssdsdsdsdd(char *buffer_in,
                                           enum eassembler_type *string1,
                                           enum eassembler_type *string2, int *integer1,
                                           enum eassembler_type *string3, int *integer2,
                                           enum eassembler_type *string4, int *integer3,
                                           enum eassembler_type *string5, int *integer4,
                                           int *integer5, int ascii) {
  char cstring1[100], cstring2[100], cstring3[100], cstring4[100],
      cstring5[100], cstring6[100];
  int n, i;
  if (!ascii) {
    i = 0;
    memcpy(&n, buffer_in, sizeof(n));
    i += sizeof(n);
    if (read_assembler_string_bin(buffer_in, n, string1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string2, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer1, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string3, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer2, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string4, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer3, &i)<0) return -11;
    if (read_assembler_string_bin(buffer_in, n, string5, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer4, &i)<0) return -11;
    if (read_assembler_integer_bin(buffer_in, n, integer5, &i)<0) return -11;
    if (i == n) {
      return n;
    } else {
      if (i < n) {
        return 0;
      } else {
        return -11;
      }
    }
  } else {
    n = sscanf(buffer_in, "%99s %99s %d %99s %d %99s %d %99s %d %d %99s",
               cstring1, cstring2, integer1, cstring3, integer2, cstring4,
               integer3, cstring5, integer4, integer5, cstring6);
    if (n < 1) {
      return -11;
    }
    *string1 = read_assembler_type(cstring1);
    if (n < 2) {
      return -11;
    }
    *string2 = read_assembler_type(cstring2);
    if (n < 4) {
      return -11;
    }
    *string3 = read_assembler_type(cstring3);
    if (n < 6) {
      return -11;
    }
    *string4 = read_assembler_type(cstring4);
    if (n < 8) {
      return -11;
    }
    *string5 = read_assembler_type(cstring5);
    if (n < 10) {
      return -11;
    }
    if (n == 11) {
      return 0;
    }
    for (i = 0; (buffer_in[i] != '\0') && (buffer_in[i] != '\n'); i++)
      ;
    return i + 1;
  }
}

int ext_mpi_switch_to_ascii(char *buffer) {
  struct parameters_block *parameters;
  if (buffer[0] != '\0') {
    return -1;
  }
  parameters =
      (struct parameters_block *)malloc(sizeof(struct parameters_block));
  if (!parameters)
    return ERROR_MALLOC;
  memcpy(parameters, buffer + 1, sizeof(struct parameters_block));
  if (parameters->ascii_out) {
    free(parameters);
    return (0);
  }
  parameters->ascii_out = 1;
  memcpy(buffer + 1, parameters, sizeof(struct parameters_block));
  free(parameters);
  return (1);
}

int ext_mpi_read_line(char *buffer_in, char *line, int ascii) {
  enum eassembler_type estring1;
  char string1[100];
  int i, j, k, flag;
  if (!ascii) {
    memcpy(&i, buffer_in, sizeof(int));
    if (i > 99)
      line = NULL;
    memcpy(line, buffer_in, i);
    return i;
  } else {
    i = sizeof(i);
    flag = 1;
    k = 0;
    while (flag) {
      while (buffer_in[k] == ' ') {
        k++;
      }
      if (sscanf(buffer_in + k, "%d", &j) == 1) {
        line[i++] = 1;
        memcpy(line + i, &j, sizeof(j));
        i += sizeof(j);
      } else {
        if (sscanf(buffer_in + k, "%99s", string1) == 1) {
          estring1 = read_assembler_type(string1);
          line[i++] = 0;
          memcpy(line + i, &estring1, sizeof(estring1));
          i += sizeof(estring1);
        }
      }
      while ((buffer_in[k] != '\n') && (buffer_in[k] != '\0') &&
             (buffer_in[k] != ' ')) {
        k++;
      }
      if (buffer_in[k] != ' ') {
        flag = 0;
      }
      if (buffer_in[k] == '\n') {
        k++;
      }
    }
    memcpy(line, &i, sizeof(i));
    return k;
  }
}

int ext_mpi_write_line(char *buffer_out, char *line, int ascii) {
  enum eassembler_type estring1;
  char string1[100];
  int i, j, k, l;
  if (!ascii) {
    memcpy(&i, line, sizeof(int));
    memcpy(buffer_out, line, i);
    return i;
  } else {
    memcpy(&i, line, sizeof(int));
    j = sizeof(i);
    k = 0;
    while (j < i) {
      switch (line[j++]) {
      case 0:
        memcpy(&estring1, line + j, sizeof(estring1));
        j += sizeof(estring1);
        write_eassembler_type(string1, estring1, 1);
        k += sprintf(buffer_out + k, "%s", string1);
        break;
      case 1:
        memcpy(&l, line + j, sizeof(l));
        j += sizeof(l);
        k += sprintf(buffer_out + k, " %d", l);
        break;
      }
    }
    k += sprintf(buffer_out + k, "\n");
    return (k);
  }
}

int ext_mpi_write_eof(char *buffer_out, int ascii) {
  int i;
  if (!ascii) {
    i = 0;
    memcpy(buffer_out, &i, sizeof(i));
    return sizeof(i);
  } else {
    buffer_out[0] = '\0';
    return 1;
  }
}

int ext_mpi_read_irecv_isend(char *line, struct line_irecv_isend *data) {
  enum eassembler_type estring;
  int i;
  if (ext_mpi_read_assembler_line_s(line, &data->type, 0) >= 0) {
    if ((data->type == eirecv) || (data->type == eirec_) || (data->type == eisend) || (data->type == eisen_)) {
      i = ext_mpi_read_assembler_line(line, 0, "ssdddd", &data->type, &data->buffer_type, &data->offset, &data->size, &data->partner, &data->tag);
      if ((i < 0) || (data->buffer_type == eshmemo)) {
        i = ext_mpi_read_assembler_line(line, 0, "ssdsdddd", &data->type, &data->buffer_type, &data->buffer_number, &estring, &data->offset, &data->size, &data->partner, &data->tag);
        data->is_offset = 0;
        if ((i < 0) || (estring == ecpbuffer_offseto)) {
          data->is_offset = 1;
          if (ext_mpi_read_assembler_line(line, 0, "ssdsdsdddd", &data->type, &data->buffer_type, &data->buffer_number, &estring, &data->offset_number, &estring, &data->offset, &data->size, &data->partner, &data->tag) < 0) {
            printf(" error reading line in ext_mpi_read_irecv_isend\n");
            exit(1);
          }
        }
      }
      return 1;
    }
  }
  return 0;
}

int ext_mpi_read_memcpy_reduce(char *line, struct line_memcpy_reduce *data) {
  enum eassembler_type estring, estring2;
  int i;
  if (ext_mpi_read_assembler_line_s(line, &data->type, 0) >= 0) {
    if ((data->type == ememcpy) || (data->type == ememcp_) || (data->type == ereduce) || (data->type == ereduc_) ||
        (data->type == esmemcpy) || (data->type == esmemcp_) || (data->type == esreduce) || (data->type == esreduc_)) {
      i = ext_mpi_read_assembler_line(line, 0, "ssd", &data->type, &data->buffer_type1, &data->offset1);
      if ((i < 0) || (data->buffer_type1 == eshmemo)) {
        i = ext_mpi_read_assembler_line(line, 0, "ssdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset1);
        data->is_offset1 = 0;
        if ((i < 0) || (estring == ecpbuffer_offseto)) {
          data->is_offset1 = 1;
          if (ext_mpi_read_assembler_line(line, 0, "ssdsdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset_number1, &estring, &data->offset1) < 0) {
            printf(" error reading line in ext_mpi_read_memcpy_reduce\n");
            exit(1);
          } else {
            i = ext_mpi_read_assembler_line(line, 0, "ssdsdsdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset_number1, &estring, &data->offset1, &data->buffer_type2, &data->offset2);
            if ((i < 0) || (data->buffer_type2 == eshmemo)) {
              i = ext_mpi_read_assembler_line(line, 0, "ssdsdsdsdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset_number1, &estring, &data->offset1, &data->buffer_type2, &data->buffer_number2, &estring2, &data->offset2);
              data->is_offset2 = 0;
              if ((i < 0) || (estring2 == ecpbuffer_offseto)) {
                data->is_offset2 = 1;
                if (ext_mpi_read_assembler_line(line, 0, "ssdsdsdsdsdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset_number1, &estring, &data->offset1, &data->buffer_type2, &data->buffer_number2, &estring, &data->offset_number2, &estring, &data->offset2) < 0) {
                  printf(" error reading line in ext_mpi_read_memcpy_reduce\n");
                  exit(1);
                }
              }
            }
          }
        } else {
          i = ext_mpi_read_assembler_line(line, 0, "ssdsdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset1, &data->buffer_type2, &data->offset2);
          if ((i < 0) || (data->buffer_type2 == eshmemo)) {
            i = ext_mpi_read_assembler_line(line, 0, "ssdsdsdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset1, &data->buffer_type2, &data->buffer_number2, &estring2, &data->offset2);
            data->is_offset2 = 0;
            if ((i < 0) || (estring2 == ecpbuffer_offseto)) {
              data->is_offset2 = 1;
              if (ext_mpi_read_assembler_line(line, 0, "ssdsdsdsdsd", &data->type, &data->buffer_type1, &data->buffer_number1, &estring, &data->offset1, &data->buffer_type2, &data->buffer_number2, &estring, &data->offset_number2, &estring, &data->offset2) < 0) {
                printf(" error reading line in ext_mpi_read_memcpy_reduce\n");
                exit(1);
              }
            }
          }
        }
      } else {
        i = ext_mpi_read_assembler_line(line, 0, "ssdsdsd", &data->type, &data->buffer_type1, &data->offset1, &data->buffer_type2, &data->offset2, &data->size);
        if ((i < 0) || (data->buffer_type2 == eshmemo)) {
          i = ext_mpi_read_assembler_line(line, 0, "ssdsdsdsd", &data->type, &data->buffer_type1, &data->offset1, &data->buffer_type2, &data->buffer_number2, &estring, &data->offset2, &data->size);
          data->is_offset2 = 0;
          if ((i < 0) || (estring == ecpbuffer_offseto)) {
            data->is_offset2 = 1;
            if (ext_mpi_read_assembler_line(line, 0, "ssdsdsdsd", &data->type, &data->buffer_type1, &data->offset1, &data->buffer_type2, &data->buffer_number2, &estring, &data->offset_number2, &estring, &data->offset2, &data->size) < 0) {
              printf(" error reading line in ext_mpi_read_memcpy_reduce\n");
              exit(1);
            }
          }
        }
      }
      return 1;
    }
  }
  return 0;
}

int ext_mpi_write_irecv_isend(char *buffer_out, struct line_irecv_isend *data, int ascii) {
  if (data->buffer_type != eshmemo) {
    return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdddd", data->type, data->buffer_type, data->offset, data->size, data->partner, data->tag);
  } else if (!data->is_offset) {
    return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdddd", data->type, data->buffer_type, data->buffer_number, ecp, data->offset, data->size, data->partner, data->tag);
  } else {
    return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdddd", data->type, data->buffer_type, data->buffer_number, ecpbuffer_offseto, data->offset_number, ecp, data->offset, data->size, data->partner, data->tag);
  }
}

int ext_mpi_write_memcpy_reduce(char *buffer_out, struct line_memcpy_reduce *data, int ascii) {
  if (data->buffer_type1 != eshmemo) {
    if (data->buffer_type2 != eshmemo) {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdd", data->type, data->buffer_type1, data->offset1, data->buffer_type2, data->offset2, data->size);
    } else if (!data->is_offset2) {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdd", data->type, data->buffer_type1, data->offset1, data->buffer_type2, data->buffer_number2, ecp, data->offset2, data->size);
    } else {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdsdd", data->type, data->buffer_type1, data->offset1, data->buffer_type2, data->buffer_number2, ecpbuffer_offseto, data->offset_number2, ecp, data->offset2, data->size);
    }
  } else if (!data->is_offset1) {
    if (data->buffer_type2 != eshmemo) {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdd", data->type, data->buffer_type1, data->buffer_number1, ecp, data->offset1, data->buffer_type2, data->offset2, data->size);
    } else if (!data->is_offset2) {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdsdd", data->type, data->buffer_type1, data->buffer_number1, ecp, data->offset1, data->buffer_type2, data->buffer_number2, ecp, data->offset2, data->size);
    } else {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdsdsdd", data->type, data->buffer_type1, data->buffer_number1, ecp, data->offset1, data->buffer_type2, data->buffer_number2, ecpbuffer_offseto, data->offset_number2, ecp, data->offset2, data->size);
    }
  } else {
    if (data->buffer_type2 != eshmemo) {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdsdd", data->type, data->buffer_type1, data->buffer_number1, ecpbuffer_offseto, data->offset_number1, ecp, data->offset1, data->buffer_type2, data->offset2, data->size);
    } else if (!data->is_offset2) {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdsdsdd", data->type, data->buffer_type1, data->buffer_number1, ecpbuffer_offseto, data->offset_number1, ecp, data->offset1, data->buffer_type2, data->buffer_number2, ecp, data->offset2, data->size);
    } else {
      return ext_mpi_write_assembler_line(buffer_out, ascii, "ssdsdsdsdsdsdd", data->type, data->buffer_type1, data->buffer_number1, ecpbuffer_offseto, data->offset_number1, ecp, data->offset1, data->buffer_type2, data->buffer_number2, ecpbuffer_offseto, data->offset_number2, ecp, data->offset2, data->size);
    }
  }
}
