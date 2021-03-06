# Copyright (c) 2015 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# add inpect executable

include(HPXLocal_SetupBoostRegex)

hpx_local_add_executable(
  inspect_local INTERNAL_FLAGS AUTOGLOB NOLIBS FOLDER "Tools/Local/Inspect"
)

if(NOT Boost_REGEX_FOUND)
  hpx_local_error("HPXLocal inspect tool requires Boost.Regex")
endif()

# Set the basic search paths for the generated HPX headers
target_link_libraries(
  inspect_local PRIVATE Boost::regex hpx_local_dependencies_boost hpx_local
)

# add dependencies to pseudo-target
hpx_local_add_pseudo_dependencies(tools.inspect inspect_local)
