// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <range/v3/algorithm/equal.hpp>
#include "controller/actions/generate_new.hpp"
#include "model/board.hpp"
#include "config.hpp"

int main() {
  using namespace match3;

  "generate new"_test = [] {
    static constexpr auto NEW = 42;
    board b{{4, 0, 3, 3, 0, 2, 1, 0, 3}, config{.board_width = 3}};
    selected s = {0, 5};
    struct {
      std::vector<board::color_t> matches;
    } event{{1, 4, 7}};

    generate_new(b, event, s, config{.board_width = 3},
                 [](auto...) { return NEW; });

    expect(ranges::equal({4, NEW, 3, 3, NEW, 2, 1, NEW, 3}, b.grids));
    expect(ranges::equal({0, 1, 4, 5, 7}, s));
  };
}
