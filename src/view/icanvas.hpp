//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "fwd.hpp"

struct SDL_Texture;

namespace match3 {

using Sprite = SDL_Texture;

// template<class TSprite>
class icanvas {
 public:
  virtual ~icanvas() noexcept = default;
  virtual std::shared_ptr<Sprite> load_image(const std::string&) const = 0;
  virtual std::shared_ptr<Sprite> create_text(const std::string&,
                                              const std::string&,
                                              int = 14) const = 0;
  virtual void draw(std::shared_ptr<Sprite>, int x = 0, int y = 0,
                    bool = true) = 0;
  virtual void render() = 0;
};

}  // match3
