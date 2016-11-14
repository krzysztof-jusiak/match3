// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/actions/find_matches.hpp"
#include <range/v3/algorithm/equal.hpp>
#include <vector>
#include "config.hpp"
#include "model/board.hpp"

int main() {
  using namespace match3;
  //"find matches empty"_test = [] {
  // board b{{1, 2, 3, 4}, config{.board_width = 2}};
  // selected s = {0, 1};
  // struct {
  // int arity;
  // std::vector<board::color_t> matches = {};
  //} event{int(s.size())};

  // find_matches(b, event, s);

  // expect(s.empty());
  // expect(event.matches.empty());
  //};

  //"find matches"_test = [] {
  // board b{{1, 2, 3, 1, 3, 2, 1, 2, 3}, config{.board_width = 3}};
  // selected s = {0};
  // struct {
  // int arity;
  // std::vector<board::color_t> matches = {};
  //} event{int(s.size())};

  // find_matches(b, event, s);

  // expect(s.empty());
  // expect(ranges::equal({0, 3, 6}, event.matches));
  //};

  //"find matches more"_test = [] {
  // board b{{1, 2, 3, 1, 1, 1, 1, 2, 3}, config{.board_width = 3}};
  // selected s = {1, 3};
  // struct {
  // int arity;
  // std::vector<board::color_t> matches = {};
  //} event{int(s.size())};

  // find_matches(b, event, s);

  // expect(s.empty());
  // expect(ranges::equal({0, 3, 4, 5, 6}, event.matches));
  //};
}
