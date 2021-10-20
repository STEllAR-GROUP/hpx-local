//  Copyright (c) 2019-2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/modules/collectives.hpp>
#include <hpx/modules/testing.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace hpx::collectives;

constexpr char const* all_to_all_direct_basename = "/test/all_to_all_direct/";

void test_one_shot_use()
{
    std::uint32_t this_locality = hpx::get_locality_id();
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);

    // test functionality based on immediate local result value
    for (int i = 0; i != 10; ++i)
    {
        std::vector<std::uint32_t> values(num_localities);
        std::fill(values.begin(), values.end(), this_locality);

        hpx::future<std::vector<std::uint32_t>> overall_result =
            all_to_all(all_to_all_direct_basename, std::move(values),
                num_sites_arg(num_localities), this_site_arg(this_locality),
                generation_arg(i));

        std::vector<std::uint32_t> r = overall_result.get();
        HPX_TEST_EQ(r.size(), num_localities);

        for (std::size_t j = 0; j != r.size(); ++j)
        {
            HPX_TEST_EQ(r[j], j);
        }
    }
}

void test_multiple_use()
{
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::uint32_t this_locality = hpx::get_locality_id();

    auto all_to_all_direct_client =
        create_communicator(all_to_all_direct_basename,
            num_sites_arg(num_localities), this_site_arg(this_locality));

    // test functionality based on immediate local result value
    for (int i = 0; i != 10; ++i)
    {
        std::vector<std::uint32_t> values(num_localities);
        std::fill(values.begin(), values.end(), this_locality);

        hpx::future<std::vector<std::uint32_t>> overall_result =
            all_to_all(all_to_all_direct_client, std::move(values));

        std::vector<std::uint32_t> r = overall_result.get();
        HPX_TEST_EQ(r.size(), num_localities);

        for (std::size_t j = 0; j != r.size(); ++j)
        {
            HPX_TEST_EQ(r[j], j);
        }
    }
}

int hpx_main()
{
    test_one_shot_use();
    test_multiple_use();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    std::vector<std::string> const cfg = {"hpx.run_hpx_main!=1"};

    hpx::init_params init_args;
    init_args.cfg = cfg;

    HPX_TEST_EQ(hpx::init(argc, argv, init_args), 0);
    return hpx::util::report_errors();
}
#endif
