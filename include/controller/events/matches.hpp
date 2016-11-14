//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <vector>

namespace match3 {

struct matches {
  static constexpr auto id = __COUNTER__;
  int arity = 0;
  mutable std::vector<short> matches;
};

} // match3
