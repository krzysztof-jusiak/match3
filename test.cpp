#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <range/v3/view/take.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/iota.hpp> // view::ints
#include <range/v3/view/join.hpp> // view::ints
#include <range/v3/algorithm/count.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/action/sort.hpp>
#include <range/v3/action/unique.hpp>

#define expect(...) (void)((__VA_ARGS__) || (expect_fail__(#__VA_ARGS__, __FILE__, __LINE__), 0))
#define static_expect(...) static_assert((__VA_ARGS__), "fail")

void expect_fail__(const char *msg, const char *file, int line) {
  std::printf("%s:%d:%s\n", file, line, msg);
  std::exit(-1);
}

struct test {
  template <class Test>
  test(const Test &test) {
    test();
  }
};

int main() {}

template<class T>
void print(const T& values) {
    for(int i=0; i < 10; ++i) {
        for(int j=0; j < 7; ++j) {
            std::cout << values[(i*7) +j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

auto row = [](auto&& v, auto n) {
    return v | ranges::view::drop(7 * n) | ranges::view::take(7);
};

auto col = [](auto&& v, auto n) {
    using namespace ranges;
    return v | ranges::view::drop(n) | ranges::view::stride(7);
};

auto match_n = [](auto&& v, auto colour, int n = 3) {
    const auto&& matches = ranges::view::ints | ranges::view::take(ranges::size(v) - n + 1) | ranges::view::transform(
      [=](auto i) { return ranges::count(v | ranges::view::drop(i) | ranges::view::take(n), colour) == n;}
    );

    constexpr auto is_match = 1;
    const auto it = ranges::find(matches, is_match);
    const auto found = it != ranges::end(matches);
    const auto length = found ? ranges::count(matches, is_match) + (n - 1) : 0;
    const auto begin = found ? ranges::distance(ranges::begin(matches), it) : 0;

    struct { decltype(begin) begin; decltype(length) length; } result{begin, length};
    return result;
};

auto is_match = [](auto&& v, auto value) {
    const auto colour = v[value];
    const auto x = value % 7;
    const auto y = value / 7;
    return match_n(row(v, y), colour).length || match_n(col(v, x), colour).length;
};

auto match = [](auto&& v, auto value) {
    const auto colour = v[value];
    const auto x = value % 7;
    const auto y = value / 7;
    const auto match_r = match_n(row(v, y), colour);
    const auto match_c = match_n(col(v, x), colour);
    const auto transform = [](auto length, auto expr) {
        return ranges::view::ints | ranges::view::take(length) | ranges::view::transform(expr);
    };
    std::vector<decltype(value)> result = ranges::view::concat(
        transform(match_r.length, [=](auto i) { return y * 7 + match_r.begin + i; })
      , transform(match_c.length, [=](auto i) { return (match_c.begin + i) * 7 + x; })
    );
    result |= ranges::action::sort | ranges::action::unique;
    return result;
};

auto mark = [](auto&& v, const auto& matches, auto expr) {
    ranges::for_each(matches, [&](auto i) { v[i] = expr(); });
};

auto scroll = [](auto&& v, const auto& matches) {
    ranges::for_each(matches, [&](auto i) {
        const auto&& c = col(v, i % 7) | ranges::view::take(i / 7 + 1);
        auto begin = ranges::begin(c);
        ranges::advance(begin, i / 7);
        ranges::rotate(c, begin);
    });
};

auto affected = [](auto&& v, const auto& matches) {
    const auto&& r = matches | ranges::view::transform([=](auto m) {
        return ranges::view::ints | ranges::view::take(m / 7 + 1) | ranges::view::transform([=](auto i) {
            return m % 7 + (i * 7);});
    });

    std::vector<int> result = (r | ranges::view::join);
    result |= ranges::action::sort | ranges::action::unique;
    return result;
};

test check = [] {
    int values[] = {
        3,5,1,4,3,2,1,
        1,1,4,2,5,1,3,
        5,3,5,4,5,3,2,
        4,4,5,5,5,4,5,
        5,1,1,2,4,5,1,
        5,2,3,5,4,2,2,
        1,5,5,1,5,5,4,
        4,3,3,1,3,3,4,
        3,2,2,5,4,4,1,
        1,2,3,2,1,3,4
    };
    print(values);

    auto v = match(values, 25);
    mark(values, v, [] { return 0; });
    print(values);
    auto vv = affected(values, v);
    for(const auto& q : vv) {
        std::cout << q << ":" << values[q] << std::endl;
    }
};
