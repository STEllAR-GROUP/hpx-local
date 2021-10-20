//  Copyright (c) 2007-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace hpx { namespace traits {
    ///////////////////////////////////////////////////////////////////////////
    // Customization point for Action::does_termination_detection
    template <typename Action, typename Enable = void>
    struct action_does_termination_detection
    {
        static constexpr bool call() noexcept
        {
            return false;
        }
    };
}}    // namespace hpx::traits
