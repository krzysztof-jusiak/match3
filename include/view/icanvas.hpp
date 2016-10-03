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

class icanvas {
 public:
  virtual ~icanvas() noexcept = default;
  virtual std::shared_ptr<void> load_image(const std::string&) const = 0;
  virtual std::shared_ptr<void> create_text(const std::string&,
                                            const std::string&, int) const = 0;
  virtual void draw(std::shared_ptr<void>, int x = 0, int y = 0,
                    bool = true) = 0;
  virtual void render() = 0;
  virtual void clear() = 0;
};

}  // match3
