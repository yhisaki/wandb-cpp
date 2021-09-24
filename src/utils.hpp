#ifndef WANDBCPP_UTILS_HPP_
#define WANDBCPP_UTILS_HPP_

#include <iterator>
#include <type_traits>

#include "src/py_object.hpp"
namespace wandbcpp {

inline namespace utils {

#if __GNUC__ < 10
template <class I>
internal::object::PyList topylist(I begin, I end) {
  static_assert(internal::object::is_iterator<I>::value);
  if constexpr (std::is_arithmetic_v<
                    typename std::iterator_traits<I>::value_type>) {
    return {begin, end};
  } else {
    std::vector<internal::object::PyList> data;
    for (auto i = begin; i != end; i++) {
      data.emplace_back(topylist(i->begin(), i->end()));
    }
    return {data.begin(), data.end()};
  }
}
#else
template <std::input_iterator I>
internal::object::PyList topylist(I begin, I end) {
  if constexpr (std::is_arithmetic_v<
                    typename std::iterator_traits<I>::value_type>) {
    return {begin, end};
  } else {
    std::vector<internal::object::PyList> data;
    for (auto i = begin; i != end; i++) {
      data.emplace_back(topylist(i->begin(), i->end()));
    }
    return {data.begin(), data.end()};
  }
}
#endif

}  // namespace utils

}  // namespace wandbcpp

#endif