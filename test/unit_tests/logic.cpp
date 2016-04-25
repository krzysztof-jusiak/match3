//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <range/v3/algorithm/equal.hpp>
#include "controller/logic.hpp"

namespace match3 {

test $row = [] {
  int v[] = {1, 2, 3, 4, 5, 6};
  expect(ranges::equal({1, 2}, row(v, 0, 2)));
  expect(ranges::equal({3, 4}, row(v, 1, 2)));
  expect(ranges::equal({5, 6}, row(v, 2, 2)));
  expect(ranges::equal({1, 2, 3}, row(v, 0, 3)));
};

test $col = [] {
  int v[] = {1, 2, 3, 4, 5, 6};
  expect(ranges::equal({1, 3, 5}, col(v, 0, 2)));
  expect(ranges::equal({2, 4, 6}, col(v, 1, 2)));
  expect(ranges::equal({1, 4}, col(v, 0, 3)));
};

test $match_n_fail = [] {
  int v[] = {1, 3, 2, 3, 2};
  auto m = match_n(v, 3);
  expect(0 == m.begin);
  expect(0 == m.length);
};

test $match_n_success_3 = [] {
  int v[] = {1, 3, 3, 3, 2};
  auto m = match_n(v, 3);
  expect(1 == m.begin);
  expect(3 == m.length);
};

test $match_n_success_4 = [] {
  int v[] = {1, 3, 3, 3, 3, 2};
  auto m = match_n(v, 3);
  expect(1 == m.begin);
  expect(4 == m.length);
};

test $match_n_success_5 = [] {
  int v[] = {1, 2, 3, 3, 3, 3, 3, 1, 2};
  auto m = match_n(v, 3);
  expect(2 == m.begin);
  expect(5 == m.length);
};

test $is_match_fail = [] {
  int v[] = {1, 2, 3
           , 4, 3, 6
           , 7, 2, 9};

  expect(!is_match(v, 1, 3));
};

test $is_match_success = [] {
  int v[] = {1, 2, 3
           , 4, 2, 6
           , 7, 2, 9};

  expect(is_match(v, 1, 3));
};

test $match_fail = [] {
  int v[] = {1, 2, 3,
             4, 3, 3,
             7, 2, 4};

  expect(ranges::equal(std::vector<int>{}, match(v, 2, 3)));
};

test $match_success = [] {
  int v[] = {1, 2, 3,
             4, 3, 3,
             7, 2, 3};

  expect(ranges::equal({2, 5, 8}, match(v, 2, 3)));
};


test $scroll = [] {
  int v[] = {1, 1, 3,
             4, 0, 2,
             7, 2, 3};

  scroll(v, 4, 3);

  expect(ranges::equal({1, 0, 3,
                        4, 1, 2,
                        7, 2, 3}, v));
};

test $scroll_many = [] {
  int v[] = {1, 1, 3,
             4, 0, 2,
             7, 0, 3};

  scroll(v, 4, 3);
  expect(ranges::equal({1, 0, 3,
                        4, 1, 2,
                        7, 0, 3}, v));

  scroll(v, 7, 3);
  expect(ranges::equal({1, 0, 3,
                        4, 0, 2,
                        7, 1, 3}, v));
};

test $affected = [] {
  std::vector<int> v{4, 5};
  expect(ranges::equal({1, 2, 4, 5}, affected(v, 3)));
};

test $affected_all = [] {
  std::vector<int> v{0, 1, 2};
  expect(ranges::equal(v, affected(v, 3)));
};

} // match3

