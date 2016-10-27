//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "config.hpp"
#include "model/board.hpp"
#include "pph.hpp"
#include "view/animations.hpp"
#include "view/view.hpp"

namespace sml = boost::sml;

namespace match3 {

using selected = std::vector<short>;
using randomize = std::function<int(int, int)>;
using points = int;
using moves = short;

struct matches {
  static constexpr auto id = __LINE__;
  int arity = 0;
  mutable std::vector<int> matches;
};

struct time_tick {
  static constexpr auto id = __LINE__;
};

/// Guards

const auto has_items = [](const selected& s) { return !s.empty(); };

const auto is_item_winning = [](const board& b, const selected& s) {
  return !s.empty() && b.is_match(s.back());
};

/// Actions

const auto drop_item = [](selected& s) {
  assert(!s.empty());
  s.pop_back();
};

const auto find_matches = [](const board& b, const auto& m, selected& s) {
  assert(m.arity >= 0);
  auto arity = m.arity;
  while (arity--) {
    m.matches |= ranges::action::push_back(b.match(s.back()));
    s.pop_back();
  }
  m.matches |= ranges::action::sort | ranges::action::unique;
};

const auto destroy_matches = [](board& b, const auto& m) {
  ranges::for_each(m.matches, [&](auto i) { b.grids[i] = {}; });
};

const auto scroll_board = [](board& b, const auto& m) {
  ranges::for_each(m.matches, [&](auto i) { b.scroll(i); });
};

const auto generate_new = [](board& b, const auto& m, selected& s,
                             const config c, randomize r) {
  ranges::action::transform(
      b.grids, [c, r](auto i) { return i ? i : r(1, c.board_colors); });
  s |= ranges::action::push_front(
           board_logic::affected(m.matches, c.board_width)) |
       ranges::action::sort | ranges::action::unique;
};

const auto add_points = [](points& p, const auto& m) { p += m.matches.size(); };

const auto show_board = [](const board& b, animations& a, view& v,
                           const config c) {
  using namespace std::chrono_literals;
  a.queue_animation(
      [b, c, &v] {
        for (auto i = 0; i < c.board_width * c.board_height; ++i) {
          v.set_grid(i % c.board_width, i / c.board_width, b[i]);
        }
      },
      150ms);
};

const auto show_matches = [](const auto& m, animations& a, view& v,
                             const config c) {
  using namespace std::chrono_literals;
  a.queue_animation(
      [m, c, &v] {
        for (auto i : m.matches) {
          v.update_grid(i % c.board_width, i / c.board_width);
        }
      },
      150ms);
};

const auto show_points = [](view& v, const points& p, animations& a) {
  using namespace std::chrono_literals;
  a.queue_animation(
      [p, &v] { v.set_text("points: " + std::to_string(p), 10, 10); });
};

struct switcher {
  auto operator()() const {
    using namespace sml;

    // clang-format off
    return make_transition_table(
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
         *("handle matches"_s)      + event<matches>     [ has_items and is_item_winning ] / (
                                                            find_matches, show_matches
                                                          , destroy_matches, show_board
                                                          , add_points, show_points
                                                          , scroll_board, show_board
                                                          , generate_new, show_board
                                                          , process(matches{.arity = 1})
                                                         )
      ,    "handle matches"_s       + event<matches>     [ has_items and not is_item_winning ] / (
                                                           drop_item, process(matches{.arity = 1})
                                                         )
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      //,  *("handle_animations"_s)   + event<time_tick>   / [](animations& a) { a.update(); }
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
    );
    // clang-format on
  }
};

}  // match3
