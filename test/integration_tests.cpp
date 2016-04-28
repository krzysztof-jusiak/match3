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
#include "pph.hpp"
#include "model/config.hpp"
#include "model/board.hpp"
#include "controller/controller.hpp"

namespace di = boost::di;
namespace msm = boost::msm::lite;

test $match3 = [] {
  // given
  // clang-format off
  auto injector = di::make_injector<mocks_provider>(
    di::bind<>.to(match3::config{"", 0, 0, 7, 10, 5, 10})
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

  using namespace fakeit;
  auto&& canvas = mocks_provider::get_mock<match3::icanvas>();
  When(Method(canvas, load_image)).AlwaysReturn(std::shared_ptr<void>{});
  When(Method(canvas, create_text)).AlwaysReturn(std::shared_ptr<void>{});
  When(Method(canvas, draw))
      .AlwaysDo([](std::shared_ptr<void>, int, int, bool) {});
  When(Method(canvas, render)).AlwaysDo([] {});
  When(Method(canvas, clear)).AlwaysDo([] {});

  auto&& board = injector.create<match3::board&>();
  auto&& selected = injector.create<match3::selected&>();
  auto sm = injector.create<msm::testing::sm<match3::controller>>();

  // when
  using namespace msm;
  sm.set_current_states("wait_for_first_item"_s, "handle_matches"_s);
  std::swap(board.grids[2], board.grids[9]);
  selected = {2, 9};
  sm.process_event(match3::matches{.arity = int(selected.size())});

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
};

test $match5 = [] {
  // given
  // clang-format off
  auto injector = di::make_injector<mocks_provider>(
    di::bind<>.to(match3::config{"", 0, 0, 7, 10, 5, 10})
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

  auto&& board = injector.create<match3::board&>();
  auto&& selected = injector.create<match3::selected&>();
  auto sm = injector.create<msm::testing::sm<match3::controller>>();

  using namespace fakeit;
  auto&& canvas = mocks_provider::get_mock<match3::icanvas>();
  When(Method(canvas, load_image)).AlwaysReturn(std::shared_ptr<void>{});
  When(Method(canvas, create_text)).AlwaysReturn(std::shared_ptr<void>{});
  When(Method(canvas, draw))
      .AlwaysDo([](std::shared_ptr<void>, int, int, bool) {});
  When(Method(canvas, render)).AlwaysDo([] {});
  When(Method(canvas, clear)).AlwaysDo([] {});

  // when
  using namespace msm;
  sm.set_current_states("wait_for_first_item"_s, "handle_matches"_s);
  std::swap(board.grids[38], board.grids[45]);
  selected = {38, 45};
  sm.process_event(match3::matches{.arity = int(selected.size())});

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
