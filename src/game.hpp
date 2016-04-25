//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "pph.hpp"
#include "controller/controller.hpp"

namespace msm = boost::msm::lite;

namespace match3 {

class game {
 public:
  explicit game(msm::sm<controller>& c) : controller_(c) {}

  void play() {
    EM(emscripten_set_main_loop_arg(
        play_impl, reinterpret_cast<void*>(&controller_), 0, 0))
    (play_impl(reinterpret_cast<void*>(&controller_)));
  }

  static void play_impl(void* c) {
    auto& controller_ = *reinterpret_cast<msm::sm<controller>*>(c);
    do {
      auto dispatch_event =
          msm::make_dispatch_table<SDL_Event, SDL_QUIT, SDL_FINGERMOTION>(
              controller_);
      controller_.process_event(time_tick{});
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        dispatch_event(event, event.type);
      }
    } while (EM(false && )() !controller_.is(msm::X));
  }

 private:
  msm::sm<controller>& controller_;
};

}  // match3
