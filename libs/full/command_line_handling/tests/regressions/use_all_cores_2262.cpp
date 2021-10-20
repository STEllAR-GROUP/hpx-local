//  Copyright (c) 2016 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This test verifies that 'hpx.os_threads=all' is equivalent to specifying
// all of the available cores (see #2262).

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/local/init.hpp>
#include <hpx/local/thread.hpp>
#include <hpx/modules/testing.hpp>

#include <string>
#include <vector>

int hpx_main()
{
    HPX_TEST_EQ(
        hpx::threads::hardware_concurrency(), hpx::get_os_thread_count());

    return hpx::local::finalize();
}

// Ignore all command line options to avoid any interference with the test
// runners.
char* argv[] = {const_cast<char*>("use_all_cores_2262"), nullptr};

int main()
{
    std::vector<std::string> cfg = {"hpx.os_threads=all"};

    hpx::local::init_params init_args;
    init_args.cfg = cfg;

    HPX_TEST_EQ(hpx::local::init(hpx_main, 1, argv, init_args), 0);
    return hpx::util::report_errors();
}
#endif
