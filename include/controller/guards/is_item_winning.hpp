//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "controller/data/selected.hpp"
#include "model/board.hpp"

namespace match3 {

const auto is_item_winning = [](const board& b, const selected& s) {
  return !s.empty() && b.is_match(s.back());
};

} // match3
