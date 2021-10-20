//  Copyright (c) 2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/components_base/component_type.hpp>
#include <hpx/modules/format.hpp>
#include <hpx/naming_base/address.hpp>
#include <hpx/util/ios_flags_saver.hpp>

#include <ostream>

namespace hpx { namespace naming {

    // this is defined in this module as its implementation relies on
    // components::get_component_type_name()
    std::ostream& operator<<(std::ostream& os, address const& addr)
    {
        return hpx::util::format_to(os, "({}:{}:{:#x})", addr.locality_,
            components::get_component_type_name(addr.type_), addr.address_);
    }
}}    // namespace hpx::naming
