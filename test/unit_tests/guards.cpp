//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/controller.hpp"
#include "model/board.hpp"
#include "model/config.hpp"

int main() {
  using namespace match3;

  "has items"_test = [] {
    selected s = {1, 2};
    expect(has_items(s));
  };

  "has items empty"_test = [] {
    selected s = {};
    expect(!has_items(s));
  };

  "is item no winning"_test = [] {
    board b;
    b.grids = {1, 2, 3, 4, 5, 6};
    expect(!is_item_winning(b, {0}, config{.board_width = 3}));
    expect(!is_item_winning(b, {1}, config{.board_width = 3}));
    expect(!is_item_winning(b, {2}, config{.board_width = 3}));
    expect(!is_item_winning(b, {3}, config{.board_width = 3}));
    expect(!is_item_winning(b, {4}, config{.board_width = 3}));
    expect(!is_item_winning(b, {5}, config{.board_width = 3}));
  };

  "is item winning"_test = [] {
    board b;
    b.grids = {1, 2, 3, 2, 2, 2};

    expect(!is_item_winning(b, {0}, config{.board_width = 3}));
    expect(!is_item_winning(b, {1}, config{.board_width = 3}));
    expect(!is_item_winning(b, {2}, config{.board_width = 3}));
    expect(is_item_winning(b, {3}, config{.board_width = 3}));
    expect(is_item_winning(b, {4}, config{.board_width = 3}));
    expect(is_item_winning(b, {5}, config{.board_width = 3}));
  };

  "is key"_test = [] {
    constexpr auto key = 42;
    struct {
      int key;
    } event{key};
    expect(is_key(key)(event));
  };

  "is not key"_test = [] {
    constexpr auto key = 42;
    struct {
      int key;
    } event{0};
    expect(!is_key(key)(event));
  };
}
