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
#include <boost/di.hpp>
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

namespace di = boost::di;
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
    assert(!TTF_Init());
    window_ = std::shared_ptr<SDL_Window>(SDL_CreateWindow(c.win_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                           c.win_width, c.win_height, SDL_WINDOW_SHOWN),
                                          SDL_DestroyWindow);
    renderer_ =
        std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(window_.get(), RENDER_DRIVER, RENDER_FLAGS), SDL_DestroyRenderer);
  }

  ~sdl_canvas() noexcept override {
    TTF_Quit();
    SDL_Quit();
  }

  void draw(std::shared_ptr<Sprite> texture, int x, int y, bool clean) override {
    SDL_Rect pos{x, y, 0, 0};
    SDL_QueryTexture(texture.get(), nullptr, nullptr, &pos.w, &pos.h);
    if (clean) {
      elements_.erase(ranges::remove_if(elements_, [=](const auto& pair) { return pair.second.x == x && pair.second.y == y; }),
                      elements_.end());
    }
    elements_.push_back(std::make_pair(texture, pos));
  }

  std::shared_ptr<Sprite> load_image(const std::string& file) const override {
    return std::shared_ptr<Sprite>(IMG_LoadTexture(renderer_.get(), file.c_str()), SDL_DestroyTexture);
  }

  std::shared_ptr<Sprite> create_text(const std::string& str, const std::string& font_file, int font_size) const override {
    std::shared_ptr<TTF_Font> font(TTF_OpenFont(font_file.c_str(), font_size), TTF_CloseFont);
    std::shared_ptr<SDL_Surface> surface(TTF_RenderText_Blended(font.get(), str.c_str(), SDL_Color{255, 255, 255}),
                                         SDL_FreeSurface);
    return std::shared_ptr<Sprite>(SDL_CreateTextureFromSurface(renderer_.get(), surface.get()), SDL_DestroyTexture);
  }

  void render() override {
    SDL_RenderClear(renderer_.get());
    for (const auto& element : elements_) {
      SDL_RenderCopy(renderer_.get(), element.first.get(), nullptr, &element.second);
    }
    SDL_RenderPresent(renderer_.get());
  }

 private:
  std::shared_ptr<SDL_Window> window_;
  std::shared_ptr<SDL_Renderer> renderer_;
  std::vector<std::pair<std::shared_ptr<Sprite>, SDL_Rect>> elements_;
};

class view {
  static constexpr auto grid_size = 38;
  static constexpr auto grid_offset = grid_size + 5;
  static constexpr auto grids_offset_x = 15;
  static constexpr auto grids_offset_y = 50;

 public:
  view(icanvas& canvas, config conf) : canvas_(canvas), config_(conf) {
    grids.reserve(config_.board_colors);
    for (auto i = 0; i <= config_.board_colors; ++i) {
      grids.emplace_back(canvas_.load_image("images/" + std::to_string(i) + ".png"));
    };

    match_ = canvas_.load_image("images/match.png");
  }

  void set_grid(int x, int y, int c) {
    canvas_.draw(grids[c], grids_offset_x + (x * grid_offset), grids_offset_y + (y * grid_offset));
  }

  void update_grid(int x, int y) {
    canvas_.draw(match_, grids_offset_x + (x * grid_offset), grids_offset_y + (y * grid_offset), false /*cleanup*/);
  }

  auto get_position(int x, int y) const noexcept {
    return (((y - grids_offset_y) / grid_offset) * config_.board_width) + ((x - grids_offset_x) / grid_offset);
  }

  void set_text(const std::string& text, int x, int y) { canvas_.draw(canvas_.create_text(text, "fonts/font.ttf"), x, y); }
  void update() { canvas_.render(); }

 private:
  std::vector<std::shared_ptr<Sprite>> grids;
  std::shared_ptr<Sprite> match_;
  icanvas& canvas_;
  config config_;
};

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

/// ------------
/// Model/Board
/// ------------

struct board {
  using color = int;
  std::vector<color> grids;
};

using selected = std::vector<short>;
using points = int;

auto row = [](auto&& v, auto n, auto width) { return v | ranges::view::drop(width * n) | ranges::view::take(width); };
auto col = [](auto&& v, auto n, auto width) { return v | ranges::view::drop(n) | ranges::view::stride(int(width)); };

auto match_n = [](auto&& v, auto color, int n = 3) {
  const auto&& matches = ranges::view::ints | ranges::view::take(ranges::size(v) - n + 1) | ranges::view::transform([=](int i) {
                           return ranges::count(v | ranges::view::drop(i) | ranges::view::take(n), color) == n;
                         });

  constexpr auto is_match = 1;
  const auto it = ranges::find(matches, is_match);
  const auto found = it != ranges::end(matches);
  const auto length = found ? ranges::count(matches, is_match) + (n - 1) : 0;
  const auto begin = found ? ranges::distance(ranges::begin(matches), it) : 0;

  struct {
    decltype(begin) begin;
    decltype(length) length;
  } result{begin, length};
  return result;
};

auto is_match = [](auto&& v, auto value, auto width) {
  const auto color = v[value];
  const auto x = value % width;
  const auto y = value / width;
  return match_n(row(v, y, width), color).length || match_n(col(v, x, width), color).length;
};

auto match = [](auto&& v, auto value, auto width) {
  const auto color = v[value];
  const auto x = value % width;
  const auto y = value / width;
  const auto match_r = match_n(row(v, y, width), color);
  const auto match_c = match_n(col(v, x, width), color);
  const auto transform = [](int length, auto expr) {
    return ranges::view::ints | ranges::view::take(length) | ranges::view::transform(expr);
  };
  std::vector<decltype(value)> result =
      ranges::view::concat(transform(match_r.length, [=](int i) { return y * width + match_r.begin + i; }),
                           transform(match_c.length, [=](int i) { return (match_c.begin + i) * width + x; }));
  result |= ranges::action::sort | ranges::action::unique;
  return result;
};

auto scroll = [](auto&& v, const auto& match, auto width) {
  const auto&& c = col(v, match % width, width) | ranges::view::take(match / width + 1);
  auto begin = ranges::begin(c);
  ranges::advance(begin, match / width);
  ranges::rotate(c, begin);
};

auto affected = [](auto&& v, const auto& matches, auto width) {
  const auto&& columns = matches | ranges::view::transform([=](int m) {
                           return ranges::view::ints | ranges::view::take(m / width + 1) |
                                  ranges::view::transform([=](int i) { return m % width + (i * width); });
                         });

  std::vector<int> result = columns | ranges::view::join;
  result |= ranges::action::sort | ranges::action::unique;
  return result;
};

/// ---------------
/// Guards
/// ---------------

auto is_mobile = [] { return bool(EM_ASM_INT_V({ return / iPhone | iPad | iPod | Android / i.test(navigator.userAgent); })); };

auto are_selected = [](const selected& s) { return !s.empty(); };

auto is_swap_items_winning = [](const board& b, const selected& s, config c) {
  assert(s.size() >= 2);
  return is_match(b.grids, *(s.end() - 1), c.board_width) || is_match(b.grids, *(s.end() - 2), c.board_width);
};

auto is_item_winning = [](const board& b, const selected& s, config c) {
  return !s.empty() && is_match(b.grids, *(s.end() - 1), c.board_width);
};

auto is_allowed = [](auto event, const view& v, const selected& s, config c) {
  assert(!s.empty());
  const auto _1 = s.back();
  const auto _2 = v.get_position(event.x, event.y);
  const auto diff = std::abs(_1 - _2);
  const auto board_size = c.board_width * c.board_height;
  return (_1 >= 0 && _1 < board_size) && (_2 >= 0 && _2 < board_size) && (diff == 1 || diff == c.board_width);
};

template <int Key>
auto is_key = [](auto event) { return event.key == Key; };

/// ---------------
/// Actions
/// ---------------

auto select_item = [](auto event, const view& v, selected& s) { s.emplace_back(v.get_position(event.x, event.y)); };
auto drop = [](selected& s) {
  assert(!s.empty());
  s.pop_back();
};
auto clean_matches = [](board& b, const auto& m) { ranges::for_each(m.matches, [&](auto i) { b.grids[i] = {}; }); };
auto clear_selected = [](selected& s) { s.clear(); };
auto show_points = [](view& v, const points& p) { v.set_text("points: " + std::to_string(p), 10, 10); };
auto add_points = [](points& p, const auto& m) { p += m.matches.size(); };

auto swap_items = [](const selected& s, board& b) {
  assert(s.size() == 2);
  const auto _1 = *s.begin();
  const auto _2 = *(s.begin() + 1);
  std::swap(b.grids[_1], b.grids[_2]);
};

auto add_matches = [](const board& b, auto& m, selected& s, animations& a, config c) {
  assert(m.arity >= 0);
  auto arity = m.arity;
  while (arity--) {
    m.matches |= ranges::action::push_back(match(b.grids, s.back(), c.board_width));
    s.pop_back();
  }
  m.matches |= ranges::action::sort | ranges::action::unique;
};

auto scroll_board = [](board& b, const auto& m, config c) {
  ranges::for_each(m.matches, [&](auto i) { scroll(b.grids, i, c.board_width); });
};

auto add_new = [](board& b, const auto& m, selected& s, config c) {
  ranges::action::transform(b.grids, [c](auto i) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, c.board_colors);
    return i ? i : dis(gen);
  });
  s |= ranges::action::push_front(affected(b.grids, m.matches, c.board_width)) | ranges::action::sort | ranges::action::unique;
};

auto show_swap = [](const board& b, const selected& s, animations& a, view& v, config c) {
  assert(s.size() == 2);
  a.queue_animation([b, c, s, &v] {
      const auto _1 = *s.begin();
      v.set_grid(_1 % c.board_width, _1 / c.board_width, b.grids[_1]);
      const auto _2 = *(s.begin() + 1);
      v.set_grid(_2 % c.board_width, _2 / c.board_width, b.grids[_2]);
  }, EM(10)(10));
};

auto show_board = [](const board& b, animations& a, view& v, config c) {
  a.queue_animation([b, c, &v] {
    for (auto i = 0; i < c.board_width * c.board_height; ++i) {
      v.set_grid(i % c.board_width, i / c.board_width, b.grids[i]);
    }
  }, EM(10)(10));
};

auto show_matches = [](const auto& m, animations& a, view& v, config c) {
  a.queue_animation([m, c, &v] {
    for (auto i : m.matches) {
      v.update_grid(i % c.board_width, i / c.board_width);
    }
  }, EM(10)(10));
};

/// -------
/// Events
/// -------

struct touch_down {
  static constexpr auto id = SDL_FINGERDOWN;
  explicit touch_down(const SDL_Event& event) : x(int(event.tfinger.x)), y(int(event.tfinger.y)) {}
  int x = 0;
  int y = 0;
};

struct button_down {
  static constexpr auto id = SDL_MOUSEBUTTONDOWN;
  explicit button_down(const SDL_Event& event) : x(event.button.x), y(event.button.y) {}
  int x = 0;
  int y = 0;
};

struct touch_up {
  static constexpr auto id = SDL_FINGERUP;
  explicit touch_up(const SDL_Event& event) : x(int(event.tfinger.x)), y(int(event.tfinger.y)) {}
  int x = 0;
  int y = 0;
};

struct button_up {
  static constexpr auto id = SDL_MOUSEBUTTONUP;
  explicit button_up(const SDL_Event& event) : x(event.button.x), y(event.button.y) {}
  int x = 0;
  int y = 0;
};

struct matches {
  static constexpr auto id = __LINE__;
  int arity = 0;
  mutable std::vector<int> matches;
};

struct time_tick {
  static constexpr auto id = __LINE__;
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

      , "wait_for_second_item"_s <=   "wait_for_first_item"_s  + event<touch_down>  [is_mobile] / select_item
      , "match_items"_s          <=   "wait_for_second_item"_s + event<touch_up>    [is_mobile && is_allowed] / (select_item, swap_items, show_swap)
      , "wait_for_first_item"_s  <=   "wait_for_second_item"_s + event<touch_up>    [is_mobile && !is_allowed] / clear_selected

      , "wait_for_second_item"_s <=   "wait_for_first_item"_s  + event<button_down> [!is_mobile] / select_item
      , "match_items"_s          <=   "wait_for_second_item"_s + event<button_up>   [!is_mobile && is_allowed] / (select_item, swap_items, show_swap)
      , "wait_for_first_item"_s  <=   "wait_for_second_item"_s + event<button_up>   [!is_mobile && !is_allowed] / clear_selected

      , "wait_for_first_item"_s  <=   "match_items"_s                               [ is_swap_items_winning ] / msm::queue_event(matches{.arity = 2})
      , "wait_for_first_item"_s  <=   "match_items"_s                               / (swap_items, clear_selected, show_board)
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      ,                             *("handle_matches"_s)      + event<matches>     [are_selected && is_item_winning] / (
                                                                                       add_matches, show_matches
                                                                                     , clean_matches, show_board
                                                                                     , add_points, show_points
                                                                                     , scroll_board, show_board
                                                                                     , add_new, show_board
                                                                                     , msm::queue_event(matches{.arity = 1})
                                                                                    )
      ,                               "handle_matches"_s       + event<matches>     [are_selected && !is_item_winning] / (
                                                                                      drop, msm::queue_event(matches{.arity = 1})
                                                                                    )
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      , X                        <= *("wait_for_client"_s)     + event<key_pressed> [ is_key<SDLK_ESCAPE> ]
      , X                        <=   "wait_for_client"_s      + event<quit>
     // +--------------------------------------------------------------------------------------------------------------------------------------------+
      ,                             *("handle_animations"_s)   + event<time_tick>   / [](animations& a) { a.update(); }
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

auto configuration = [] {
  // clang-format off
  return di::make_injector(
    di::bind<icanvas>.to<sdl_canvas>()
  , di::bind<iclient*[]>.to<sdl_user>()
  , di::bind<>.to(config{.win_title = "match3",
                         .win_width = 320,
                         .win_height = 480,
                         .board_width = 7,
                         .board_height = 10,
                         .board_colors = 5})
  , di::bind<board::color[]>.to({
        3,5,1,4,3,2,1,
        1,1,4,2,5,1,3,
        5,3,5,4,5,3,2,
        4,4,2,1,3,4,5,
        5,1,1,2,4,5,1,
        5,2,3,5,4,2,2,
        1,5,5,1,5,5,4,
        2,3,3,1,3,3,4,
        3,2,2,5,4,4,1,
        1,2,3,4,1,3,4})
  );
  // clang-format on
};

}  // match3

int main() {
  auto injector = di::make_injector(match3::configuration());
  injector.create<match3::game>().play();
}
