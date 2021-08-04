#include "src/py_util.hpp"

#include <iostream>
#include <memory>

namespace wandbcpp::internal::util {

using namespace object;

SharedPyObjectPtr PyCall(const SharedPyObjectPtr& func) {
  PyTuple args;
  PyDict kwargs;
  SharedPyObjectPtr args_ptr(args.get_pyobject());
  SharedPyObjectPtr kwargs_ptr(kwargs.get_pyobject());
  return SharedPyObjectPtr(
      PyObject_Call(func.get(), args_ptr.get(), kwargs_ptr.get()));
}

SharedPyObjectPtr PyCall(const SharedPyObjectPtr& func, const PyDict& kwargs) {
  PyTuple args;
  SharedPyObjectPtr args_ptr(args.get_pyobject());
  SharedPyObjectPtr kwargs_ptr(kwargs.get_pyobject());
  return SharedPyObjectPtr(
      PyObject_Call(func.get(), args_ptr.get(), kwargs_ptr.get()));
}

SharedPyObjectPtr PyCall(const SharedPyObjectPtr& func, const PyTuple& args) {
  PyDict kwargs;
  SharedPyObjectPtr args_ptr(args.get_pyobject());
  SharedPyObjectPtr kwargs_ptr(kwargs.get_pyobject());
  return SharedPyObjectPtr(
      PyObject_Call(func.get(), args_ptr.get(), kwargs_ptr.get()));
}

void print_pyobject(PyObject* obj, int idx) {
  PyObject_Print(obj, stdout, idx);
  printf("\n");
}

}  // namespace wandbcpp::internal::util
