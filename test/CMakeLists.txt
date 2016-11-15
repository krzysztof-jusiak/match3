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

test(unit_tests/controller/actions/clear_selected)
test(unit_tests/controller/actions/destroy_matches)
test(unit_tests/controller/actions/drop_item)
test(unit_tests/controller/actions/find_matches)
test(unit_tests/controller/actions/generate_new)
test(unit_tests/controller/actions/scroll_board)
test(unit_tests/controller/actions/swap_items)
test(unit_tests/controller/guards/has_items)
test(unit_tests/controller/guards/is_item_winning)
test(unit_tests/controller/guards/is_key)
test(unit_tests/model/board)

test(functional_tests/match3_matchl_no_moves_reset)
test(functional_tests/match5_out_of_moves)
