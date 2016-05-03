//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <range/v3/algorithm/equal.hpp>
#include "controller/controller.hpp"
#include "model/board.hpp"

namespace match3 {

test $drop_item = [] {
  selected s = {1};
  drop_item(s);
  expect(s.empty());
};

test $clear_selected = [] {
  selected s = {1, 2, 3};
  clear_selected(s);
  expect(s.empty());
};

test $swap_items = [] {
  board b;
  b.grids = {1, 2};
  selected s = {0, 1};
  swap_items(s, b);
  expect(ranges::equal({2, 1}, b.grids));
};

test $find_matches_empty = [] {
  board b;
  b.grids = {1, 2, 3, 4};
  selected s = {0, 1};
  struct {
    int arity;
    std::vector<board::color> matches;
  } event{int(s.size())};

  find_matches(b, event, s, config{.board_width = 2});

  expect(s.empty());
  expect(event.matches.empty());
};

test $find_matches = [] {
  board b;
  b.grids = {1, 2, 3,
             1, 3, 2,
             1, 2, 3};
  selected s = {0};
  struct {
    int arity;
    std::vector<board::color> matches;
  } event{int(s.size())};

  find_matches(b, event, s, config{.board_width = 3});

  expect(s.empty());
  expect(ranges::equal({0, 3, 6}, event.matches));
};

test $find_matches_more = [] {
  board b;
  b.grids = {1, 2, 3,
             1, 1, 1,
             1, 2, 3};
  selected s = {1, 3};
  struct {
    int arity;
    std::vector<board::color> matches;
  } event{int(s.size())};

  find_matches(b, event, s, config{.board_width = 3});

  expect(s.empty());
  expect(ranges::equal({0, 3, 4, 5, 6}, event.matches));
};

test $destroy_matches = [] {
  board b;
  b.grids = {4, 2, 3,
             3, 2, 2,
             1, 2, 3};
  struct { std::vector<board::color> matches; } event{{1, 4, 7}};

  destroy_matches(b, event);

  expect(ranges::equal({4, 0, 3,
                        3, 0, 2,
                        1, 0, 3}, b.grids));
};

test $scroll_board = [] {
  board b;
  b.grids = {4, 3, 3,
             3, 2, 2,
             0, 0, 0};
  struct { std::vector<board::color> matches; } event{{6, 7, 8}};

  scroll_board(b, event, config{.board_width=3});

  expect(ranges::equal({0, 0, 0,
                        4, 3, 3,
                        3, 2, 2}, b.grids));
};

test $generate_new = [] {
  static constexpr auto NEW = 42;
  board b;
  b.grids = {4, 0, 3,
             3, 0, 2,
             1, 0, 3};
  selected s = {0, 5};
  struct { std::vector<board::color> matches; } event{{1, 4, 7}};

  generate_new(b, event, s, config{.board_width = 3}, [](auto...) { return NEW; });

  expect(ranges::equal({4, NEW, 3,
                        3, NEW, 2,
                        1, NEW, 3}, b.grids));
  expect(ranges::equal({0, 1, 4, 5, 7}, s));
};

}  // match3
