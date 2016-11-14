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

namespace match3 {

const auto is_allowed = [](auto event, const view& v, const selected& s,
                           const config c) {
  assert(!s.empty());
  const auto _1 = s.back();
  const auto _2 = v.get_position(event.x, event.y);
  const auto diff = std::abs(_1 - _2);
  const auto board_size = c.board_width * c.board_height;
  return (_1 >= 0 && _1 < board_size) && (_2 >= 0 && _2 < board_size) &&
         (diff == 1 || diff == c.board_width);
};

}  // match3
