#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>

#include <variant>

#include "kwargscpp/kwargs.h"
#include "kwargscpp/pybind11/binding.h"

namespace py = pybind11;
using namespace py::literals;

kwargscpp::DictType echo_dict(const kwargscpp::DictType& kwargs) {
    // Optionally, implement checks to handle or prevent cyclic references
    return kwargs;
}

kwargscpp::DictType generate_dict()
{
  kwargscpp::DictType dict;
  dict["int"] = kwargscpp::ValueType(42);
  dict["double"] = kwargscpp::ValueType(3.14);
  dict["bool"] = kwargscpp::ValueType(true);
  dict["string"] = kwargscpp::ValueType("hello");
  kwargscpp::DictType nested_dict;
  nested_dict["inner_key"] = kwargscpp::ValueType(42);
  dict["nested"] = kwargscpp::ValueType(nested_dict);
  
  std::vector<kwargscpp::ValueType> vec;
  vec.push_back(42);
  vec.push_back(3.14);
  vec.push_back("hello");
  vec.push_back(true);
  dict["vector_val"] = kwargscpp::ValueType(vec);
  return dict;
}

PYBIND11_MODULE(bind_pybind11, m) {
  m.def("echo_dict", &echo_dict, "Echo the input dictionary");
  m.def("generate_dict", &generate_dict, "Generate a dictionary");
}