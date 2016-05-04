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

struct config {
  const std::string win_title;
  const int win_width;
  const int win_height;
  const int board_width;
  const int board_height;
  const int board_colors;
  const int max_moves;
};

}  // match3
