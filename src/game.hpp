#pragma once

#include "controller/controller.hpp"
#include <boost/msm-lite.hpp>

namespace msm = boost::msm::lite;

namespace match3 {

class game {
 public:
  explicit game(msm::sm<controller>& c) : controller_(c) {}

  void play() {
    EM(emscripten_set_main_loop_arg(run_impl, reinterpret_cast<void*>(&controller_), 0, 0))
    (run_impl(reinterpret_cast<void*>(&controller_)));
  }

  static void run_impl(void* c) {
    auto& controller_ = *reinterpret_cast<msm::sm<controller>*>(c);
    do {
      auto dispatch_event = msm::make_dispatch_table<SDL_Event, SDL_QUIT, SDL_FINGERMOTION>(controller_);
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

} // match3
