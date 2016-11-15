//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <functional>
#include "view/view.hpp"

namespace match3 {

class ianimations {
 public:
  virtual ~ianimations() noexcept = default;
  virtual void queue_animation(
      const std::function<void()>&,
      std::chrono::milliseconds length = std::chrono::milliseconds(80)) = 0;
  virtual void update() = 0;
  virtual bool done() const = 0;
};

}  // match3
