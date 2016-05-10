//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "pph.hpp"
#include "view/view.hpp"

namespace match3 {

class animations {
  struct animation {
    std::function<void()> animate;
    std::chrono::milliseconds length;
    std::chrono::milliseconds start = {};
  };

 public:
  explicit animations(view& v) : view_(v) {}

  void queue_animation(const std::function<void()>& animate,
                       std::chrono::milliseconds length = {}) {
    animations_.emplace_back(animation{animate, length});
  }

  void update() {
    using namespace std::chrono_literals;
    constexpr auto async_limit = 1;
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    ranges::for_each(animations_ | ranges::view::take(async_limit),
                     [&](auto& a) {
                       a.start = a.start == 0ms ? now : a.start;
                       a.animate();
                       view_.update();
                     });

    animations_.erase(ranges::remove_if(animations_, [&](const auto& a) {
                        return a.start > 0ms && now >= a.start + a.length;
                      }), animations_.end());
  }

 private:
  std::vector<animation> animations_;
  view& view_;
};

}  // match3
