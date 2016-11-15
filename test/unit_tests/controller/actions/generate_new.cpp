// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/actions/generate_new.hpp"
#include <range/v3/algorithm/equal.hpp>
#include "common/utils.hpp"
#include "config.hpp"
#include "model/board.hpp"

int main() {
  using namespace match3;

  "generate new"_test = [] {
    static constexpr auto NEW = 42;
    constexpr auto width = 3;
    constexpr auto height = 3;
    board b{{4, 0, 3, 3, 0, 2, 1, 0, 3},
            config{.board_width = width, .board_height = height}};
    selected s = {0, 5};
    generate_new(b, s, config{.board_width = width, .board_height = height},
                 [](auto...) { return NEW; });

    expect(equal<width * height>({4, NEW, 3, 3, NEW, 2, 1, NEW, 3}, b));
    expect(ranges::equal({0, 1, 2, 3, 4, 5, 6, 7, 8}, s));
  };
}
