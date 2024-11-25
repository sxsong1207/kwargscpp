#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>

#include <variant>

#include "kwargspp/kwargs.h"
#include "kwargspp/nanobind/binding.h"

namespace nb = nanobind;
using namespace nb::literals;

kwargspp::DictType echo_dict(const kwargspp::DictType& kwargs) {
    // Optionally, implement checks to handle or prevent cyclic references
    return kwargs;
}

kwargspp::DictType generate_dict()
{
  kwargspp::DictType dict;
  dict["int"] = kwargspp::ValueType(42);
  dict["double"] = kwargspp::ValueType(3.14);
  dict["bool"] = kwargspp::ValueType(true);
  dict["string"] = kwargspp::ValueType("hello");
  kwargspp::DictType nested_dict;
  nested_dict["inner_key"] = kwargspp::ValueType(42);
  dict["nested"] = kwargspp::ValueType(nested_dict);
  
  std::vector<kwargspp::ValueType> vec;
  vec.push_back(42);
  vec.push_back(3.14);
  vec.push_back("hello");
  vec.push_back(true);
  dict["vector_val"] = kwargspp::ValueType(vec);
  return dict;
}

NB_MODULE(bind_nanobind, m) {
  m.def("echo_dict", &echo_dict, "Echo the input dictionary");
  m.def("generate_dict", &generate_dict, "Generate a dictionary");
}