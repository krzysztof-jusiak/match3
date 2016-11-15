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
#include "controller/data/moves.hpp"
#include "controller/data/points.hpp"
#include "controller/data/randomize.hpp"
#include "controller/data/selected.hpp"
#include "model/board.hpp"
#include "view/view.hpp"

namespace match3 {

const auto reset = [](config c, board original, board& b, points& p, moves& m,
                      view& v) {
  b = original;
  p = {};
  m = c.max_moves;
  v.clear();
  v.update();
};

}  // match3
