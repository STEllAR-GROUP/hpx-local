//  Copyright (c) 2011 Bryce Adelstein-Lelbach
//  Copyright (c) 2012-2021 Hartmut Kaiser
//  Copyright (c) 2016 Thomas Heller
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/modules/errors.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace agas {

    // Register all performance counter types exposed by the component_namespace
    HPX_EXPORT void component_namespace_register_counter_types(
        error_code& ec = throws);
}}    // namespace hpx::agas
