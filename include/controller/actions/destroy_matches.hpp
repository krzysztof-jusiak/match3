//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "controller/events/matches.hpp"
#include "model/board.hpp"

namespace match3 {

const auto destroy_matches = [](board& b, const auto& m) {
  ranges::for_each(m.matches, [&](auto i) { b.grids[i] = {}; });
};

}  // match3
