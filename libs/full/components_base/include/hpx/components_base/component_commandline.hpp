//  Copyright (c) 2007-2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/modules/program_options.hpp>
#include <hpx/runtime_configuration/component_commandline_base.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components {

    ///////////////////////////////////////////////////////////////////////////
    namespace commandline_options_provider {

        hpx::program_options::options_description add_commandline_options();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// The \a component_startup_shutdown provides a minimal implementation of
    /// a component's startup/shutdown function provider.
    ///
    struct component_commandline : component_commandline_base
    {
        /// \brief Return any additional command line options valid for this
        ///        component
        ///
        /// \return The module is expected to fill a options_description object
        ///         with any additional command line options this component
        ///         will handle.
        ///
        /// \note   This function will be executed by the runtime system
        ///         during system startup.
        hpx::program_options::options_description add_commandline_options()
            override
        {
            return commandline_options_provider::add_commandline_options();
        }
    };
}}    // namespace hpx::components

///////////////////////////////////////////////////////////////////////////////
#define HPX_DEFINE_COMPONENT_COMMANDLINE_OPTIONS(add_options_function)         \
    namespace hpx { namespace components {                                     \
            namespace commandline_options_provider {                           \
                hpx::program_options::options_description                      \
                add_commandline_options()                                      \
                {                                                              \
                    return add_options_function();                             \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }                                                                          \
    /***/

///////////////////////////////////////////////////////////////////////////////
#define HPX_REGISTER_COMMANDLINE_MODULE(add_options_function)                  \
    HPX_REGISTER_COMMANDLINE_OPTIONS()                                         \
    HPX_REGISTER_COMMANDLINE_REGISTRY(                                         \
        hpx::components::component_commandline, commandline_options)           \
    HPX_DEFINE_COMPONENT_COMMANDLINE_OPTIONS(add_options_function)             \
    /**/
#define HPX_REGISTER_COMMANDLINE_MODULE_DYNAMIC(add_options_function)          \
    HPX_REGISTER_COMMANDLINE_OPTIONS_DYNAMIC()                                 \
    HPX_REGISTER_COMMANDLINE_REGISTRY_DYNAMIC(                                 \
        hpx::components::component_commandline, commandline_options)           \
    HPX_DEFINE_COMPONENT_COMMANDLINE_OPTIONS(add_options_function)             \
    /**/
