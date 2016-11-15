//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <boost/di.hpp>
#include <type_traits>
#include "fakeit.hpp"

namespace di = boost::di;
template <class T>
auto& mock(bool reset = true) {
  using namespace fakeit;
  static Mock<T> mock;
  if (reset) {
    mock.Reset();
  }
  When(Dtor(mock)).AlwaysDo([] {});
  return mock;
}

struct mocks_provider : di::config {
  struct mock_provider {
    template <class...>
    struct is_creatable {
      static constexpr auto value = true;
    };

    template <class T, class... TArgs>
    auto get(const di::type_traits::direct&, const di::type_traits::heap&,
             TArgs&&... args) {
      return new T(static_cast<TArgs&&>(args)...);
    }

    template <class T, class... TArgs>
    std::enable_if_t<!std::is_polymorphic<T>::value, T*> get(
        const di::type_traits::uniform&, const di::type_traits::heap&,
        TArgs&&... args) {
      return new T{static_cast<TArgs&&>(args)...};
    }

    template <class T, class... TArgs>
    std::enable_if_t<std::is_polymorphic<T>::value, T*> get(
        const di::type_traits::uniform&, const di::type_traits::heap&,
        TArgs&&...) {
      return &mock<T>(false).get();
    }

    template <class T, class... TArgs>
    auto get(const di::type_traits::direct&, const di::type_traits::stack&,
             TArgs&&... args) const noexcept {
      return T(static_cast<TArgs&&>(args)...);
    }

    template <class T, class... TArgs>
    auto get(const di::type_traits::uniform&, const di::type_traits::stack&,
             TArgs&&... args) const noexcept {
      return T{static_cast<TArgs&&>(args)...};
    }
  };

 public:
  static auto provider(...) noexcept { return mock_provider{}; }
};
