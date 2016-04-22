//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <random>
#include <functional>
#include <range/v3/view/take.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/iota.hpp>  // view::ints
#include <range/v3/view/join.hpp>
#include <range/v3/algorithm/count.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/action/transform.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>
#include <range/v3/action/push_back.hpp>
#include <range/v3/action/push_front.hpp>
#include <boost/msm-lite.hpp>
// clang-format off
#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#elif __has_include(<SDL.h>)
#include <SDL.h>
#endif
#if __has_include(<SDL2/SDL_image.h>)
#include <SDL2/SDL_image.h>
#elif __has_include(<SDL_image.h>)
#include <SDL_image.h>
#endif
#if __has_include(<SDL2/SDL_ttf.h>)
#include <SDL2/SDL_ttf.h>
#elif __has_include(<SDL_ttf.h>)
#include <SDL_ttf.h>
#endif
// clang-format on
#if defined(EMSCRIPTEN)
#include <emscripten.h>
#define EM(...) __VA_ARGS__ EM_ELSE
#define EM_ELSE(...)
#else
#define EM(...) EM_ELSE
#define EM_ELSE(...) __VA_ARGS__
#define EM_ASM_INT_V(...) 0
#endif

namespace msm = boost::msm::lite;

namespace match3 {

using Sprite = SDL_Texture;

struct config {
  std::string win_title;
  int win_width;
  int win_height;
  int board_width;
  int board_height;
  int board_colors;
};

class icanvas {
 public:
  virtual ~icanvas() noexcept = default;
  virtual std::shared_ptr<Sprite> load_image(const std::string&) const = 0;
  virtual std::shared_ptr<Sprite> create_text(const std::string&, const std::string&, int = 14) const = 0;
  virtual void draw(std::shared_ptr<Sprite>, int x = 0, int y = 0, bool = true) = 0;
  virtual void render() = 0;
};

class iclient {
 public:
  virtual ~iclient() noexcept = default;
  virtual void run() = 0;
};

/// -----
/// View
/// -----

class sdl_canvas : public icanvas {
  static constexpr auto RENDER_DRIVER = -1;
  static constexpr auto RENDER_FLAGS = SDL_RENDERER_ACCELERATED;

 public:
  explicit sdl_canvas(config c) noexcept {
    assert(!SDL_Init(SDL_INIT_VIDEO));
    window_ = std::shared_ptr<SDL_Window>(SDL_CreateWindow(c.win_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                           c.win_width, c.win_height, SDL_WINDOW_SHOWN),
                                          SDL_DestroyWindow);
  }

  ~sdl_canvas() noexcept override {
    SDL_Quit();
  }

  void draw(std::shared_ptr<Sprite> texture, int x, int y, bool clean) override { }

  std::shared_ptr<Sprite> load_image(const std::string& file) const override {
      return {};
  }

  std::shared_ptr<Sprite> create_text(const std::string& str, const std::string& font_file, int font_size) const override {
      return {};
  }

  void render() override { }

 private:
  std::shared_ptr<SDL_Window> window_;
};

class view {
};

/// ------------
/// Model/Board
/// ------------

struct board {
  using color = int;
  std::vector<color> grids;
};

struct quit {
  static constexpr auto id = SDL_QUIT;
};

struct key_pressed {
  static constexpr auto id = SDL_KEYDOWN;
  explicit key_pressed(const SDL_Event& event) : key(event.key.keysym.sym) {}
  int key = 0;
};

/// -----------
/// Controller
/// -----------

struct controller {
  auto configure() const noexcept {
    using namespace msm;

    // clang-format off
    return make_transition_table(
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
        "wait_for_first_item"_s  <= *("idle"_s)                                     / (show_board, show_points)
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      , X                        <= *("wait_for_client"_s)     + event<key_pressed> [ is_key<SDLK_ESCAPE> ]
      , X                        <=   "wait_for_client"_s      + event<quit>
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
    );
    // clang-format on
  }
};

class sdl_user : public iclient {
 public:
  explicit sdl_user(msm::sm<controller>& c) : controller_(c) {}

  void run() override {
    EM(emscripten_set_main_loop_arg(run_impl, reinterpret_cast<void*>(&controller_), 0, 0))
    (run_impl(reinterpret_cast<void*>(&controller_)));
  }

  static void run_impl(void* c) {
    auto& controller_ = *reinterpret_cast<msm::sm<controller>*>(c);
    do {
      auto dispatch_event = msm::make_dispatch_table<SDL_Event, SDL_QUIT, SDL_FINGERMOTION>(controller_);
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        dispatch_event(event, event.type);
      }
    } while (EM(false && )() !controller_.is(msm::X));
  }

 private:
  msm::sm<controller>& controller_;
};

class game {
 public:
  game(msm::sm<controller>& c, std::vector<std::shared_ptr<iclient>> cl) : controller_(c), clients_(cl) {}

  void play() {
    for (const auto& c : clients_) {
      c->run();
    }
  }

  std::vector<std::shared_ptr<iclient>> clients_;
  msm::sm<controller>& controller_;
};

}  // match3
