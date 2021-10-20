//  Copyright (c) 2007-2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/local/execution.hpp>
#include <hpx/local/init.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/modules/timed_execution.hpp>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iterator>
#include <string>
#include <vector>

using namespace std::chrono;

///////////////////////////////////////////////////////////////////////////////
hpx::thread::id test(int passed_through)
{
    HPX_TEST_EQ(passed_through, 42);
    return hpx::this_thread::get_id();
}

void test_timed_sync()
{
    typedef hpx::execution::parallel_executor executor;

    executor exec;
    HPX_TEST(hpx::parallel::execution::sync_execute_after(exec, milliseconds(1),
                 &test, 42) != hpx::this_thread::get_id());

    HPX_TEST(hpx::parallel::execution::sync_execute_at(exec,
                 steady_clock::now() + milliseconds(1), &test,
                 42) != hpx::this_thread::get_id());
}

void test_timed_async()
{
    typedef hpx::execution::parallel_executor executor;

    executor exec;
    HPX_TEST(hpx::parallel::execution::async_execute_after(
                 exec, milliseconds(1), &test, 42)
                 .get() != hpx::this_thread::get_id());
    HPX_TEST(hpx::parallel::execution::async_execute_at(
                 exec, steady_clock::now() + milliseconds(1), &test, 42)
                 .get() != hpx::this_thread::get_id());
}

void test_timed_apply()
{
    typedef hpx::execution::parallel_executor executor;

    executor exec;
    hpx::parallel::execution::post_after(exec, milliseconds(1), &test, 42);
    hpx::parallel::execution::post_at(
        exec, steady_clock::now() + milliseconds(1), &test, 42);
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main()
{
    test_timed_sync();
    test_timed_async();
    test_timed_apply();

    return hpx::local::finalize();
}

int main(int argc, char* argv[])
{
    // By default this test should run on all available cores
    std::vector<std::string> const cfg = {"hpx.os_threads=all"};

    // Initialize and run HPX
    hpx::local::init_params init_args;
    init_args.cfg = cfg;

    HPX_TEST_EQ_MSG(hpx::local::init(hpx_main, argc, argv, init_args), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
