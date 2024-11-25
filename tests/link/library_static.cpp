#include "kwargscpp/kwargs.h"

namespace kwargscpp_test {

kwargscpp::DictType static_library_function() {
    kwargscpp::DictType dict;
    kwargscpp::set(dict, "flag", true);
    kwargscpp::set(dict, "pi", 3.1415);

    kwargscpp::DictType nested_dict;
    kwargscpp::set(nested_dict, "key1", 42);
    kwargscpp::set(nested_dict, "key2", std::string("value"));
    nested_dict["key3"] = 3.14;
    nested_dict["key4"] = true;

    kwargscpp::set(dict, "nested", nested_dict);
    dict["nested"] = nested_dict;
    return dict;
}

} // namespace kwargscpp_test
