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
#include "model/board.hpp"

namespace match3 {

const auto swap_items = [](const selected& s, board& b) {
  assert(s.size() == 2);
  b.swipe(s[0], s[1]);
};

}  // match3
