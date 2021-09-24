#include "src/wandb_object.hpp"

#include <iostream>
#include <utility>

#include "src/py_util.hpp"
namespace wandbcpp {

using namespace internal::object;

internal::object::SharedPyObjectPtr& Table::TablePointer() {
  static internal::object::SharedPyObjectPtr table_;
  return table_;
}

Table::Table(const std::vector<std::string>& columns)
    : columns_(columns.begin(), columns.end()) {}

Table::Table(const std::vector<std::string>& columns,
             const std::vector<internal::object::PyList>& data)
    : columns_(columns.begin(), columns.end()), data_(data) {}

void Table::add_data(const internal::object::PyList& l) {
  data_.emplace_back(l);
}

void Table::add_data(internal::object::PyList&& l) {
  data_.emplace_back(std::forward<internal::object::PyList>(l));
}

void Table::add_column(std::string name, const internal::object::PyList& l) {
  columns_.append(name);
  if (data_.size() != l.size()) {
    data_.resize(l.size());
  }
  for (size_t i = 0; i < l.size(); i++) {
    data_[i].append(l[i]);
  }
}

PyObject* Table::get_pyobject() const {
  SharedPyObjectPtr args(PyTuple().get_pyobject());
  SharedPyObjectPtr kwargs(
      PyDict(
          {{"columns", columns_}, {"data", PyList(data_.begin(), data_.end())}})
          .get_pyobject());
  return PyObject_Call(TablePointer().get(), args.get(), kwargs.get());
}


internal::object::SharedPyObjectPtr& Object3D::Object3DPointer() {
  static internal::object::SharedPyObjectPtr object3d_;
  return object3d_;
}

Object3D::Object3D(const internal::object::PyDict& dict)
    : dict_(dict), arr_(std::nullopt) {}

Object3D::Object3D(const numpy::ndarray& arr)
    : dict_(std::nullopt), arr_(arr) {}

PyObject* Object3D::get_pyobject() const {
  if (dict_) {
    SharedPyObjectPtr args(PyTuple(dict_.value()).get_pyobject());
    SharedPyObjectPtr kwargs(PyDict().get_pyobject());
    return PyObject_Call(Object3DPointer().get(), args.get(), kwargs.get());
  } else if (arr_) {
    SharedPyObjectPtr args(PyTuple(arr_.value()).get_pyobject());
    SharedPyObjectPtr kwargs(PyDict().get_pyobject());
    return PyObject_Call(Object3DPointer().get(), args.get(), kwargs.get());
  }
  return nullptr;
}

}  // namespace wandbcpp
