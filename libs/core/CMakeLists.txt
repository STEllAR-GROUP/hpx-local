# Copyright (c) 2020-2021 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Do not edit this file! It has been generated by the
# libs/create_module_skeleton.py script.

include(HPXLocal_Message)

# cmake-format: off
set(_hpx_local_modules
    affinity
    algorithms
    allocator_support
    asio
    assertion
    async_base
    async_combinators
    async_cuda
    async_local
    async_mpi
    batch_environments
    cache
    command_line_handling_local
    concepts
    concurrency
    config_local
    config_registry
    coroutines
    datastructures
    debugging
    errors
    execution
    execution_base
    executors
    filesystem
    format
    functional
    futures
    hardware
    hashing
    include_local
    ini
    init_runtime_local
    io_service
    iterator_support
    itt_notify
    lcos_local
    lock_registration
    logging
    memory
    mpi_base
    pack_traversal
    plugin
    prefix
    preprocessor
    program_options
    properties
    resiliency
    resource_partitioner
    runtime_configuration
    runtime_local
    schedulers
    serialization
    static_reinit
    string_util
    synchronization
    tag_invoke
    testing
    thread_pool_util
    thread_pools
    thread_support
    threading
    threading_base
    threadmanager
    timed_execution
    timing
    topology
    type_support
    util
    version_local
)
# cmake-format: on

hpx_local_info("")
hpx_local_info("  Configuring libhpx_local modules:")

foreach(module ${_hpx_local_modules})
  add_subdirectory(${module})
endforeach()
