//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "pph.hpp"

namespace match3 {

template<int Min, int Max>
using safe_int = int;

struct board {
  using color = safe_int<0, 5>;
  std::vector<color> grids;
};

}  // match3
