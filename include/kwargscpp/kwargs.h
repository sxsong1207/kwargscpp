#ifndef KWARGS_H
#define KWARGS_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace kwargscpp {

using KeyType = std::string;
struct ValueType;
using DictType = std::unordered_map<KeyType, ValueType>;

// Definition of ValueType
struct ValueType
    : public std::variant<intmax_t, uintmax_t, double, bool, std::string, std::vector<ValueType>, DictType> {
  using variant::variant;

    // Constructors for convenience
    ValueType();
    template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
    ValueType(const T &v);
    ValueType(const double &v);
    ValueType(const bool &v);
    ValueType(const std::string &v);
    ValueType(const char *v);
    ValueType(const std::vector<ValueType> &v);
    ValueType(const DictType &v);

    // Member functions
    bool is_int() const;
    bool is_uint() const;
    bool is_double() const;
    bool is_bool() const;
    bool is_string() const;
    bool is_vector() const;
    bool is_dict() const;

    intmax_t as_int() const;
    uintmax_t as_uint() const;
    double as_double() const;
    bool as_bool() const;
    const std::string& as_string() const;
    const std::vector<ValueType>& as_vector() const;
    const DictType& as_dict() const;
};

// string representation of the dictionary
std::string to_string(const DictType &dict);
// string representation of the value
std::string to_string(const ValueType &value);

// set a key-value pair in the dictionary
inline void set(DictType &dict, const KeyType &key, const ValueType &value) { dict[key] = value; }
// check if a key exists in the dictionary
inline bool has_key(const DictType &dict, const KeyType &key) { return dict.find(key) != dict.end(); }

// get a value from the dictionary
template <typename T>
T get_or_die(const DictType &dict, const KeyType &key);
// get a value from the dictionary with a default value
template <typename T>
T get(const DictType &dict, const KeyType &key, const T &default_value);

// add a prefix to all keys in the dictionary, not recursive to nested dictionaries
DictType with_prefix(const DictType &dict, const std::string &prefix);

// merge two dictionaries, the second dictionary overwrites the first one, not recursive to nested dictionaries
DictType merge(const DictType &dict, const DictType &other);

}  // namespace kwargs

#include "kwargs_imph.h"
#endif  // KWARGS_H