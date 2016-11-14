//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "controller/guards/is_key.hpp"

int main() {

  using namespace match3;
  "is key"_test = [] {
    constexpr auto key = 42;
    struct {
      int key;
    } event{key};
    expect(is_key(key)(event));
  };

  "is not key"_test = [] {
    constexpr auto key = 42;
    struct {
      int key;
    } event{0};
    expect(!is_key(key)(event));
  };
}

