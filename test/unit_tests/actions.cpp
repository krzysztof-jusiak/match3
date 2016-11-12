// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <range/v3/algorithm/equal.hpp>
#include "controller/controller.hpp"
#include "model/board.hpp"

int main() {
  using namespace match3;

  "drop item"_test = [] {
    selected s = {1};
    drop_item(s);
    expect(s.empty());
  };

  "clear selected"_test = [] {
    selected s = {1, 2, 3};
    clear_selected(s);
    expect(s.empty());
  };

  "swap items"_test = [] {
    board b{{1, 2}, config{}};
    selected s = {0, 1};
    swap_items(s, b);
    expect(ranges::equal({2, 1}, b.grids));
  };

  "find matches empty"_test = [] {
    board b{{1, 2, 3, 4}, config{.board_width = 2}};
    selected s = {0, 1};
    struct {
      int arity;
      std::vector<board::color_t> matches = {};
    } event{int(s.size())};

    find_matches(b, event, s);

    expect(s.empty());
    expect(event.matches.empty());
  };

  "find matches"_test = [] {
    board b{{1, 2, 3, 1, 3, 2, 1, 2, 3}, config{.board_width = 3}};
    selected s = {0};
    struct {
      int arity;
      std::vector<board::color_t> matches = {};
    } event{int(s.size())};

    find_matches(b, event, s);

    expect(s.empty());
    expect(ranges::equal({0, 3, 6}, event.matches));
  };

  "find matches more"_test = [] {
    board b{{1, 2, 3, 1, 1, 1, 1, 2, 3}, config{.board_width = 3}};
    selected s = {1, 3};
    struct {
      int arity;
      std::vector<board::color_t> matches = {};
    } event{int(s.size())};

    find_matches(b, event, s);

    expect(s.empty());
    expect(ranges::equal({0, 3, 4, 5, 6}, event.matches));
  };

  "destroy matches"_test = [] {
    board b{{4, 2, 3, 3, 2, 2, 1, 2, 3}, config{}};
    struct {
      std::vector<board::color_t> matches;
    } event{{1, 4, 7}};

    destroy_matches(b, event);

    expect(ranges::equal({4, 0, 3, 3, 0, 2, 1, 0, 3}, b.grids));
  };

  "scroll board"_test = [] {
    board b{{4, 3, 3, 3, 2, 2, 0, 0, 0}, config{.board_width = 3}};
    struct {
      std::vector<board::color_t> matches;
    } event{{6, 7, 8}};

    scroll_board(b, event);

    expect(ranges::equal({0, 0, 0, 4, 3, 3, 3, 2, 2}, b.grids));
  };

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
