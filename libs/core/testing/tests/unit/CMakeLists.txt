# Copyright (c) 2020 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(tests test_macros)

foreach(test ${tests})

  set(sources ${test}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  hpx_local_add_executable(
    ${test}_test INTERNAL_FLAGS
    SOURCES ${sources} ${${test}_FLAGS}
    EXCLUDE_FROM_ALL
    HPX_PREFIX ${HPX_BUILD_PREFIX}
    FOLDER "Tests/Unit/Modules/Local/Testing"
  )

  hpx_local_add_unit_test("modules.testing" ${test} ${${test}_PARAMETERS})

endforeach()
