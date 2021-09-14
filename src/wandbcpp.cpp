#include "wandbcpp.hpp"

#include <Python.h>

#include <fstream>
#include <iterator>
#include <optional>

#include "src/async_logging.hpp"
#define PY_ARRAY_UNIQUE_SYMBOL cool_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/arrayobject.h"

namespace wandbcpp {
using namespace std::literals;
using namespace internal;
using namespace internal::object;
using namespace internal::util;

wandb::wandb() {}

void wandb::init(const init_args& ia) {
  if (Py_IsInitialized() == 0) {
    throw std::runtime_error("Error Python is not initialized!");
  }

  wandb_module_ = PyImport_ImportModule("wandb");
  int i = _import_array();
  if (i < 0) {
    throw std::runtime_error("import numpy failed");
  }

  if (wandb_module_.is_null()) {
    PyErr_Print();
    throw std::runtime_error("Error loading module wandb!");
  }
  init_ = PyObject_GetAttrString(wandb_module_.get(), "init");
  if (init_.is_null() || !PyCallable_Check(init_.get())) {
    PyErr_Print();
    throw std::runtime_error("Error loading wandb.init()!");
  }

  PyDict init_kwargs({{"project", ia.project},
                      {"name", ia.name},
                      {"tags", PyList(ia.tags.begin(), ia.tags.end())}});
  if (!ia.entity.empty()) init_kwargs["entity"] = ia.entity;
  run_ = PyCall(init_, init_kwargs);

  log_ = PyObject_GetAttrString(wandb_module_.get(), "log");
  config_ = PyObject_GetAttrString(wandb_module_.get(), "config");
  summary_ = PyObject_GetAttrString(wandb_module_.get(), "summary");
  save_ = PyObject_GetAttrString(wandb_module_.get(), "save");
  Table::TablePointer() = PyObject_GetAttrString(wandb_module_.get(), "Table");
  Object3D::Object3DPointer() =
      PyObject_GetAttrString(wandb_module_.get(), "Object3D");
}

void wandb::log(const PyDict& logs) { PyCall(log_, PyTuple(logs)); }

// void wandb::log(PyDict&& logs) { PyCall(log_, PyTuple(logs)); }

void wandb::save(const std::string& file_path) {
  PyCall(save_, PyTuple(file_path));
}

void wandb::add_config(const internal::object::PyDictItem& conf) {
  internal::object::SharedPyObjectPtr conf_value(conf.get_pyobject_of_value());
  PyObject_SetAttrString(config_.get(), conf.key(), conf_value.get());
}

void wandb::add_summary(const internal::object::PyDictItem& summ) {
  internal::object::SharedPyObjectPtr summ_value(summ.get_pyobject_of_value());
  PyObject_SetAttrString(summary_.get(), summ.key(), summ_value.get());
}

wandb::wandb_mode wandb::get_mode() {
  static std::optional<wandb_mode> mode;

  if (!mode) {
    std::ifstream ifs("wandb/settings");
    if (ifs.is_open()) {
      std::string wandb_settings((std::istreambuf_iterator<char>(ifs)),
                                 std::istreambuf_iterator<char>());
      if (wandb_settings.find("mode") == std::string::npos) {
        mode = wandb_mode::online;
      } else {
        if (wandb_settings.find("mode = online") != std::string::npos) {
          mode = wandb_mode::online;
        } else if (wandb_settings.find("mode = offline") != std::string::npos) {
          mode = wandb_mode::offline;
        } else if (wandb_settings.find("disabled = true") !=
                   std::string::npos) {
          mode = wandb_mode::disabled;
        } else {
          mode = wandb_mode::online;
        }
      }
    } else {
      mode = wandb_mode::online;
    }
  }

  return mode.value();
}

using namespace internal::async;

std::unique_ptr<AsyncLoggingWorker> logging_worker;

void preprocessing() {
  if (!logging_worker) {
    logging_worker = std::make_unique<internal::async::AsyncLoggingWorker>();
  }
}

void init(const wandb::init_args& ia, bool wait_initialized) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  logging_worker->initialize_wandb(ia);
  if (wait_initialized) logging_worker->wait_initialized();
}

void log(const PyDict& logs) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  logging_worker->append_log(logs);
}

void log(PyDict&& logs) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  logging_worker->append_log(std::forward<PyDict>(logs));
}

void save(const std::string& file_path) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  logging_worker->append_file_path(file_path);
}

void add_config(const std::vector<PyDictItem>& confs) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  for (const auto& conf : confs) {
    logging_worker->append_config(conf);
  }
}

void add_config(std::vector<PyDictItem>&& confs) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  for (auto&& conf : confs) {
    logging_worker->append_config(std::forward<PyDictItem>(conf));
  }
}

void add_summary(const std::vector<PyDictItem>& summs) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  for (const auto& summ : summs) {
    logging_worker->append_summary(summ);
  }
}

void add_summary(std::vector<PyDictItem>&& summs) {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  preprocessing();
  for (auto&& summ : summs) {
    logging_worker->append_summary(std::forward<PyDictItem>(summ));
  }
}

void finish() {
  if (wandb::get_mode() == wandb::wandb_mode::disabled) {
    return;
  }
  if (logging_worker) {
    logging_worker.reset();
  }
}

}  // namespace wandbcpp