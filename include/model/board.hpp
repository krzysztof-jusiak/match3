//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <algorithm>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/transform.hpp>
#include <range/v3/action/unique.hpp>
#include <range/v3/algorithm/count.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/iota.hpp>  // view::ints
#include <range/v3/view/stride.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include "config.hpp"

namespace match3 {

namespace board_logic {

/**
 * Row view
 *
 * @param view view
 * @param n row number
 * @param width row width
 *
 * \code{.cpp}
 *
 * | 1 2 3 |           |  1   2   3  |
 * | 3 4 5 | => n:1 -> | [3] [4] [5] |
 * | 6 7 8 |           |  6   7   8  |
 *
 * assert(row([1, 2, 3, 4, 5, 6, 7, 8], 1, 3), [3, 4, 5]);
 *
 * \endcode
 *
 * @return row view
 */
const auto row = [](auto&& view, auto n, auto width) {
  return view | ranges::view::drop(width * n) |
         ranges::view::take(width);  // or slice
};

/**
 * Column view
 *
 * @param view view
 * @param n column number
 * @param width row width
 *
 * \code{.cpp}
 *
 * | 1 2 3 |           | 1 [2] 3 |
 * | 3 4 5 | => n:1 -> | 3 [4] 5 |
 * | 6 7 8 |           | 6 [7] 8 |
 *
 * assert(col([1, 2, 3, 4, 5, 6, 7, 8], 1, 3), [2, 4, 7]);
 *
 * \endcode
 *
 * @return column view
 */
const auto col = [](auto&& view, auto n, auto width) {
  return view | ranges::view::drop(n) | ranges::view::stride(width);
};

/**
 * Look for at least n consecutive same elements
 *
 * @param view view
 * @param color element to be looked for
 * @param width row width
 *
 * \code{.cpp}
 *
 * | 1 3 3 3 2 1 | => color:3, n:3 -> {begin: 1, length: 3}
 * | 1 2 3 3 3 3 | => color:3, n:3 -> {begin: 2, length: 4}
 *
 * assert(match_n([1, 3, 3, 3, 2], 3), {1, 3});
 *
 * \endcode
 *
 * @return column view
 */
const auto match_n = [](auto&& view, auto color,
                        const int max_match_length = 3) {
  const auto&& matches =
      ranges::view::ints |
      ranges::view::take(ranges::size(view) - max_match_length + 1) |
      ranges::view::transform([=](auto i) {
        return ranges::count(view | ranges::view::drop(i) |
                                 ranges::view::take(max_match_length),
                             color) == max_match_length;
      });

  constexpr auto is_match = true;
  const auto it = ranges::find(matches, is_match);
  const auto found = it != ranges::end(matches);
  const auto mlength =
      found ? ranges::count(matches, is_match) + (max_match_length - 1) : 0;
  const auto mbegin = found ? ranges::distance(ranges::begin(matches), it) : 0;

  struct {
    decltype(mbegin) begin;
    decltype(mlength) length;
  } result{mbegin, mlength};
  return result;
};

/**
 * Checks whether there is a match for given position
 *
 * @param view view
 * @param n position n
 * @param width row width
 *
 * \code{.cpp}
 *
 * | 1 2 3 4 5 |            | 1 2 3  4  5 |    | 1 2 3  4  5 |
 * | 6 7 7 9 3 |            | 6 7 7  9  3 |    | 6 7 7  9  3 |
 * | 2 3 4 5 3 | => n:13 -> | 2 3 4 [5] 3 | => | 2 3 4 [5] 3 | => true
 * | 2 1 3 5 1 |            | 2 1 3  5  1 |    | 2 1 3 [5] 1 |
 * | 4 2 1 5 8 |            | 4 2 1  5  8 |    | 4 2 1 [5] 8 |
 *
 * | 1 2 3 4 5 |            | 1 2 [3] 4 5 |    | 1 2 3 4 5 |
 * | 6 7 7 9 3 |            | 6 7  7  9 3 |    | 6 7 7 9 3 |
 * | 2 3 4 5 3 | => n:2 ->  | 2 3  4  5 3 | => | 2 3 4 5 3 | => false
 * | 2 1 3 5 1 |            | 2 1  3  5 1 |    | 2 1 3 5 1 |
 * | 4 2 1 5 8 |            | 4 2  1  5 8 |    | 4 2 1 5 8 |
 *
 * assert(is_match([1, 2, 3, 4, 2, 6, 7, 2, 9], 1, 3));
 * assert(!is_match([1, 2, 3, 4, 3, 6, 7, 2, 9], 1, 3));
 *
 * \endcode
 *
 * @return true, if there is a match, false otherwise
 */
const auto is_match = [](auto&& view, auto n, auto width) {
  const auto color = view[n];
  const auto x = n % width;
  const auto y = n / width;
  return match_n(row(view, y, width), color).length ||
         match_n(col(view, x, width), color).length;
};

// clang-format off
/**
 * Creates a sorted and unique list of positions which matches
 *
 * @param view view`
 * @param n position n
 * @param width row width
 *
 * \code{.cpp}
 *
 * | 1 2 3 4 5 |             | 1  2 3  4  5 |    | 1  2   3   4  5 |
 * | 6 7 7 9 3 |             | 6  7 7  9  3 |    | 6  7   7   9  3 |
 * | 2 5 5 5 3 |  => n:13 -> | 2  5 5 [5] 3 | => | 2 [5] [5] [5] 3 | => [11, 12, 13, 18, 23]
 * | 2 1 3 5 1 |             | 2  1 3  5  3 |    | 2  1   3  [5] 3 |
 * | 4 2 1 5 8 |             | 4  2 1  5  8 |    | 4  2   1  [5] 8 |
 *
 * assert(match([1, 2, 3, 4, 4, 3, 7, 2, 3], 2, 3) == [2, 5, 8]);
 * assert(!match([1, 2, 3, 4, 4, 3, 7, 2, 1], 2, 3) == []);
 *
 * \endcode
 *
 * @return sorted, unique list of positions which matches
 */
// clang-format on
const auto match = [](auto&& view, auto n, auto width) {
  const auto color = view[n];
  const auto x = n % width;
  const auto y = n / width;
  const auto match_r = match_n(row(view, y, width), color);
  const auto match_c = match_n(col(view, x, width), color);
  const auto transform = [](auto length, auto expr) {
    return ranges::view::ints | ranges::view::take(length) |
           ranges::view::transform(expr);
  };
  std::vector<decltype(n)> result = ranges::view::concat(
      transform(match_r.length,
                [=](auto i) { return y * width + match_r.begin + i; }),
      transform(match_c.length,
                [=](auto i) { return (match_c.begin + i) * width + x; }));
  result |= ranges::action::sort | ranges::action::unique;
  return result;
};

/**
 * Scrolls given view
 *
 * \code{.cpp}
 *
 * | 3 |            |  3  |    | 0 |
 * | 0 |            | [0] |    | 0 |
 * | 0 |  => n:1 -> |  0  | => | 0 |
 * | 0 |            |  0  |    | 3 |
 * | 4 |            |  4  |    | 4 |
 *
 * assert(scroll([3, 0, 0, 0, 4], 1, 3) == [0, 0, 0, 3, 4]);
 *
 * \endcode
 *
 * @param view view
 * @param n position
 * @param width row width
 */
const auto scroll = [](auto&& view, auto n, auto width) {
  const auto&& c =
      col(view, n % width, width) | ranges::view::take(n / width + 1);
  auto begin = ranges::begin(c);
  ranges::advance(begin, n / width);
  ranges::rotate(c, begin);
};

}  // board_logic

class board {
 public:
  using color_t = int;

  board(const std::vector<color_t>& grids, const config c)
      : grids(grids), width(c.board_width) {}

  void swipe(const int p1, const int p2) { std::swap(grids[p1], grids[p2]); }

  bool is_match(const int position) const {
    return board_logic::is_match(grids, position, width);
  }

  auto match(const int position) const {
    return board_logic::match(grids, position, width);
  }

  void scroll(const int position) {
    board_logic::scroll(grids, position, width);
  }

  void update(const int position, const color_t value) {
    grids[position] = value;
  }

  auto operator[](const int position) const { return grids[position]; }

 private:
  std::vector<color_t> grids;
  int width = 0;
};

}  // match3
