//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "view/view.hpp"
#include "controller/data/selected.hpp"
#include "controller/events/up.hpp"
#include "controller/events/down.hpp"

namespace match3 {

const auto select_item = [](const auto& event, const view& v, selected& s) {
  s.emplace_back(v.get_position(event.x, event.y));
};

} // match3
