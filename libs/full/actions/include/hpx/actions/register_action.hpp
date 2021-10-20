//  Copyright (c) 2016 Thomas Heller
//  Copyright (c) 2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/actions/actions_fwd.hpp>
#include <hpx/actions_base/actions_base_support.hpp>
#include <hpx/actions_base/detail/action_factory.hpp>

#if defined(HPX_HAVE_NETWORKING)

namespace hpx { namespace actions { namespace detail {

    template <typename Action>
    struct register_action
    {
        register_action(register_action const&) = delete;
        register_action(register_action&&) = delete;
        register_action& operator=(register_action const&) = delete;
        register_action& operator=(register_action&&) = delete;

        register_action();

        // defined in actions/transfer_action.hpp
        static base_action* create();

        // defined in async_distributed/transfer_continuation_action.hpp
        static base_action* create_cont();

        register_action& instantiate();

        static register_action instance;
    };

    template <typename Action>
    register_action<Action> register_action<Action>::instance;

    template <typename Action>
    register_action<Action>::register_action()
    {
        action_registry::instance().register_factory(
            hpx::actions::detail::get_action_name<Action>(), &create,
            &create_cont);
    }

    template <typename Action>
    register_action<Action>& register_action<Action>::instantiate()
    {
        return *this;
    }
}}}    // namespace hpx::actions::detail

#endif
