#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "kwargscpp/kwargs.h"

namespace kwargscpp_test {
#ifdef WITH_STATIC
kwargscpp::DictType static_library_function();
#endif // WITH_STATIC

#ifdef WITH_SHARED
kwargscpp::DictType shared_library_function();
#endif // WITH_SHARED
}

TEST_CASE("Linking static and shared libraries") {
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

    // check they are not empty
    CHECK(!dict.empty());

    #ifdef WITH_STATIC
    const auto& dict_from_static  = kwargscpp_test::static_library_function();
    CHECK(!dict_from_static.empty());
    // check the contents of the dictionaries
    CHECK(dict == dict_from_static);
    #endif // WITH_STATIC

    #ifdef WITH_SHARED
    const auto& dict_from_shared = kwargscpp_test::shared_library_function();
    CHECK(!dict_from_shared.empty());
    // check the contents of the dictionaries
    CHECK(dict == dict_from_shared);
    #endif // WITH_SHARED
}
