//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <range/v3/algorithm/equal.hpp>
#include "common/logger.hpp"
#include "fakes/fake_canvas.hpp"
#include "pph.hpp"
#include "model/config.hpp"
#include "model/board.hpp"
#include "controller/controller.hpp"

namespace di = boost::di;
namespace msm = boost::msm::lite;

template <class T>
auto make_click_event(int x, int y) {
  union {
    struct {
      int x;
      int y;
    } fake;
    T event;
  } click{{x * 43 + 25, y * 43 + 65}};
  return click.event;
};

template <class SM>
void swipe(SM& sm, std::pair<int, int> from, std::pair<int, int> to) {
  sm.process_event(
      make_click_event<match3::button_down>(from.first, from.second));
  sm.process_event(make_click_event<match3::button_up>(to.first, to.second));
};

test match5_out_of_moves = [] {
  // given
  constexpr auto moves = 1;

  // clang-format off
  auto injector = di::make_injector(
    di::bind<match3::icanvas>.to<fake_canvas>()
  , di::bind<>.to(match3::config{"", 0, 0, 7, 10, 5, moves})
  , di::bind<match3::board::color[]>.to({
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

  expect(ranges::equal(injector.create<match3::board>().grids,
                       injector.create<match3::board&>().grids));
  auto sm = injector.create<msm::sm<match3::controller>>();
  expect(1 == injector.create<match3::moves&>());

  // when
  swipe(sm, {3, 5}, {3, 6});
  expect(0 == injector.create<match3::moves&>());

  // then
  expect(ranges::equal({/*0 1 2 3 4 5 6*/
                        /*0*/ 3, 42, 43, 44, 45, 46, 2,
                        /*1*/ 1, 5, 1, 4, 3, 2, 3,
                        /*2*/ 5, 1, 4, 2, 5, 1, 2,
                        /*3*/ 4, 3, 5, 4, 5, 3, 5,
                        /*4*/ 5, 4, 2, 1, 3, 4, 1,
                        /*5*/ 5, 1, 1, 2, 4, 5, 1,
                        /*6*/ 1, 2, 3, 1, 4, 2, 4,
                        /*7*/ 2, 3, 3, 1, 3, 3, 4,
                        /*8*/ 3, 2, 2, 5, 4, 4, 1,
                        /*9*/ 1, 2, 3, 4, 1, 3, 4},
                       injector.create<match3::board&>().grids));
};

test match3_matchl_out_of_moves_game_over_reset = [] {
  // given
  constexpr auto moves = 2;

  // clang-format off
  auto injector = di::make_injector(
    di::bind<match3::icanvas>.to<fake_canvas>()
  , di::bind<>.to(match3::config{"", 0, 0, 7, 10, 5, moves})
  , di::bind<match3::board::color[]>.to({
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

  auto sm = injector.create<msm::sm<match3::controller>>();
  expect(ranges::equal(injector.create<match3::board>().grids,
                       injector.create<match3::board&>().grids));
  expect(2 == injector.create<match3::moves&>());

  // when
  swipe(sm, {2, 1}, {2, 0});
  expect(1 == injector.create<match3::moves&>());

  // then
  expect(ranges::equal({/*0 1 2 3 4 5 6*/
                        /*0*/ 42, 43, 44, 4, 3, 2, 2,
                        /*1*/ 3, 5, 4, 2, 5, 1, 3,
                        /*2*/ 5, 3, 5, 4, 5, 3, 2,
                        /*3*/ 4, 4, 2, 1, 3, 4, 5,
                        /*4*/ 5, 1, 1, 2, 4, 5, 1,
                        /*5*/ 5, 2, 3, 5, 4, 2, 1,
                        /*6*/ 1, 5, 5, 1, 5, 5, 4,
                        /*7*/ 2, 3, 3, 1, 3, 3, 4,
                        /*8*/ 3, 2, 2, 5, 4, 4, 1,
                        /*9*/ 1, 2, 3, 4, 1, 3, 4},
                       injector.create<match3::board&>().grids));

  // when
  swipe(sm, {6, 9}, {6, 8});
  expect(0 == injector.create<match3::moves&>());

  // then
  expect(ranges::equal({/*0 1 2 3 4 5 6*/
                        /*0*/ 42, 43, 44, 4, 45, 46, 50,
                        /*1*/ 3, 5, 4, 2, 3, 2, 51,
                        /*2*/ 5, 3, 5, 4, 5, 1, 52,
                        /*3*/ 4, 4, 2, 1, 5, 3, 47,
                        /*4*/ 5, 1, 1, 2, 3, 4, 48,
                        /*5*/ 5, 2, 3, 5, 4, 5, 49,
                        /*6*/ 1, 5, 5, 1, 4, 2, 2,
                        /*7*/ 2, 3, 3, 1, 5, 5, 3,
                        /*8*/ 3, 2, 2, 5, 3, 3, 2,
                        /*9*/ 1, 2, 3, 4, 1, 3, 5},
                       injector.create<match3::board&>().grids));

  // when
  sm.process_event(make_click_event<match3::button_up>(3, 3));  // reset
  expect(moves == injector.create<match3::moves&>());
};
