#pragma once

#include <vector>

namespace match3 {

struct board {
  using color = int;
  std::vector<color> grids;
};

} // match3
