#ifndef WANDBCPP_HPP_
#define WANDBCPP_HPP_

#include <Python.h>

#include <string>
#include <vector>

#include "src/py_object.hpp"
#include "src/py_util.hpp"
#include "src/wandb_object.hpp"

namespace wandbcpp {

class wandb {
  internal::object::SharedPyObjectPtr wandb_module_;
  internal::object::SharedPyObjectPtr run_;
  internal::object::SharedPyObjectPtr init_;
  internal::object::SharedPyObjectPtr log_;
  internal::object::SharedPyObjectPtr config_;
  internal::object::SharedPyObjectPtr summary_;
  internal::object::SharedPyObjectPtr save_;

 public:
  wandb();

  struct init_args {
    std::string project;
    std::string entity = "";
    std::string name = "";
    std::vector<std::string> tags = {};
  };

  void init(const init_args& ia);

  void log(const internal::object::PyDict& logs);

  // void log(internal::object::PyDict&& logs);

  void save(const std::string& file_path);

  void add_config(const internal::object::PyDictItem& conf);

  void add_summary(const internal::object::PyDictItem& summ);

  enum class wandb_mode { online, offline, disabled };
  static wandb_mode get_mode();
};

// void init(std::string project, std::string entity = "", std::string name =
// "",
//           const std::vector<std::string>& tags = {});

using log_dict = internal::object::PyDict;

using py_list = internal::object::PyList;
using py_dict = internal::object::PyDict;

void init(const wandb::init_args& ia, bool wait_initialized = true);

void log(const internal::object::PyDict& logs);

void log(internal::object::PyDict&& logs);

void save(const std::string& file_path);

void add_config(const std::vector<internal::object::PyDictItem>& confs);

void add_config(std::vector<internal::object::PyDictItem>&& confs);

void add_summary(const std::vector<internal::object::PyDictItem>& summs);

void add_summary(std::vector<internal::object::PyDictItem>&& summs);

void finish();

}  // namespace wandbcpp

#endif