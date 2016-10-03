#
# Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
include_directories(${CMAKE_CURRENT_LIST_DIR})

function(test test)
    string(REPLACE "/" "_" tmp ${test})
    add_executable(test.${tmp} ${CMAKE_CURRENT_LIST_DIR}/${test}.cpp)
    add_test(test.${tmp} test.${tmp})
endfunction()

include_directories(.)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -include common/test.hpp")

test(unit_tests/actions)
test(unit_tests/guards)
test(unit_tests/board)
test(functional_tests)
test(integration_tests)
