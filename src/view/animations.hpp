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
    std::function<void()> f;
    std::chrono::milliseconds length;
    std::chrono::milliseconds start = {};
  };

 public:
  explicit animations(view& v) : view_(v) {}

  void queue_animation(const std::function<void()>& f,
                       std::chrono::milliseconds length = {}) {
    anims_.emplace_back(animation{f, length});
  }

  void update() {
    using namespace std::chrono_literals;
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    for (auto& a : anims_) {
      if (a.length >= 0ms) {
        if (a.start == 0ms) {
          a.start = now;
        }
        a.f();
        view_.update();
        break;
      }
    }

    anims_.erase(ranges::remove_if(anims_, [&](const auto& a) {
                   return a.start > 0ms && now >= a.start + a.length;
                 }), anims_.end());
  }

 private:
  std::vector<animation> anims_;
  view& view_;
};

}  // match3
