//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "fwd.hpp"

namespace match3 {

auto row = [](auto&& v, auto n, auto width) { return v | ranges::view::drop(width * n) | ranges::view::take(width); };
auto col = [](auto&& v, auto n, auto width) { return v | ranges::view::drop(n) | ranges::view::stride(int(width)); };

auto match_n = [](auto&& v, auto color, int n = 3) {
  const auto&& matches = ranges::view::ints | ranges::view::take(ranges::size(v) - n + 1) | ranges::view::transform([=](int i) {
                           return ranges::count(v | ranges::view::drop(i) | ranges::view::take(n), color) == n;
                         });

  constexpr auto is_match = 1;
  const auto it = ranges::find(matches, is_match);
  const auto found = it != ranges::end(matches);
  const auto length = found ? ranges::count(matches, is_match) + (n - 1) : 0;
  const auto begin = found ? ranges::distance(ranges::begin(matches), it) : 0;

  struct {
    decltype(begin) begin;
    decltype(length) length;
  } result{begin, length};
  return result;
};

auto is_match = [](auto&& v, auto value, auto width) {
  const auto color = v[value];
  const auto x = value % width;
  const auto y = value / width;
  return match_n(row(v, y, width), color).length || match_n(col(v, x, width), color).length;
};

auto match = [](auto&& v, auto value, auto width) {
  const auto color = v[value];
  const auto x = value % width;
  const auto y = value / width;
  const auto match_r = match_n(row(v, y, width), color);
  const auto match_c = match_n(col(v, x, width), color);
  const auto transform = [](int length, auto expr) {
    return ranges::view::ints | ranges::view::take(length) | ranges::view::transform(expr);
  };
  std::vector<decltype(value)> result =
      ranges::view::concat(transform(match_r.length, [=](int i) { return y * width + match_r.begin + i; }),
                           transform(match_c.length, [=](int i) { return (match_c.begin + i) * width + x; }));
  result |= ranges::action::sort | ranges::action::unique;
  return result;
};

auto scroll = [](auto&& v, const auto& match, auto width) {
  const auto&& c = col(v, match % width, width) | ranges::view::take(match / width + 1);
  auto begin = ranges::begin(c);
  ranges::advance(begin, match / width);
  ranges::rotate(c, begin);
};

auto affected = [](const auto& matches, auto width) {
  const auto&& columns = matches | ranges::view::transform([=](int m) {
                           return ranges::view::ints | ranges::view::take(m / width + 1) |
                                  ranges::view::transform([=](int i) { return m % width + (i * width); });
                         });

  std::vector<int> result = columns | ranges::view::join;
  result |= ranges::action::sort | ranges::action::unique;
  return result;
};

} // match3
