#ifndef KWARGS_IMPL_H
#define KWARGS_IMPL_H

#include "kwargs.h"

namespace kwargspp {

// Implementation of member functions
inline bool ValueType::is_int() const {
  return std::holds_alternative<intmax_t>(*this);
}

inline bool ValueType::is_double() const {
  return std::holds_alternative<double>(*this);
}

inline bool ValueType::is_bool() const {
  return std::holds_alternative<bool>(*this);
}

inline bool ValueType::is_string() const {
  return std::holds_alternative<std::string>(*this);
}

inline bool ValueType::is_vector() const {
  return std::holds_alternative<std::vector<ValueType>>(*this);
}

inline bool ValueType::is_dict() const {
  return std::holds_alternative<DictType>(*this);
}

inline intmax_t ValueType::as_int() const {
  return std::get<intmax_t>(*this);
}

inline double ValueType::as_double() const {
  return std::get<double>(*this);
}

inline bool ValueType::as_bool() const {
  return std::get<bool>(*this);
}

inline const std::string& ValueType::as_string() const {
  return std::get<std::string>(*this);
}

inline const std::vector<ValueType>& ValueType::as_vector() const {
  return std::get<std::vector<ValueType>>(*this);
}

inline const DictType& ValueType::as_dict() const {
  return std::get<DictType>(*this);
}

}  // namespace kwargspp

#endif  // KWARGS_IMPL_H