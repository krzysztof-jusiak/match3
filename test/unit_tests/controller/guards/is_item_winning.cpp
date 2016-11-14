//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "config.hpp"
#include "controller/guards/is_item_winning.hpp"
#include "model/board.hpp"

int main() {
  using namespace match3;
  "is item no winning"_test = [] {
    board b{{1, 2, 3, 4, 5, 6}, config{.board_width = 3}};
    expect(!is_item_winning(b, {0}));
    expect(!is_item_winning(b, {1}));
    expect(!is_item_winning(b, {2}));
    expect(!is_item_winning(b, {3}));
    expect(!is_item_winning(b, {4}));
    expect(!is_item_winning(b, {5}));
  };

  "is item winning"_test = [] {
    board b{{1, 2, 3, 2, 2, 2}, config{.board_width = 3}};

    expect(!is_item_winning(b, {0}));
    expect(!is_item_winning(b, {1}));
    expect(!is_item_winning(b, {2}));
    expect(is_item_winning(b, {3}));
    expect(is_item_winning(b, {4}));
    expect(is_item_winning(b, {5}));
  };
}
