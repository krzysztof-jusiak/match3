//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <utility>
#include <functional>
#include <type_traits>
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
#include <boost/di.hpp>
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
