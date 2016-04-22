#pragma once

#include "fwd.hpp"
#include "view/view.hpp"

namespace match3 {

class animations {
 public:
  explicit animations(view& v) : view_(v) {}

  void queue_animation(const std::function<void()>& anim, int length) { anims_.emplace_back(std::make_pair(anim, length)); }

  void update() {
    for (auto& a : anims_) {
      if (a.second > 0) {
        a.first();
        --a.second;
        view_.update();
        break;
      }
    }
    anims_.erase(ranges::remove_if(anims_, [](const auto& p) { return !p.second; }), anims_.end());
  }

 private:
  std::vector<std::pair<std::function<void()>, int>> anims_;
  view& view_;
};

} // match3
