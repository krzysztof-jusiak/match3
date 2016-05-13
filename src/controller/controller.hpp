//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "pph.hpp"
#include "model/config.hpp"
#include "model/board.hpp"
#include "view/view.hpp"
#include "view/animations.hpp"
#include "controller/logic.hpp"

namespace msm = boost::msm::lite;

namespace match3 {

using selected = std::vector<short>;
using randomize = std::function<int(int, int)>;
using points = int;
using moves = unsigned int;

/// Events

struct touch_down {
  static constexpr auto id = SDL_FINGERDOWN;
  explicit touch_down(const SDL_Event& event)
      : x(int(event.tfinger.x)), y(int(event.tfinger.y)) {}
  const int x = 0;
  const int y = 0;
};

struct button_down {
  static constexpr auto id = SDL_MOUSEBUTTONDOWN;
  explicit button_down(const SDL_Event& event)
      : x(event.button.x), y(event.button.y) {}
  const int x = 0;
  const int y = 0;
};

struct touch_up {
  static constexpr auto id = SDL_FINGERUP;
  explicit touch_up(const SDL_Event& event)
      : x(int(event.tfinger.x)), y(int(event.tfinger.y)) {}
  const int x = 0;
  const int y = 0;
};

struct button_up {
  static constexpr auto id = SDL_MOUSEBUTTONUP;
  explicit button_up(const SDL_Event& event)
      : x(event.button.x), y(event.button.y) {}
  const int x = 0;
  const int y = 0;
};

struct matches {
  static constexpr auto id = __LINE__;
  int arity = 0;
  mutable std::vector<int> matches;
};

struct time_tick {
  static constexpr auto id = __LINE__;
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

const auto is_mobile = [] {
  // clang-format off
  return bool(EM_ASM_INT_V({return /iPhone|iPad|iPod|Android/i.test(navigator.userAgent);}));
  // clang-format on
};

const auto has_items = [](const selected& s) { return !s.empty(); };

const auto is_swap_items_winning = [](const board& b, const selected& s,
                                      const config c) {
  assert(s.size() >= 2);
  return is_match(b.grids, s[0], c.board_width) ||
         is_match(b.grids, s[1], c.board_width);
};

const auto is_item_winning = [](const board& b, const selected& s,
                                const config c) {
  return !s.empty() && is_match(b.grids, s.back(), c.board_width);
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

const auto drop_item = [](selected& s) {
  assert(!s.empty());
  s.pop_back();
};

const auto clear_selected = [](selected& s) { s.clear(); };

const auto swap_items = [](const selected& s, board& b) {
  assert(s.size() >= 2);
  std::swap(b.grids[s[0]], b.grids[s[1]]);
};

const auto find_matches = [](const board& b, auto& m, selected& s,
                             const config c) {
  assert(m.arity >= 0);
  auto arity = m.arity;
  while (arity--) {
    m.matches |=
        ranges::action::push_back(match(b.grids, s.back(), c.board_width));
    s.pop_back();
  }
  m.matches |= ranges::action::sort | ranges::action::unique;
};

const auto destroy_matches = [](board& b, const auto& m) {
  ranges::for_each(m.matches, [&](auto i) { b.grids[i] = {}; });
};

const auto scroll_board = [](board& b, const auto& m, const config c) {
  ranges::for_each(m.matches,
                   [&](auto i) { scroll(b.grids, i, c.board_width); });
};

const auto generate_new = [](board& b, const auto& m, selected& s,
                             const config c, randomize r) {
  ranges::action::transform(
      b.grids, [c, r](auto i) { return i ? i : r(1, c.board_colors); });
  s |= ranges::action::push_front(affected(m.matches, c.board_width)) |
       ranges::action::sort | ranges::action::unique;
};

const auto add_points = [](points& p, const auto& m) { p += m.matches.size(); };

const auto reset = [](config c, board original, board& b, points& p, moves& m,
                      view& v) {
  b = original;
  p = {};
  m = c.max_moves;
  v.clear();
};

const auto show_swap = [](const board& b, const selected& s, animations& a,
                          view& v, const config c) {
  assert(s.size() >= 2);
  using namespace std::chrono_literals;
  a.queue_animation([b, c, s, &v] {
    const auto _1 = s[0];
    const auto _2 = s[1];
    v.set_grid(_1 % c.board_width, _1 / c.board_width, b.grids[_1]);
    v.set_grid(_2 % c.board_width, _2 / c.board_width, b.grids[_2]);
  }, 150ms);
};

const auto show_board = [](const board& b, animations& a, view& v,
                           const config c) {
  using namespace std::chrono_literals;
  a.queue_animation([b, c, &v] {
    for (auto i = 0; i < c.board_width * c.board_height; ++i) {
      v.set_grid(i % c.board_width, i / c.board_width, b.grids[i]);
    }
  }, 150ms);
};

const auto show_matches = [](const auto& m, animations& a, view& v,
                             const config c) {
  using namespace std::chrono_literals;
  a.queue_animation([m, c, &v] {
    for (auto i : m.matches) {
      v.update_grid(i % c.board_width, i / c.board_width);
    }
  }, 150ms);
};

const auto show_points = [](view& v, const points& p, animations& a) {
  using namespace std::chrono_literals;
  a.queue_animation(
      [p, &v] { v.set_text("points: " + std::to_string(p), 10, 10); });
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
  auto configure() const noexcept {
    using namespace msm;

    const auto reset_and_show = (reset, show_board, show_points, show_moves);
    const auto select_and_swap_items = (select_item, swap_items, show_swap);

    // clang-format off
    return make_transition_table(
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
        "wait_for_first_item"_s  <= *("idle"_s)                                     / reset_and_show

      ,  "wait_for_click"_s      <=   "wait_for_first_item"_s                       [ ([](moves& m) { return not m; }) ] / show_game_over
      ,  "wait_for_first_item"_s <=   "wait_for_click"_s       + event<touch_up>    [ is_mobile ] / reset_and_show
      ,  "wait_for_first_item"_s <=   "wait_for_click"_s       + event<button_up>   [ not is_mobile ] / reset_and_show

      , "wait_for_second_item"_s <=   "wait_for_first_item"_s  + event<touch_down>  [ is_mobile ] / select_item
      , "match_items"_s          <=   "wait_for_second_item"_s + event<touch_up>    [ is_mobile and is_allowed ] / select_and_swap_items
      , "wait_for_first_item"_s  <=   "wait_for_second_item"_s + event<touch_up>    [ is_mobile and not is_allowed ] / drop_item

      , "wait_for_second_item"_s <=   "wait_for_first_item"_s  + event<button_down> [ not is_mobile ] / select_item
      , "match_items"_s          <=   "wait_for_second_item"_s + event<button_up>   [ not is_mobile and is_allowed ] / select_and_swap_items
      , "wait_for_first_item"_s  <=   "wait_for_second_item"_s + event<button_up>   [ not is_mobile and not is_allowed ] / drop_item

      , "wait_for_first_item"_s  <=   "match_items"_s                               [ is_swap_items_winning ] / (
                                                                                      [](moves& m) {--m;}, show_moves,
                                                                                      msm::queue_event(matches{.arity = 2})
                                                                                    )
      , "wait_for_first_item"_s  <=   "match_items"_s                               / (swap_items, show_swap, clear_selected)
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      ,                             *("handle_matches"_s)      + event<matches>     [ has_items and is_item_winning ] / (
                                                                                       find_matches, show_matches
                                                                                     , destroy_matches, show_board
                                                                                     , add_points, show_points
                                                                                     , scroll_board, show_board
                                                                                     , generate_new, show_board
                                                                                     , msm::queue_event(matches{.arity = 1})
                                                                                    )
      ,                               "handle_matches"_s       + event<matches>     [ has_items and not is_item_winning ] / (
                                                                                      drop_item, msm::queue_event(matches{.arity = 1})
                                                                                    )
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      , X                        <= *("wait_for_client"_s)     + event<key_pressed> [ is_key(SDLK_ESCAPE) ]
      , X                        <=   "wait_for_client"_s      + event<quit>
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      ,                             *("handle_animations"_s)   + event<time_tick>   / [](animations& a) { a.update(); }
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
    );
    // clang-format on
  }
};

}  // match3
