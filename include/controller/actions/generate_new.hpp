//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <cassert>
#include "controller/data/selected.hpp"
#include "controller/data/randomize.hpp"
#include "model/board.hpp"
#include "config.hpp"

namespace match3 {

const auto generate_new = [](board& b, const auto& m, selected& s,
                             const config c, randomize r) {
  ranges::action::transform(
      b.grids, [c, r](auto i) { return i ? i : r(1, c.board_colors); });
  s |= ranges::action::push_front(
           board_logic::affected(m.matches, c.board_width)) |
       ranges::action::sort | ranges::action::unique;
};

} // match3
