//  Copyright (c) 2007-2016 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#include <hpx/plugins/binary_filter/bzip2_serialization_filter_registration.hpp>
#include <hpx/plugins/binary_filter/snappy_serialization_filter_registration.hpp>
#include <hpx/plugins/binary_filter/zlib_serialization_filter_registration.hpp>

#if HPX_HAVE_DEPRECATION_WARNINGS
#if defined(HPX_MSVC)
#pragma message(                                                               \
    "The header hpx/include/compression_registration.hpp is deprecated, \
    please include directly the corresponding header \
    hpx/plugins/binary_filter/<libname>_serialization_filter_registration.hpp instead")
#else
#warning "The header hpx/include/compression_registration.hpp is deprecated, \
    please include directly the corresponding header \
    hpx/plugins/binary_filter/<libname>_serialization_filter_registration.hpp instead"
#endif
#endif
