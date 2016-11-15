//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <range/v3/algorithm/equal.hpp>
#include "common/logger.hpp"
#include "common/mocks_provider.hpp"
#include "common/utils.hpp"
#include "config.hpp"
#include "controller/player.hpp"
#include "model/board.hpp"

namespace di = boost::di;
namespace sml = boost::sml;

int main() {
  "match3, matchl, out of moves, game over, reset"_test = [] {
    // given
    constexpr auto moves = 2;

    // clang-format off
    auto injector = di::make_injector<mocks_provider>(
      di::bind<>.to(match3::config{"", 0, 0, 7, 10, 5, moves, 3})
    , di::bind<match3::board::color_t[]>.to({
          /*0 1 2 3 4 5 6*/
      /*0*/ 3,5,1,4,3,2,2,
      /*1*/ 1,1,4,2,5,1,3,
      /*2*/ 5,3,5,4,5,3,2,
      /*3*/ 4,4,2,1,3,4,5,
      /*4*/ 5,1,1,2,4,5,1,
      /*5*/ 5,2,3,5,4,2,1,
      /*6*/ 1,5,5,1,5,5,4,
      /*7*/ 2,3,3,1,3,3,4,
      /*8*/ 3,2,2,5,4,4,1,
      /*9*/ 1,2,3,4,1,3,4
      })
    , di::bind<match3::randomize>.to([](int, int) { static auto i = 42; return i++; })
    );
    // clang-format on

    using namespace fakeit;
    auto&& canvas = mocks_provider::get_mock<match3::icanvas>();
    When(Method(canvas, load_image)).AlwaysReturn(std::shared_ptr<void>{});
    When(Method(canvas, create_text)).AlwaysReturn(std::shared_ptr<void>{});
    When(Method(canvas, draw))
        .AlwaysDo([](std::shared_ptr<void>, int, int, bool) {});
    When(Method(canvas, render)).AlwaysDo([] {});
    When(Method(canvas, clear)).AlwaysDo([] {});

    expect(ranges::equal(injector.create<match3::board>().grids,
                         injector.create<match3::board&>().grids));

    // when
    auto sm = injector.create<sml::sm<match3::player>>();
    expect(2 == injector.create<match3::moves&>());

    swipe(sm, {2, 1}, {2, 0});
    expect(1 == injector.create<match3::moves&>());

    // then
    expect(ranges::equal({/*0 1 2 3 4 5 6*/
                          /*0*/ 42, 43, 44, 4, 3, 2, 2,
                          /*1*/ 3,  5,  4,  2, 5, 1, 3,
                          /*2*/ 5,  3,  5,  4, 5, 3, 2,
                          /*3*/ 4,  4,  2,  1, 3, 4, 5,
                          /*4*/ 5,  1,  1,  2, 4, 5, 1,
                          /*5*/ 5,  2,  3,  5, 4, 2, 1,
                          /*6*/ 1,  5,  5,  1, 5, 5, 4,
                          /*7*/ 2,  3,  3,  1, 3, 3, 4,
                          /*8*/ 3,  2,  2,  5, 4, 4, 1,
                          /*9*/ 1,  2,  3,  4, 1, 3, 4},
                         injector.create<match3::board&>().grids));

    // when
    swipe(sm, {6, 9}, {6, 8});
    expect(0 == injector.create<match3::moves&>());

    // then
    expect(ranges::equal({/*0 1 2 3 4 5 6*/
                          /*0*/ 42, 43, 44, 4, 45, 46, 50,
                          /*1*/ 3,  5,  4,  2, 3,  2,  51,
                          /*2*/ 5,  3,  5,  4, 5,  1,  52,
                          /*3*/ 4,  4,  2,  1, 5,  3,  47,
                          /*4*/ 5,  1,  1,  2, 3,  4,  48,
                          /*5*/ 5,  2,  3,  5, 4,  5,  49,
                          /*6*/ 1,  5,  5,  1, 4,  2,  2,
                          /*7*/ 2,  3,  3,  1, 5,  5,  3,
                          /*8*/ 3,  2,  2,  5, 3,  3,  2,
                          /*9*/ 1,  2,  3,  4, 1,  3,  5},
                         injector.create<match3::board&>().grids));

    // when
    sm.process_event(make_click_event<match3::up>(3, 3));  // reset
    expect(moves == injector.create<match3::moves&>());
  };
}
