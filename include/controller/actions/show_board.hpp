//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "config.hpp"
#include "model/board.hpp"
#include "view/animations.hpp"
#include "view/view.hpp"

namespace match3 {

const auto show_board = [](const board& b, animations& a, view& v,
                           const config c) {
  using namespace std::chrono_literals;
  a.queue_animation([b, c, &v] {
    for (auto i = 0; i < c.board_width * c.board_height; ++i) {
      v.set_grid(i % c.board_width, i / c.board_width, b[i]);
    }
  });
};

}  // match3
