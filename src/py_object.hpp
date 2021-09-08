#ifndef WANDBCPP_PY_OBJECTS_HPP_
#define WANDBCPP_PY_OBJECTS_HPP_

#include <Python.h>

#include <memory>
#include <utility>
#include <vector>

namespace wandbcpp::internal::object {

struct PyObjectBase {
  virtual PyObject* get_pyobject() const = 0;
  virtual PyObjectBase* clone() const = 0;
};

template <class Derived>
struct PyObjectBaseClonable : public PyObjectBase {
  PyObjectBase* clone() const override {
    return new Derived(static_cast<Derived const&>(*this));
  }
};

class SharedPyObjectPtr {
  PyObject* ptr_;

 public:
  SharedPyObjectPtr();
  explicit SharedPyObjectPtr(PyObject* ptr);
  SharedPyObjectPtr& operator=(PyObject* ptr);
  SharedPyObjectPtr& operator=(const SharedPyObjectPtr& ptr);
  ~SharedPyObjectPtr();
  SharedPyObjectPtr(const SharedPyObjectPtr& src);
  PyObject* get() const;
  bool is_null();
  PyObject* operator->();
};

template <class ValueType>
class PyBasicType
    : public PyObjectBaseClonable<PyBasicType<ValueType>> {
 private:
  ValueType value;

 public:
  PyBasicType(const ValueType& value) : value(value) {}
  PyObject* get_pyobject() const;
};

template <class T>
PyObjectBase* create_pybjectbase_ptr(const T& t) {
  if constexpr (std::is_base_of_v<PyObjectBase, T>) {
    return t.clone();
  } else {
    return new PyBasicType<T>(t);
  }
}

class PyDictItem {
  std::string key_;
  std::shared_ptr<PyObjectBase> value_;

 public:
  PyDictItem(std::string key);
  PyDictItem(const std::string& key, const PyObjectBase& value);
  template <class ValueType>
  PyDictItem(const std::string& key, const ValueType& value)
      : key_(key), value_(create_pybjectbase_ptr(value)) {}
  template <class ValueType>
  explicit PyDictItem(const std::pair<std::string, ValueType>& key_value_pair)
      : key_(key_value_pair.first),
        value_(create_pybjectbase_ptr(key_value_pair.second)) {}

  const char* key() const;
  template <class ValueType>
  PyDictItem& operator=(const ValueType value) {
    value_.reset(create_pybjectbase_ptr(value));
    return *this;
  }
  PyObject* get_pyobject_of_value() const;
};

class PyDict : public PyObjectBaseClonable<PyDict> {
 private:
  std::vector<PyDictItem> items_;

 public:
  PyDict(const std::initializer_list<PyDictItem>& items);
  PyDict();
  PyDict(const PyDict& other);
  PyDictItem& operator[](std::string key);
  void update(const PyDict& other);
  void clear();
  bool empty() const;
  PyObject* get_pyobject() const;
};

class PyList : public PyObjectBaseClonable<PyList> {
  std::vector<std::shared_ptr<PyObjectBase>> items_;

 public:
  PyList() {}

  template <class T>
  PyList(std::initializer_list<T> items) {
    items_.reserve(items.size());
    for (auto& e : items) {
      append(e);
    }
  }
  template <class InputIterator>
  PyList(InputIterator first, InputIterator last) {
    for (InputIterator it = first; it != last; it++) {
      append(*it);
    }
  }
  template <class T>
  void append(const T& v) {
    items_.emplace_back(create_pybjectbase_ptr(v));
  }
  PyObject* get_pyobject() const;
};

class PyTuple : public PyObjectBaseClonable<PyTuple> {
  std::vector<std::shared_ptr<PyObjectBase>> items_;

 public:
  PyTuple();
  template <class... Items>
  PyTuple(const Items&... items) {
    using swallow = int[];
    (void)swallow{(append(items), 0)...};
  }

  template <class T>
  void append(const T& v) {
    items_.emplace_back(create_pybjectbase_ptr(v));
  }
  PyObject* get_pyobject() const;
};
}  // namespace wandbcpp::internal::object

#endif