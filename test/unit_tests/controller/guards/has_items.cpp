//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/guards/has_items.hpp"

int main() {
  using namespace match3;
  "has items"_test = [] {
    selected s = {1, 2};
    expect(has_items(s));
  };

  "has items empty"_test = [] {
    selected s = {};
    expect(!has_items(s));
  };
}
