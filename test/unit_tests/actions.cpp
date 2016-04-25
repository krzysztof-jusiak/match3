//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <range/v3/algorithm/equal.hpp>
#include "controller/controller.hpp"
#include "model/board.hpp"

namespace match3 {

test $drop_item = [] {
  selected s = {1};
  drop_item(s);
  expect(s.empty());
};

test $clear_selected = [] {
  selected s = {1, 2, 3};
  clear_selected(s);
  expect(s.empty());
};

test $swap_items = [] {
  board b;
  b.grids = {1, 2};
  selected s = {0, 1};
  swap_items(s, b);
  expect(ranges::equal({2, 1}, b.grids));
};

} // match3
