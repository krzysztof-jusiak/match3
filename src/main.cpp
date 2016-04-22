#include <boost/di.hpp>

namespace di = boost::di;

auto configuration = [] {
  using namespace match3;

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

int main() {
  auto injector = di::make_injector(configuration());
  injector.create<match3::game>().play();
}
