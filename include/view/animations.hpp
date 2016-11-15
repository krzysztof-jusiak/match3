//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "view/ianimations.hpp"
#include "view/view.hpp"

namespace match3 {

class animations : public ianimations {
  struct animation {
    std::function<void()> animate;
    std::chrono::milliseconds length;
    std::chrono::milliseconds start = {};
  };

 public:
  explicit animations(view& v) : view_(v) {}

  void queue_animation(const std::function<void()>& animate,
                       std::chrono::milliseconds length) override {
    animations_.emplace_back(animation{animate, length});
  }

  virtual void update() override {
    using namespace std::chrono_literals;
    constexpr auto in_parallel = 1;
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    ranges::for_each(animations_ | ranges::view::take(in_parallel),
                     [&](auto& a) {
                       a.start = a.start == 0ms ? now : a.start;
                       a.animate();
                       view_.update();
                     });

    animations_.erase(ranges::remove_if(animations_,
                                        [&](const auto& a) {
                                          return a.start > 0ms &&
                                                 now >= a.start + a.length;
                                        }),
                      animations_.end());
  }

  virtual bool done() const override { return animations_.empty(); }

 private:
  std::vector<animation> animations_;
  view& view_;
};

}  // match3
