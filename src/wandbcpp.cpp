#include "wandbcpp.hpp"

#include <Python.h>

#include "src/async_logging.hpp"

namespace wandbcpp {
using namespace std::literals;
using namespace internal;
using namespace internal::object;
using namespace internal::util;

wandb::wandb() {}

void wandb::init(const std::string& project, const std::string& name,
                 const std::vector<std::string>& tags) {
  if (Py_IsInitialized() == 0) {
    throw std::runtime_error("Error Python is not initialized!");
  }

  wandb_module_ = PyImport_ImportModule("wandb");

  if (wandb_module_.is_null()) {
    PyErr_Print();
    throw std::runtime_error("Error loading module wandb!");
  }
  init_ = PyObject_GetAttrString(wandb_module_.get(), "init");
  if (init_.is_null() || !PyCallable_Check(init_.get())) {
    PyErr_Print();
    throw std::runtime_error("Error loading wandb.init()!");
  }

  PyDict init_kwargs({{"project", project},
                      {"name", name},
                      {"tags", PyList(tags.begin(), tags.end())}});
  run_ = PyCall(init_, init_kwargs);

  log_ = PyObject_GetAttrString(wandb_module_.get(), "log");
  config_ = PyObject_GetAttrString(wandb_module_.get(), "config");
  summary_ = PyObject_GetAttrString(wandb_module_.get(), "summary");
  save_ = PyObject_GetAttrString(wandb_module_.get(), "save");
}

void wandb::log(const PyDict& logs) { PyCall(log_, PyTuple(logs)); }

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

using namespace internal::async;

std::unique_ptr<AsyncLoggingWorker> logging_worker;

void init(std::string project, std::string name,
          const std::vector<std::string>& tags) {
  logging_worker = std::make_unique<internal::async::AsyncLoggingWorker>();
  logging_worker->initialize_wandb(project, name, tags);
}
void log(const PyDict& logs) {
  if (!logging_worker) {
    logging_worker = std::make_unique<internal::async::AsyncLoggingWorker>();
  }
  logging_worker->append_log(logs);
}

void save(const std::string& file_path) {
  if (!logging_worker) {
    logging_worker = std::make_unique<internal::async::AsyncLoggingWorker>();
  }
  logging_worker->append_file_path(file_path);
}

void add_config(const std::initializer_list<PyDictItem>& confs) {
  if (!logging_worker) {
    logging_worker = std::make_unique<internal::async::AsyncLoggingWorker>();
  }
  for (const auto& conf : confs) {
    logging_worker->append_config(conf);
  }
}

void add_summary(const std::initializer_list<PyDictItem>& summs) {
  if (!logging_worker) {
    logging_worker = std::make_unique<internal::async::AsyncLoggingWorker>();
  }
  for (const auto& summ : summs) {
    logging_worker->append_summary(summ);
  }
}

void finish() { logging_worker.reset(); }

}  // namespace wandbcpp