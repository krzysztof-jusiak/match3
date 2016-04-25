//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "pph.hpp"

namespace match3 {

/**
 * Row view
 *
 * @param v view
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
auto row = [](auto&& v, auto n, auto width) {
  return v | ranges::view::drop(width * n) | ranges::view::take(width);
};

/**
 * Column view
 *
 * @param v view
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
auto col = [](auto&& v, auto n, auto width) {
  return v | ranges::view::drop(n) | ranges::view::stride(int(width));
};

/**
 * Look for at least n consecutive same elements
 *
 * @param v view
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
auto match_n = [](auto&& v, auto color, int n = 3) {
  const auto&& matches =
      ranges::view::ints | ranges::view::take(ranges::size(v) - n + 1) |
      ranges::view::transform([=](/*auto*/ int i) {
        return ranges::count(v | ranges::view::drop(i) | ranges::view::take(n),
                             color) == n;
      });

  constexpr auto is_match = 1;
  const auto it = ranges::find(matches, is_match);
  const auto found = it != ranges::end(matches);
  const auto mlength = found ? ranges::count(matches, is_match) + (n - 1) : 0;
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
 * @param v view
 * @param value position value
 * @param width row width
 *
 * \code{.cpp}
 *
 * | 1 2 3 4 5 |                | 1 2 3  4  5 |    | 1 2 3  4  5 |
 * | 6 7 7 9 3 |                | 6 7 7  9  3 |    | 6 7 7  9  3 |
 * | 2 3 4 5 3 | => value:13 -> | 2 3 4 [5] 3 | => | 2 3 4 [5] 3 | => true
 * | 2 1 3 5 1 |                | 2 1 3  5  1 |    | 2 1 3 [5] 1 |
 * | 4 2 1 5 8 |                | 4 2 1  5  8 |    | 4 2 1 [5] 8 |
 *
 * | 1 2 3 4 5 |                | 1 2 [3] 4 5 |    | 1 2 3 4 5 |
 * | 6 7 7 9 3 |                | 6 7  7  9 3 |    | 6 7 7 9 3 |
 * | 2 3 4 5 3 | => value:2 ->  | 2 3  4  5 3 | => | 2 3 4 5 3 | => false
 * | 2 1 3 5 1 |                | 2 1  3  5 1 |    | 2 1 3 5 1 |
 * | 4 2 1 5 8 |                | 4 2  1  5 8 |    | 4 2 1 5 8 |
 *
 * assert(is_match([1, 2, 3, 4, 2, 6, 7, 2, 9], 1, 3));
 * assert(!is_match([1, 2, 3, 4, 3, 6, 7, 2, 9], 1, 3));
 *
 * \endcode
 *
 * @return true, if there is a match, false otherwise
 */
auto is_match = [](auto&& v, auto value, auto width) {
  const auto color = v[value];
  const auto x = value % width;
  const auto y = value / width;
  return match_n(row(v, y, width), color).length ||
         match_n(col(v, x, width), color).length;
};

// clang-format off
/**
 * Creates a sorted and unique list of positions which matches
 *
 * @param v view`
 * @param value position value
 * @param width row width
 *
 * \code{.cpp}
 *
 * | 1 2 3 4 5 |                 | 1  2 3  4  5 |    | 1  2   3   4  5 |
 * | 6 7 7 9 3 |                 | 6  7 7  9  3 |    | 6  7   7   9  3 |
 * | 2 5 5 5 3 |  => value:13 -> | 2  5 5 [5] 3 | => | 2 [5] [5] [5] 3 | => [11, 12, 13, 18, 23]
 * | 2 1 3 5 1 |                 | 2  1 3  5  3 |    | 2  1   3  [5] 3 |
 * | 4 2 1 5 8 |                 | 4  2 1  5  8 |    | 4  2   1  [5] 8 |
 *
 * assert(match([1, 2, 3, 4, 4, 3, 7, 2, 3], 2, 3) == [2, 5, 8]);
 * assert(!match([1, 2, 3, 4, 4, 3, 7, 2, 1], 2, 3) == []);
 *
 * \endcode
 *
 * @return sorted, unique list of positions which matches
 */
// clang-format on
auto match = [](auto&& v, auto value, auto width) {
  const auto color = v[value];
  const auto x = value % width;
  const auto y = value / width;
  const auto match_r = match_n(row(v, y, width), color);
  const auto match_c = match_n(col(v, x, width), color);
  const auto transform = [](/*auto*/ int length, auto expr) {
    return ranges::view::ints | ranges::view::take(length) |
           ranges::view::transform(expr);
  };
  std::vector<decltype(value)> result = ranges::view::concat(
      transform(match_r.length,
                [=](/*auto*/ int i) { return y * width + match_r.begin + i; }),
      transform(match_c.length, [=](/*auto*/ int i) {
        return (match_c.begin + i) * width + x;
      }));
  result |= ranges::action::sort | ranges::action::unique;
  return result;
};

/**
 * Scrolls given view
 *
 * \code{.cpp}
 *
 * | 3 |                |  3  |    | 0 |
 * | 0 |                | [0] |    | 0 |
 * | 0 |  => value:1 -> |  0  | => | 0 |
 * | 0 |                |  0  |    | 3 |
 * | 4 |                |  4  |    | 4 |
 *
 * assert(scroll([3, 0, 0, 0, 4], 1, 3) == [0, 0, 0, 3, 4]);
 *
 * \endcode
 *
 * @param v view
 * @param value matched position
 * @param width row width
 */
auto scroll = [](auto&& v, const auto& value, auto width) {
  const auto&& c =
      col(v, value % width, width) | ranges::view::take(value / width + 1);
  auto begin = ranges::begin(c);
  ranges::advance(begin, value / width);
  ranges::rotate(c, begin);
};

// clang-format off
/**
 * Creates a list of elements which could generate new matches
 *
 * @param matches list of matches
 * @param width row width
 *
 * \code{.cpp}
 *
 *                         | 1  2   3   4  5 |    | 1  [2] [3] [4] 5 |
 *                         | 6  7   7   9  3 |    | 6  [7] [7] [9] 3 |
 * [11, 12, 13, 18, 23] => | 2 [5] [5] [5] 3 | => | 2  [5] [5] [5] 3 | => [1, 2, 3, 6, 7, 8, 11, 12, 13, 18, 23]
 *                         | 2  1   3  [5] 3 |    | 2   1   3  [5] 3 |
 *                         | 4  2   1  [5] 8 |    | 4   2   1  [5] 8 |
 *
 * assert(affected([0, 1, 2], 3) == [0, 1, 2]);
 *
 * \endcode
 *
 * @return sorted, unique list of positions
 */
// clang-format on
auto affected = [](const auto& matches, auto width) {
  const auto&& columns = matches | ranges::view::transform([=](/*auto*/ int m) {
                           return ranges::view::ints |
                                  ranges::view::take(m / width + 1) |
                                  ranges::view::transform([=](/*auto*/ int i) {
                                    return m % width + (i * width);
                                  });
                         });

  std::decay_t<decltype(matches)> result = columns | ranges::view::join;
  result |= ranges::action::sort | ranges::action::unique;
  return result;
};

}  // match3
