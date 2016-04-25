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
  std::string win_title;
  int win_width;
  int win_height;
  int board_width;
  int board_height;
  int board_colors;
  int max_moves;
};

}  // match3
