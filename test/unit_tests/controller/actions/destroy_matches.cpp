// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/actions/destroy_matches.hpp"
#include <range/v3/algorithm/equal.hpp>
#include <vector>
#include "common/utils.hpp"
#include "model/board.hpp"

int main() {
  using namespace match3;

  "destroy matches"_test = [] {
    constexpr auto width = 3;
    constexpr auto height = 3;

    board b{{4, 2, 3, 3, 2, 2, 1, 2, 3},
            config{.board_width = width, .board_height = height}};
    struct {
      std::vector<board::color_t> matches;
    } event{{1, 4, 7}};

    destroy_matches(b, event);

    expect(equal<width * height>({4, 0, 3, 3, 0, 2, 1, 0, 3}, b));
  };
}
