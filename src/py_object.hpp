#ifndef WANDBCPP_PY_OBJECTS_HPP_
#define WANDBCPP_PY_OBJECTS_HPP_

#include <Python.h>

#include <memory>
#include <utility>
#include <vector>

namespace wandbcpp::internal::object {

struct CastablePyObjectBase {
  virtual PyObject* get_pyobject() const = 0;
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
class PyBasicType : public CastablePyObjectBase {
 private:
  ValueType value;

 public:
  PyBasicType(const ValueType& value) : value(value) {}
  PyObject* get_pyobject() const;
};

class PyDictItem {
  std::string key_;
  std::shared_ptr<CastablePyObjectBase> value_;

 public:
  PyDictItem(std::string key);
  template <class ValueType>
  PyDictItem(const std::string& key, const ValueType& value)
      : key_(key), value_(new PyBasicType<ValueType>(value)) {}

  template <class ValueType>
  explicit PyDictItem(const std::pair<std::string, ValueType>& key_value_pair)
      : key_(key_value_pair.first),
        value_(new PyBasicType<ValueType>(key_value_pair.second)) {}

  const char* key() const;
  template <class ValueType>
  PyDictItem& operator=(const ValueType value) {
    value_.reset(new PyBasicType<ValueType>(value));
    return *this;
  }
  PyObject* get_pyobject_of_value() const;
};

class PyDict : public CastablePyObjectBase {
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

class PyList : public CastablePyObjectBase {
  std::vector<std::shared_ptr<CastablePyObjectBase>> items_;

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
    items_.emplace_back(new PyBasicType<T>(v));
  }
  PyObject* get_pyobject() const;
};

class PyTuple : public CastablePyObjectBase {
  std::vector<std::shared_ptr<CastablePyObjectBase>> items_;

 public:
  PyTuple();
  template <class... Items>
  PyTuple(const Items&... items) {
    using swallow = int[];
    (void)swallow{(append(items), 0)...};
  }
  template <class T>
  void append(const T& v) {
    items_.emplace_back(new PyBasicType<T>(v));
  }
  PyObject* get_pyobject() const;
};
}  // namespace wandbcpp::internal::object

#endif