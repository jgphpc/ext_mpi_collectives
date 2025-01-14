function(add_mpi_test sourcename testname)
    add_executable(${testname} ${sourcename})

if(CMAKE_CUDA_COMPILER)
    target_compile_definitions(${testname} PRIVATE M_MAP GPU_ENABLED)
else()
    target_compile_definitions(${testname} PRIVATE M_MAP)
endif()

    target_include_directories(${testname} PRIVATE 
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include/core
        ${PROJECT_SOURCE_DIR}/include/mpi)

    target_link_libraries(${testname} PRIVATE ext_mpi_collectives)
    install(TARGETS ${testname} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()

add_mpi_test(test_waitany.c test_waitany.x)
add_mpi_test(test_use_recvbuf.c test_use_recvbuf.x)
add_mpi_test(test_reduce_copyout.c test_reduce_copyout.x)
add_mpi_test(test_reduce_copyin.c test_reduce_copyin.x)
add_mpi_test(test_raw_code_tasks_node_master.c test_raw_code_tasks_node_master.x)
add_mpi_test(test_raw_code_tasks_node.c test_raw_code_tasks_node.x)
add_mpi_test(test_raw_code_merge.c test_raw_code_merge.x)
add_mpi_test(test_raw_code.c test_raw_code.x)
add_mpi_test(test_rank_permutation_forward.c test_rank_permutation_forward.x)
add_mpi_test(test_rank_permutation_backward.c test_rank_permutation_backward.x)
add_mpi_test(test_parallel_memcpy.c test_parallel_memcpy.x)
add_mpi_test(test_optimise_buffers.c test_optimise_buffers.x)
add_mpi_test(test_optimise_buffers2.c test_optimise_buffers2.x)
add_mpi_test(test_no_socket_barriers.c test_no_socket_barriers.x)
add_mpi_test(test_no_offset.c test_no_offset.x)
add_mpi_test(test_no_first_barrier.c test_no_first_barrier.x)
add_mpi_test(test_move_first_memcpy.c test_move_first_memcpy.x)
add_mpi_test(test_dummy.c test_dummy.x)
add_mpi_test(test_clean_barriers.c test_clean_barriers.x)
add_mpi_test(test_buffer_offset.c test_buffer_offset.x)
add_mpi_test(test_alltoall.c test_alltoall.x)
add_mpi_test(test_allreduce_single.c test_allreduce_single.x)
add_mpi_test(test_allreduce_short.c test_allreduce_short.x)
add_mpi_test(test_allreduce.c test_allreduce.x)
add_mpi_test(simulate.c simulate.x)
add_mpi_test(get_input_reduce_scatter.c get_input_reduce_scatter.x)
add_mpi_test(get_input_alltoall.c get_input_alltoall.x)
add_mpi_test(get_input_allreduce.c get_input_allreduce.x)
add_mpi_test(get_input_allgatherv.c get_input_allgatherv.x)

add_mpi_test(benchmark_hierarchical.c benchmark_hierarchical.x)
add_mpi_test(benchmark.c benchmark.x)

# TODO: add_mpi_test(benchmark_nccl.c benchmark_nccl.x)

if(CMAKE_CUDA_COMPILER)
    add_mpi_test(benchmark_gpu.c benchmark_gpu.x)
endif()
