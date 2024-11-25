#ifndef KWARGS_PYBIND11_H
#define KWARGS_PYBIND11_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>

#include "kwargspp/kwargs.h"

namespace py = pybind11;

namespace pybind11 {
namespace detail {

// Overloaded helper for std::visit (C++17)
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// Forward declaration of type_caster for DictType
template <>
struct type_caster<kwargspp::ValueType>;

// Type caster for ValueType
template <>
struct type_caster<kwargspp::ValueType> {
 public:
  PYBIND11_TYPE_CASTER(kwargspp::ValueType, _("kwargs::ValueType"));

  bool load(py::handle src, bool convert) {
    if (py::isinstance<py::int_>(src)) {
      value = py::cast<intmax_t>(src);
      return true;
    } else if (py::isinstance<py::float_>(src)) {
      value = py::cast<double>(src);
      return true;
    } else if (py::isinstance<py::bool_>(src)) {
      value = py::cast<bool>(src);
      return true;
    } else if (py::isinstance<py::str>(src)) {
      value = py::cast<std::string>(src);
      return true;
    } else if (py::isinstance<py::list>(src)) {
      std::vector<kwargspp::ValueType> list;
      py::object obj = py::reinterpret_borrow<py::object>(src);
      if (load_list(obj, list, convert)) {
        value = list;
        return true;
      }
    } else if (py::isinstance<py::dict>(src)) {
      // Recursively load the dictionary
      kwargspp::DictType dict;
      py::object obj = py::reinterpret_borrow<py::object>(src);
      if (load_dict(obj, dict, convert)) {
        value = dict;
        return true;
      }
    }
    return false;
  }

  static py::handle cast(const kwargspp::ValueType& src, py::return_value_policy policy, py::handle parent) {
    return std::visit(
        overloaded{[&](intmax_t v) { return py::int_(v).release(); },
                   [&](uintmax_t v) { return py::int_(v).release(); },
                   [&](double v) { return py::float_(v).release(); },
                   [&](bool v) { return py::bool_(v).release(); },
                   [&](const std::string& v) { return py::str(v.c_str(), v.length()).release(); },
                   [&](const std::vector<kwargspp::ValueType>& vec) -> py::handle {
                     // Convert kwargspp::ValueType to Python list
                     py::list py_list;
                     for (const auto& val : vec) {
                       py::handle h = type_caster<kwargspp::ValueType>::cast(val, policy, parent);
                       if (!h) {
                         return py::handle();  // Handle casting failure
                       }
                       py_list.append(h);
                     }
                     return py_list.release();
                   },
                   [&](const kwargspp::DictType& dict) -> py::handle {
                     // Convert kwargspp::DictType to Python dict
                     py::dict py_dict;
                     for (const auto& [key, val] : dict) {
                       py::handle h = type_caster<kwargspp::ValueType>::cast(val, policy, parent);
                       if (!h) {
                         return py::handle();  // Handle casting failure
                       }
                       py_dict[key.c_str()] = h;
                     }
                     return py_dict.release();
                   }},
        src);
  }

 private:
  // Helper function to recursively load a Python list into std::vector<kwargspp::ValueType>
  bool load_list(py::object src, std::vector<kwargspp::ValueType>& dest, bool convert) {
    if (!py::isinstance<py::list>(src)) return false;

    py::list py_list = py::reinterpret_borrow<py::list>(src);
    for (auto item : py_list) {
      kwargspp::ValueType val;
      py::handle value_handle = item;
      type_caster<kwargspp::ValueType> value_caster;
      if (!value_caster.load(value_handle, convert)) {
        return false;
      }
      val = value_caster.value;

      dest.push_back(val);
    }
    return true;
  }
  // Helper function to recursively load a Python dict into kwargspp::DictType
  bool load_dict(py::object src, kwargspp::DictType& dest, bool convert) {
    if (!py::isinstance<py::dict>(src)) return false;

    py::dict py_dict = py::reinterpret_borrow<py::dict>(src);
    for (auto item : py_dict) {
      std::string key = py::cast<std::string>(item.first);

      kwargspp::ValueType val;
      py::handle value_handle = item.second;
      type_caster<kwargspp::ValueType> value_caster;
      if (!value_caster.load(value_handle, convert)) {
        return false;
      }
      val = value_caster.value;

      dest[key] = val;
    }
    return true;
  }
};
}  // namespace detail
}  // namespace pybind11

#endif  // KWARGS_PYBIND11_H