//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <string>

template <class T>
auto get_type() {
#if defined(_MSC_VER)
  auto type = std::string{&__FUNCSIG__[29]};
  return type.substr(0, type.length() - 7);
#elif defined(__clang__)
  auto type = std::string{&__PRETTY_FUNCTION__[21]};
  return type.substr(0, type.length() - 1);
#elif defined(__GCC__)
  auto type = std::string{&__PRETTY_FUNCTION__[26]};
  return type.substr(0, type.length() - 1);
#else
#error "Platform not supported!"
#endif
}

template <class SM, class TEvent>
void log_process_event(const TEvent&) {
  printf("[%s][process_event] %s\n", get_type<SM>().c_str(), get_type<TEvent>().c_str());
}

template <class SM, class TAction, class TEvent>
void log_guard(const TAction&, const TEvent&, bool result) {
  printf("[%s][guard] %s %s %s\n", get_type<SM>().c_str(), get_type<TAction>().c_str(), get_type<TEvent>().c_str(),
         (result ? "[OK]" : "[Reject]"));
}

template <class SM, class TAction, class TEvent>
void log_action(const TAction&, const TEvent&) {
  printf("[%s][action] %s %s\n", get_type<SM>().c_str(), get_type<TAction>().c_str(), get_type<TEvent>().c_str());
}

template <class SM, class TSrcState, class TDstState>
void log_state_change(const TSrcState& src, const TDstState& dst) {
  printf("[%s][transition] %s -> %s\n", get_type<SM>().c_str(), src.c_str(), dst.c_str());
}

#define BOOST_MSM_LITE_LOG(T, SM, ...) log_##T<SM>(__VA_ARGS__)
