#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "kwargspp/kwargs.h"

namespace kwargspp_test {
kwargspp::DictType static_library_function();
kwargspp::DictType shared_library_function();
}

TEST_CASE("Linking static and shared libraries") {
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

    const auto& dict_from_static  = kwargspp_test::static_library_function();
    const auto& dict_from_shared = kwargspp_test::shared_library_function();


    // check they are not empty
    CHECK(!dict.empty());
    CHECK(!dict_from_static.empty());
    CHECK(!dict_from_shared.empty());
    // check the contents of the dictionaries
    CHECK(dict == dict_from_static);
    CHECK(dict == dict_from_shared);
}
