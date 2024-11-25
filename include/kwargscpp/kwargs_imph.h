#ifndef KWARGS_IMPL_H
#define KWARGS_IMPL_H

#include "kwargs.h"

namespace kwargscpp {

// Constructors for convenience
inline ValueType::ValueType() : variant() {}

template <typename T, std::enable_if_t<std::is_integral_v<T>, bool>>
inline ValueType::ValueType(const T &v) : variant(static_cast<std::conditional_t<std::is_signed_v<T>, intmax_t, uintmax_t>>(v)) {}

inline ValueType::ValueType(const double &v) : variant(v) {}
inline ValueType::ValueType(const bool &v) : variant(v) {}
inline ValueType::ValueType(const std::string &v) : variant(v) {}
inline ValueType::ValueType(const char *v) : variant(std::string(v)) {}
inline ValueType::ValueType(const std::vector<ValueType> &v) : variant(v) {}
inline ValueType::ValueType(const DictType &v) : variant(v) {}

// Implementation of member functions
inline bool ValueType::is_int() const {
    return std::holds_alternative<intmax_t>(*this);
}

inline bool ValueType::is_uint() const {
    return std::holds_alternative<uintmax_t>(*this);
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

inline uintmax_t ValueType::as_uint() const {
    return std::get<uintmax_t>(*this);
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

template <typename T>
T get_or_die(const DictType &dict, const KeyType &key) {
  auto it = dict.find(key);
  if (it != dict.end()) {
    return std::visit(
        [](auto &&arg) -> T {
          using ArgType = std::decay_t<decltype(arg)>;
          // If the requested type matches the stored type, return it directly
          if constexpr (std::is_same_v<T, ArgType>) {
            return arg;
          }
          // Handle conversions between numeric types and bool
          else if constexpr ((std::is_integral_v<T> && std::is_arithmetic_v<ArgType>) ||
                             (std::is_floating_point_v<T> && std::is_arithmetic_v<ArgType>) ||
                             (std::is_same_v<T, bool> && std::is_arithmetic_v<ArgType>)) {
            // Perform static_cast for valid conversions
            return static_cast<T>(arg);
          }
          // Handle conversion from bool to numeric types
          else if constexpr (std::is_floating_point_v<T> && std::is_same_v<ArgType, bool>) {
            return arg ? static_cast<T>(1) : static_cast<T>(0);
          } else if constexpr (std::is_integral_v<T> && std::is_same_v<ArgType, bool>) {
            return arg ? static_cast<T>(1) : static_cast<T>(0);
          }
          // Handle nested DictType
          else if constexpr (std::is_same_v<T, DictType>) {
            if constexpr (std::is_same_v<ArgType, DictType>) {
              return arg;
            } else {
              throw std::bad_variant_access();
            }
          }
          // For strings, do not attempt conversion; throw an exception
          else if constexpr (std::is_same_v<T, std::string>) {
            throw std::bad_variant_access();  // Or a custom exception/message
          } else {
            // If conversion is not possible, throw an exception
            throw std::bad_variant_access();
          }
        },
        it->second);
  }
  throw std::runtime_error("Key not found in dictionary");
}

template <typename T>
T get(const DictType &dict, const KeyType &key, const T &default_value) {
  try {
    return get_or_die<T>(dict, key);
  } catch (const std::exception &e) {
    return default_value;
  }
}

inline DictType with_prefix(const DictType &dict, const std::string &prefix) {
  DictType out_dict;
  for (const auto &[key, value] : dict) {
    out_dict[prefix + key] = value;
  }
  return out_dict;
}

inline DictType merge(const DictType &dict, const DictType &other) {
  DictType out_dict(dict);
  for (const auto &[key, value] : other) {
    out_dict[key] = value;
  }
  return out_dict;
}

inline std::string to_string(const DictType &dict) {
    std::string result = "{";
    for (auto it = dict.begin(); it != dict.end(); ++it) {
        result += "\"" + it->first + "\": " + to_string(it->second);
        if (std::next(it) != dict.end()) {
            result += ", ";
        }
    }
    result += "}";
    return result;
}

inline std::string to_string(const ValueType &value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, intmax_t> || std::is_same_v<T, uintmax_t> || 
                      std::is_same_v<T, double>) {
            return std::to_string(arg);
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        }
        else if constexpr (std::is_same_v<T, std::vector<ValueType>>) {
            std::string s = "[";
            for (auto it = arg.begin(); it != arg.end(); ++it) {
                s += to_string(*it);
                if (std::next(it) != arg.end()) {
                    s += ", ";
                }
            }
            s += "]";
            return s;
        }
        else if constexpr (std::is_same_v<T, DictType>) {
            return to_string(arg);
        }
        else {
            return "";
        }
    }, value);
}

}  // namespace kwargscpp

#endif  // KWARGS_IMPL_H