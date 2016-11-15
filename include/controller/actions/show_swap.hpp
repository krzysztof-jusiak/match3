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
#include "controller/data/selected.hpp"
#include "model/board.hpp"
#include "view/animations.hpp"
#include "view/view.hpp"

namespace match3 {

const auto show_swap = [](const board& b, const selected& s, animations& a,
                          view& v, const config c) {
  assert(s.size() == 2);
  using namespace std::chrono_literals;
  a.queue_animation([b, c, s, &v] {
    const auto _1 = s[0];
    const auto _2 = s[1];
    v.set_grid(_1 % c.board_width, _1 / c.board_width, b[_1]);
    v.set_grid(_2 % c.board_width, _2 / c.board_width, b[_2]);
  });
};

}  // match3
