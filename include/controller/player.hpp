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

struct down {
  static constexpr auto id = EM(SDL_FINGERDOWN)(SDL_MOUSEBUTTONDOWN);
  explicit down(const SDL_Event& event)
      : EM(x(int(event.tfinger.x)), y(int(event.tfinger.y)))(
            x(event.button.x), y(event.button.y)) {}
  const int x = 0;
  const int y = 0;
};

struct up {
  static constexpr auto id = EM(SDL_FINGERUP)(SDL_MOUSEBUTTONUP);
  explicit up(const SDL_Event& event)
      : EM(x(int(event.tfinger.x)), y(int(event.tfinger.y)))(
            x(event.button.x), y(event.button.y)) {}
  const int x = 0;
  const int y = 0;
};

struct quit {
  static constexpr auto id = SDL_QUIT;
};

struct key_pressed {
  static constexpr auto id = SDL_KEYDOWN;
  explicit key_pressed(const SDL_Event& event) : key(event.key.keysym.sym) {}
  int key = 0;
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

//const auto drop_item = [](selected& s) {
  //assert(!s.empty());
  //s.pop_back();
//};

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

//const auto show_board = [](const board& b, animations& a, view& v,
                           //const config c) {
  //using namespace std::chrono_literals;
  //a.queue_animation(
      //[b, c, &v] {
        //for (auto i = 0; i < c.board_width * c.board_height; ++i) {
          //v.set_grid(i % c.board_width, i / c.board_width, b[i]);
        //}
      //},
      //150ms);
//};

//const auto show_points = [](view& v, const points& p, animations& a) {
  //using namespace std::chrono_literals;
  //a.queue_animation(
      //[p, &v] { v.set_text("points: " + std::to_string(p), 10, 10); });
//};

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
    return make_transition_table(
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
        "wait for first item"_s  <= *("idle"_s)                                     / reset_and_show
      //, "wait_for_click"_s       <=   "wait for first item"_s                       [ ([](moves& m) { return not m; }) ] / show_game_over
      //, "wait for first item"_s  <=   "wait_for_click"_s       + event<up>          / reset_and_show
      , "wait for second item"_s <=   "wait for first item"_s  + event<down>        / select_item
      , "wait for first item"_s  <=   "wait for second item"_s + event<up>          [ not is_allowed ] / drop_item
      , "match items"_s          <=   "wait for second item"_s + event<up>          [ is_allowed ] / select_and_swap_items
      , "wait for first item"_s  <=   "match items"_s                               [ not is_swap_items_winning] / (swap_items, show_swap, clear_selected)
      , state<switcher>          <=   "match items"_s                               [ is_swap_items_winning ] / ([](moves& m) {--m;}, show_moves, process(matches{.arity = 2}))
      , "wait for second item"_s <=   state<switcher>          + event<down>        / select_item
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      , X                        <= *("wait for player"_s)     + event<key_pressed> [ is_key(SDLK_ESCAPE) ]
      , X                        <=   "wait for player"_s      + event<quit>
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      ,  *("handle animations"_s)   + event<time_tick>   / [](animations& a) { a.update(); }
    );
    // clang-format on
  }
};

}  // match3
