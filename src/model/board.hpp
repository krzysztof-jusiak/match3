#pragma once

#include "fwd.hpp"

namespace match3 {

struct board {
  using color = int;
  std::vector<color> grids;
};

} // match3
