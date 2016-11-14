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
#include "controller/events/matches.hpp"
#include "model/board.hpp"

namespace match3 {

const auto find_matches = [](const board& b, const auto& m, selected& s) {
  assert(m.arity >= 0);
  auto arity = m.arity;
  while (arity--) {
    m.matches |= ranges::action::push_back(b.match(s.back()));
    s.pop_back();
  }
  m.matches |= ranges::action::sort | ranges::action::unique;
};

}  // match3
