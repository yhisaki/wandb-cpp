#ifndef WANDBCPP_WANDB_OBJECT_HPP_
#define WANDBCPP_WANDB_OBJECT_HPP_

#include <Python.h>

#include <string>
#include <vector>

#include "src/py_object.hpp"
#include "src/py_util.hpp"

namespace wandbcpp {

class TableData {
  std::vector<internal::object::PyList> data_;

 public:
  template <class... Args>
  void emplace_back(const Args&... args) {
    
  }
};

class Table {
 public:
  Table(const std::vector<std::string>& columns);
};

}  // namespace wandbcpp

#endif