#ifndef WANDB_OBJECT_HPP_
#define WANDB_OBJECT_HPP_
#include <optional>
#include <string>
#include <vector>

#include "src/np_object.hpp"
#include "src/py_object.hpp"
#include "wandbcpp.hpp"

namespace wandbcpp {

class Table : public internal::object::PyObjectBaseClonable<Table> {
  friend class wandb;
  internal::object::PyList columns_;
  std::vector<internal::object::PyList> data_;
  static internal::object::SharedPyObjectPtr& TablePointer();

 public:
  Table() = default;
  explicit Table(const std::vector<std::string>& columns);
  Table(const std::vector<std::string>& columns,
        const std::vector<internal::object::PyList>& data);
  void add_data(const internal::object::PyList& l);
  void add_data(internal::object::PyList&& l);
  void add_column(std::string name, const internal::object::PyList& l);
  PyObject* get_pyobject() const override;
};

class Object3D : public internal::object::PyObjectBaseClonable<Object3D> {
  friend class wandb;
  static internal::object::SharedPyObjectPtr& Object3DPointer();

 private:
  std::optional<internal::object::PyDict> dict_;
  std::optional<numpy::ndarray> arr_;

 public:
  Object3D() = default;
  explicit Object3D(const internal::object::PyDict& dict);
  explicit Object3D(const numpy::ndarray& arr);

  PyObject* get_pyobject() const override;
};

}  // namespace wandbcpp

#endif