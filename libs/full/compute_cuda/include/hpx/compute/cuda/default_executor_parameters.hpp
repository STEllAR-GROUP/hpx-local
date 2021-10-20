//  Copyright (c) 2016 Hartmut Kaiser
//  Copyright (c) 2016 Thomas Heller
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_GPU_SUPPORT)
#include <hpx/execution_base/traits/is_executor_parameters.hpp>

#include <cstddef>
#include <type_traits>

namespace hpx { namespace cuda { namespace experimental {
    struct default_executor_parameters
    {
        template <typename Executor, typename F>
        std::size_t get_chunk_size(Executor&, F&&, std::size_t, std::size_t)
        {
            return std::size_t(-1);
        }
    };
}}}    // namespace hpx::cuda::experimental

namespace hpx { namespace parallel { namespace execution {
    template <>
    struct is_executor_parameters<
        cuda::experimental::default_executor_parameters> : std::true_type
    {
    };
}}}    // namespace hpx::parallel::execution

#endif
