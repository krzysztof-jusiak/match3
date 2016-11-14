// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/actions/swap_items.hpp"
#include <range/v3/algorithm/equal.hpp>
#include "config.hpp"
#include "model/board.hpp"

int main() {
  using namespace match3;

  "swap items"_test = [] {
    board b{{1, 2}, config{}};
    selected s = {0, 1};
    swap_items(s, b);
    expect(ranges::equal({2, 1}, b.grids));
  };
}
