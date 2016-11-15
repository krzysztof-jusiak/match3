//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <uitlity>

template <class T>
auto make_click_event(int x, int y) {
  union {
    struct {
      const int x, y = 0;
    } fake;
    T event;
  } click{{x * 43 + 25, y * 43 + 65}};
  return click.event;
};

template <class SM>
void swipe(SM& sm, std::pair<int, int> from, std::pair<int, int> to) {
  sm.process_event(make_click_event<match3::down>(from.first, from.second));
  sm.process_event(make_click_event<match3::up>(to.first, to.second));
};
