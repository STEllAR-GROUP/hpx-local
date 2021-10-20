//  Copyright (c)      2016 Thomas Heller
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/actions_base/actions_base_fwd.hpp>

namespace hpx { namespace actions {

    /// \cond NOINTERNAL

#if defined(HPX_HAVE_NETWORKING)
    struct base_action;
    struct HPX_EXPORT base_action_data;

    ///////////////////////////////////////////////////////////////////////////
    template <typename Action>
    struct transfer_action;

    template <typename Action>
    struct transfer_continuation_action;
#endif
    /// \endcond
}}    // namespace hpx::actions
