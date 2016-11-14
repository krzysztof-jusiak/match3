//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "controller/data/points.hpp"
#include "controller/events/matches.hpp"

namespace match3 {

const auto add_points = [](points& p, const auto& m) { p += m.matches.size(); };

}  // match3
