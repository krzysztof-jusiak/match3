//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "pph.hpp"
#include "view/icanvas.hpp"
#include "model/config.hpp"

namespace match3 {

class view {
  static constexpr auto grid_size = 38;
  static constexpr auto grid_offset = grid_size + 5;
  static constexpr auto grids_offset_x = 15;
  static constexpr auto grids_offset_y = 50;

 public:
  view(icanvas& canvas, config conf) : canvas_(canvas), config_(conf) {
    grids.reserve(config_.board_colors);
    for (auto i = 0; i <= config_.board_colors; ++i) {
      grids.emplace_back(
          canvas_.load_image("data/images/" + std::to_string(i) + ".png"));
    };

    match_ = canvas_.load_image("data/images/match.png");
  }

  void set_grid(int x, int y, int c) {
    canvas_.draw(grids[c], grids_offset_x + (x * grid_offset),
                 grids_offset_y + (y * grid_offset));
  }

  void update_grid(int x, int y) {
    canvas_.draw(match_, grids_offset_x + (x * grid_offset),
                 grids_offset_y + (y * grid_offset), false /*cleanup*/);
  }

  auto get_position(int x, int y) const noexcept {
    return (((y - grids_offset_y) / grid_offset) * config_.board_width) +
           ((x - grids_offset_x) / grid_offset);
  }

  void set_text(const std::string& text, int x, int y, int font_size = 14) {
    canvas_.draw(canvas_.create_text(text, "data/fonts/font.ttf", font_size), x,
                 y);
  }

  void update() { canvas_.render(); }
  void clear() { canvas_.clear(); }

 private:
  std::vector<std::shared_ptr<void>> grids;
  std::shared_ptr<void> match_;
  icanvas& canvas_;
  config config_;
};

}  // match3
