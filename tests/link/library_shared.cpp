#include "kwargspp/kwargs.h"

namespace kwargspp_test {

kwargspp::DictType shared_library_function() {
    kwargspp::DictType dict;
    kwargspp::set(dict, "flag", true);
    kwargspp::set(dict, "pi", 3.1415);

    kwargspp::DictType nested_dict;
    kwargspp::set(nested_dict, "key1", 42);
    kwargspp::set(nested_dict, "key2", std::string("value"));
    nested_dict["key3"] = 3.14;
    nested_dict["key4"] = true;

    kwargspp::set(dict, "nested", nested_dict);
    dict["nested"] = nested_dict;
    return dict;
}

} // namespace kwargspp_test
