//  Copyright (c) 2019 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  This work is inspired by https://github.com/aprell/tasking-2.0

#include <hpx/local/future.hpp>
#include <hpx/local/init.hpp>
#include <hpx/local/thread.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/synchronization/channel_mpsc.hpp>

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

constexpr int NUM_WORKERS = 1000;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
inline T channel_get(hpx::lcos::local::channel_mpsc<T> const& c)
{
    T result;
    while (!c.get(&result))
    {
        hpx::this_thread::yield();
    }
    return result;
}

template <typename T>
inline void channel_set(hpx::lcos::local::channel_mpsc<T>& c, T val)
{
    while (!c.set(std::move(val)))    // NOLINT
    {
        hpx::this_thread::yield();
    }
}

///////////////////////////////////////////////////////////////////////////////
int thread_func(int i, hpx::lcos::local::channel_mpsc<int>& channel,
    hpx::lcos::local::channel_mpsc<int>& next)
{
    channel_set(channel, i);
    return channel_get(next);
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main()
{
    std::vector<hpx::lcos::local::channel_mpsc<int>> channels;
    channels.reserve(NUM_WORKERS);

    std::vector<hpx::future<int>> workers;
    workers.reserve(NUM_WORKERS);

    for (int i = 0; i != NUM_WORKERS; ++i)
    {
        channels.emplace_back(std::size_t(1));
    }

    for (int i = 0; i != NUM_WORKERS; ++i)
    {
        workers.push_back(hpx::async(&thread_func, i, std::ref(channels[i]),
            std::ref(channels[(i + 1) % NUM_WORKERS])));
    }

    hpx::wait_all(workers);

    for (int i = 0; i != NUM_WORKERS; ++i)
    {
        HPX_TEST_EQ((i + 1) % NUM_WORKERS, workers[i].get());
    }

    hpx::local::finalize();
    return hpx::util::report_errors();
}

int main(int argc, char* argv[])
{
    return hpx::local::init(hpx_main, argc, argv);
}
