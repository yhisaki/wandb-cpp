#ifndef WANDBCPP_HPP_
#define WANDBCPP_HPP_

#include <Python.h>

#include <string>
#include <vector>

#include "src/py_object.hpp"
#include "src/py_util.hpp"

namespace wandbcpp {

class wandb {
  internal::object::SharedPyObjectPtr wandb_module_;
  internal::object::SharedPyObjectPtr run_;
  internal::object::SharedPyObjectPtr init_;
  internal::object::SharedPyObjectPtr log_;
  internal::object::SharedPyObjectPtr config_;
  internal::object::SharedPyObjectPtr save_;

 public:
  wandb();

  void init(const std::string& project, const std::string& name = "",
            const std::vector<std::string>& tags = {});
  void log(const internal::object::PyDict& logs);

  void save(const std::string& file_path);

  void add_config(const internal::object::PyDictItem conf) {
    internal::object::SharedPyObjectPtr conf_value(
        conf.get_pyobject_of_value());
    PyObject_SetAttrString(config_.get(), conf.key(), conf_value.get());
  }
};

void init(std::string project, std::string name = "",
          const std::vector<std::string>& tags = {});

void log(const internal::object::PyDict& logs);

void save(const std::string& file_path);

void add_config(
    const std::initializer_list<internal::object::PyDictItem>& confs);

}  // namespace wandbcpp

#endif