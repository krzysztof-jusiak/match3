//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <cassert>
#include "config.hpp"
#include "controller/data/randomize.hpp"
#include "controller/data/selected.hpp"
#include "model/board.hpp"

namespace match3 {

const auto generate_new = [](board& b, selected& s, const config c,
                             randomize r) {
  ranges::action::transform(
      b.grids, [c, r](auto i) { return i ? i : r(1, c.board_colors); });
  s |= ranges::action::push_front(
           ranges::view::ints |
           ranges::view::take(c.board_width * c.board_height)) |
       ranges::action::sort | ranges::action::unique;
};

}  // match3
