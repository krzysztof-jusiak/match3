//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "config.hpp"
#include "game.hpp"
#include "model/board.hpp"
#include "view/animations.hpp"
#include "view/sdl_canvas.hpp"

namespace di = boost::di;

auto configuration = []() {
  using namespace match3;

  // clang-format off
  return di::make_injector(
    di::bind<icanvas>.to<sdl_canvas>()
  , di::bind<ianimations>.to<animations>()

  , di::bind<>.to(
      config{.win_title = "match3",
             .win_width = 320,
             .win_height = 480,
             .board_width = 7,
             .board_height = 10,
             .board_colors = 5,
             .max_moves = 10}
    )

  , di::bind<board::color_t[]>.to({
      3,5,1,4,3,2,2,
      1,1,4,2,5,1,3,
      5,3,5,4,5,3,2,
      4,4,2,1,3,4,5,
      5,1,1,2,4,5,1,
      5,2,3,5,4,2,1,
      1,5,5,1,5,5,4,
      2,3,3,1,3,3,4,
      3,2,2,5,4,4,1,
      1,2,3,4,1,3,4
    })

  , di::bind<randomize>.to([](int begin, int end) {
      static std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> dis(begin, end);
      return dis(gen);
    })
  );
  // clang-format on
};

int main() {
  const auto injector = di::make_injector(configuration());
  injector.create<match3::game>().play();
}
