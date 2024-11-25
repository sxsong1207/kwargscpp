#ifndef KWARGS_NANOBIND_H
#define KWARGS_NANOBIND_H

#include <nanobind/nanobind.h>

#include "kwargscpp/kwargs.h"

namespace nb = nanobind;

namespace nanobind {
namespace detail {

// Overloaded helper for std::visit (C++17)
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// // Forward declaration of type_caster for DictType
template <>
struct type_caster<kwargscpp::ValueType>;

// Type caster for ValueType
template <>
struct type_caster<kwargscpp::ValueType> {
 public:
  NB_TYPE_CASTER(kwargscpp::ValueType, const_name("kwargs::ValueType"));

  bool from_python(nb::handle src, uint8_t flags, cleanup_list* cleanup) noexcept {
    if (nb::isinstance<nb::int_>(src)) {
      value = nb::cast<intmax_t>(src);
      return true;
    } else if (nb::isinstance<nb::float_>(src)) {
      value = nb::cast<double>(src);
      return true;
    } else if (nb::isinstance<nb::bool_>(src)) {
      value = nb::cast<bool>(src);
      return true;
    } else if (nb::isinstance<nb::str>(src)) {
      value = nb::cast<std::string>(src);
      return true;
    } else if (nb::isinstance<nb::list>(src)) {
      std::vector<kwargscpp::ValueType> list;
      nb::object obj = nb::borrow<nb::object>(src);
      if (load_list(obj, list, flags, cleanup)) {
        value = list;
        return true;
      }
    } else if (nb::isinstance<nb::dict>(src)) {
      // Recursively load the dictionary
      kwargscpp::DictType dict;
      nb::object obj = nb::borrow<nb::object>(src);
      if (load_dict(obj, dict, flags, cleanup)) {
        value = dict;
        return true;
      }
    }
    return false;
  }

  static nb::handle from_cpp(const kwargscpp::ValueType& src, rv_policy policy, cleanup_list* cleanup) noexcept {
    return std::visit(
        overloaded{[&](intmax_t v) { return nb::int_(v).release(); },
                   [&](uintmax_t v) { return nb::int_(v).release(); },
                   [&](double v) { return nb::float_(v).release(); },
                   [&](bool v) { return nb::bool_(v).release(); },
                   [&](const std::string& v) { return nb::str(v.c_str(), v.length()).release(); },
                   [&](const std::vector<kwargscpp::ValueType>& vec) -> nb::handle {
                     // Convert kwargscpp::ValueType to Python list
                     nb::list py_list;
                     for (const auto& val : vec) {
                       nb::handle h = type_caster<kwargscpp::ValueType>::from_cpp(val, policy, cleanup);
                       if (!h) {
                         return nb::handle();  // Handle casting failure
                       }
                       py_list.append(h);
                     }
                     return py_list.release();
                   },
                   [&](const kwargscpp::DictType& dict) -> nb::handle {
                     // Convert kwargscpp::DictType to Python dict
                     nb::dict py_dict;
                     for (const auto& [key, val] : dict) {
                       nb::handle h = type_caster<kwargscpp::ValueType>::from_cpp(val, policy, cleanup);
                       if (!h) {
                         return nb::handle();  // Handle casting failure
                       }
                       py_dict[key.c_str()] = h;
                     }
                     return py_dict.release();
                   }},
        src);
  }

 private:
  // Helper function to recursively load a Python list into std::vector<kwargscpp::ValueType>
  bool load_list(nb::object src, std::vector<kwargscpp::ValueType>& dest, uint8_t flags, cleanup_list* cleanup) {
    if (!nb::isinstance<nb::list>(src)) return false;

    nb::list py_list = nb::borrow<nb::list>(src);
    for (auto item : py_list) {
      kwargscpp::ValueType val;
      nb::handle value_handle = item;
      type_caster<kwargscpp::ValueType> value_caster;
      if (!value_caster.from_python(value_handle, flags, cleanup)) {
        return false;
      }
      val = value_caster.value;

      dest.push_back(val);
    }
    return true;
  }
  // Helper function to recursively load a Python dict into kwargscpp::DictType
  bool load_dict(nb::object src, kwargscpp::DictType& dest, uint8_t flags, cleanup_list* cleanup) {
    if (!nb::isinstance<nb::dict>(src)) return false;

    nb::dict py_dict = nb::borrow<nb::dict>(src);
    for (auto item : py_dict) {
      std::string key = nb::cast<std::string>(item.first);

      kwargscpp::ValueType val;
      nb::handle value_handle = item.second;
      type_caster<kwargscpp::ValueType> value_caster;
      if (!value_caster.from_python(value_handle, flags, cleanup)) {
        return false;
      }
      val = value_caster.value;

      dest[key] = val;
    }
    return true;
  }
};
}  // namespace detail
}  // namespace nanobind

#endif  // KWARGS_NANOBIND_H