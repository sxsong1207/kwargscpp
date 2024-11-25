#ifndef KWARGS_H
#define KWARGS_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace kwargspp {

using KeyType = std::string;

// Forward declaration of DictType
struct ValueType;

using DictType = std::unordered_map<KeyType, ValueType>;

// Definition of ValueType
struct ValueType
    : public std::variant<intmax_t, uintmax_t, double, bool, std::string, std::vector<ValueType>, DictType> {
  using variant::variant;

  // Constructors for convenience
  ValueType() : variant() {}
  template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
  ValueType(const T &v) : variant(static_cast<std::conditional_t<std::is_signed_v<T>, intmax_t, uintmax_t>>(v)) {}
  ValueType(const double &v) : variant(v) {}
  ValueType(const bool &v) : variant(v) {}
  ValueType(const std::string &v) : variant(v) {}
  ValueType(const char *v) : variant(std::string(v)) {}
  ValueType(const std::vector<ValueType> &v) : variant(v) {}
  ValueType(const DictType &v) : variant(v) {}
};

inline std::string variant_to_string(const ValueType &value) {
  if (std::holds_alternative<intmax_t>(value)) {
    return std::to_string(std::get<intmax_t>(value));
  } else if (std::holds_alternative<uintmax_t>(value)) {
    return std::to_string(std::get<uintmax_t>(value));
  } else if (std::holds_alternative<double>(value)) {
    return std::to_string(std::get<double>(value));
  } else if (std::holds_alternative<bool>(value)) {
    return std::get<bool>(value) ? "true" : "false";
  } else if (std::holds_alternative<std::string>(value)) {
    return std::get<std::string>(value);
  } else if (std::holds_alternative<std::vector<ValueType>>(value)) {
    return "[...]";  // Simplified representation
  } else if (std::holds_alternative<DictType>(value)) {
    return "{...}";  // Simplified representation
  }
  return "";
}

inline void set(DictType &dict, const KeyType &key, const ValueType &value) { dict[key] = value; }

inline bool has_key(const DictType &dict, const KeyType &key) { return dict.find(key) != dict.end(); }

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

}  // namespace kwargs

#endif  // KWARGS_H