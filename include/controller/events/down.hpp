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

struct down : sml::utility::id<SDL_FINGERDOWN, SDL_MOUSEBUTTONDOWN> {
  explicit down(const SDL_Event& event)
      : down(EM_ASM_INT_V({
               return / iPhone | iPad | iPod |
                      Android / i.test(navigator.userAgent);
             }),
             event) {}

  explicit down(bool is_mobile, const SDL_Event& event)
      : x(is_mobile ? int(event.tfinger.x) : int(event.button.x)),
        y(is_mobile ? int(event.tfinger.y) : int(event.button.y)) {}

  const int x, y = 0;
};

}  // match3
