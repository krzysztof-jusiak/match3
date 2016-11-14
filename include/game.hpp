//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "controller/player.hpp"
#include "pph.hpp"

namespace sml = boost::sml;

namespace match3 {

class game {
 public:
  explicit game(sml::sm<player>& c) : player_(c) {}

  void play() {
    EM(emscripten_set_main_loop_arg(
        play_impl, reinterpret_cast<void*>(&player_), 0, 0))
    (play_impl(reinterpret_cast<void*>(&player_)));
  }

  static void play_impl(void* c) {
    auto& player_ = *reinterpret_cast<sml::sm<player>*>(c);
    do {
      auto dispatch_event =
          sml::utility::make_dispatch_table<SDL_Event, SDL_QUIT, SDL_FINGERMOTION>(
              player_);
      player_.process_event(time_tick{});
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        dispatch_event(event, event.type);
      }
    } while (EM(false &&)() !player_.is(sml::X));
  }

 private:
  sml::sm<player>& player_;
};

}  // match3
