//  Copyright (c) 2007-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#ifdef HPX_HAVE_APEX

#include <hpx/performance_counters/counters.hpp>
#include <hpx/threading_base/external_timer.hpp>

#include <cstdint>
#include <string>

namespace hpx { namespace util { namespace external_timer {

    // The actual function pointers. Some of them need to be exported,
    // because through the miracle of chained headers they get referenced
    // outside of the HPX library.
    static inline void sample_value(const std::string& name, double value)
    {
        if (sample_value_function != nullptr)
        {
            sample_value_function(name, value);
        }
    }
    static inline void sample_value(
        hpx::performance_counters::counter_info const& info, double value)
    {
        if (sample_value_function != nullptr)
        {
            sample_value_function(info.fullname_, value);
        }
    }
}}}    // namespace hpx::util::external_timer

#endif
