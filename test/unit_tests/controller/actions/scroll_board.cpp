// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/actions/scroll_board.hpp"
#include <range/v3/algorithm/equal.hpp>
#include <vector>
#include "common/utils.hpp"
#include "model/board.hpp"

int main() {
  using namespace match3;

  "scroll board"_test = [] {
    constexpr auto width = 3;
    constexpr auto height = 3;

    board b{{4, 3, 3, 3, 2, 2, 0, 0, 0},
            config{.board_width = width, .board_height = height}};
    struct {
      std::vector<board::color_t> matches;
    } event{{6, 7, 8}};

    scroll_board(b, event);

    expect(equal<width * height>({0, 0, 0, 4, 3, 3, 3, 2, 2}, b));
  };
}
