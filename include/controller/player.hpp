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
#include "controller/switcher.hpp"

namespace sml = boost::sml;

namespace match3 {

using selected = std::vector<short>;
using randomize = std::function<int(int, int)>;
using points = int;
using moves = short;

/// Events

struct down : sml::utility::id<SDL_FINGERDOWN, SDL_MOUSEBUTTONDOWN> {
  explicit down(const SDL_Event& event)
    : down(event, EM_ASM_INT_V({return /iPhone|iPad|iPod|Android/i.test(navigator.userAgent);}))
  { }

  explicit down(const SDL_Event& event, bool is_mobile)
    : x(is_mobile ? int(event.tfinger.x) : int(event.button.x))
    , y(is_mobile ? int(event.tfinger.y) : int(event.button.y))
  { }

  const int x, y = 0;
};

struct up : sml::utility::id<SDL_FINGERUP, SDL_MOUSEBUTTONUP> {
  explicit up(const SDL_Event& event)
    : up(event, EM_ASM_INT_V({return /iPhone|iPad|iPod|Android/i.test(navigator.userAgent);}))
  { }

  explicit up(const SDL_Event& event, bool is_mobile)
    : x(is_mobile ? int(event.tfinger.x) : int(event.button.x))
    , y(is_mobile ? int(event.tfinger.y) : int(event.button.y))
  { }

  const int x, y = 0;
};

struct quit {
  static constexpr auto id = SDL_QUIT;
};

struct key_pressed {
  static constexpr auto id = SDL_KEYDOWN;
  explicit key_pressed(const SDL_Event& event) : key(event.key.keysym.sym) {}
  const int key = 0;
};

/// Guards

const auto is_swap_items_winning = [](const board& b, const selected& s) {
  assert(s.size() == 2);
  return b.is_match(s[0]) || b.is_match(s[1]);
};

const auto is_allowed = [](auto event, const view& v, const selected& s,
                           const config c) {
  assert(!s.empty());
  const auto _1 = s.back();
  const auto _2 = v.get_position(event.x, event.y);
  const auto diff = std::abs(_1 - _2);
  const auto board_size = c.board_width * c.board_height;
  return (_1 >= 0 && _1 < board_size) && (_2 >= 0 && _2 < board_size) &&
         (diff == 1 || diff == c.board_width);
};

const auto is_key = [](int Key) {
  return [=](auto event) { return event.key == Key; };
};

/// Actions

const auto select_item = [](auto event, const view& v, selected& s) {
  s.emplace_back(v.get_position(event.x, event.y));
};

const auto clear_selected = [](selected& s) { s.clear(); };

const auto swap_items = [](const selected& s, board& b) {
  assert(s.size() == 2);
  std::swap(b[s[0]], b[s[1]]);
};

const auto reset = [](config c, board original, board& b, points& p, moves& m,
                      view& v) {
  b = original;
  p = {};
  m = c.max_moves;
  v.clear();
};

const auto show_swap = [](const board& b, const selected& s, animations& a,
                          view& v, const config c) {
  assert(s.size() == 2);
  using namespace std::chrono_literals;
  a.queue_animation(
      [b, c, s, &v] {
        const auto _1 = s[0];
        const auto _2 = s[1];
        v.set_grid(_1 % c.board_width, _1 / c.board_width, b[_1]);
        v.set_grid(_2 % c.board_width, _2 / c.board_width, b[_2]);
      },
      150ms);
};

const auto show_moves = [](view& v, const moves& m, animations& a) {
  using namespace std::chrono_literals;
  a.queue_animation(
      [m, &v] { v.set_text("moves: " + std::to_string(m), 240, 10); });
};

const auto show_game_over = [](view& v, animations& a) {
  using namespace std::chrono_literals;
  a.queue_animation(
      [&v] { v.set_text("GAME OVER", 20, 230, 48 /*font size*/); }, 1s);
};

struct controller {
  auto operator()() const {
    using namespace sml;

    const auto reset_and_show = (reset, show_board, show_points, show_moves);
    const auto select_and_swap_items = (select_item, swap_items, show_swap);

    /**
     * Transition table for player
     *
     * \code{.cpp}
     *   dst_state <= src_state + event [ guard ] / action
     * \endcode
     */
    // clang-format off
      //, "wait_for_click"_s       <=   "wait for first item"_s                       [ ([](moves& m) { return not m; }) ] / show_game_over
      //, "wait for first item"_s  <=   "wait_for_click"_s       + event<up>          / reset_and_show
    return make_transition_table(
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
        (*"idle"_s)                    / reset_and_show                          = "first item"_s,
         "first item"_s  + event<down> / select_item                             = "second item"_s,
         "second item"_s + event<up>   [ is_allowed ] / select_and_swap_items    = "match items"_s,
         "second item"_s + event<up>   / drop_item                               = "first item"_s,
         "match items"_s               [ is_swap_items_winning ] /
                                          process(matches{.arity=2})             = "switcher"_s.sm<switcher>(),
         "match items"_s               / (swap_items, show_swap, clear_selected) = "first item"_s,
         "switcher"_s.sm<switcher>()                                             = "first item"_s,
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
         (*"is click"_s) + event<key_pressed> [ is_key(SDLK_ESCAPE) ]         = X,
           "is click"_s  + event<quit>                                        = X,
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
        (*"anims"_s)    + event<time_tick> / [](animations& a) { a.update(); }
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
    );
    // clang-format on
  }
};

}  // match3
