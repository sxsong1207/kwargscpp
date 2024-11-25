#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "kwargscpp/kwargs.h"

#include <iostream>

TEST_CASE("Test set and get with various types") {
    kwargscpp::DictType dict;

    // Integer types
    int int_val = -42;
    short short_val = 123;
    uint8_t uint8_val = 255;
    uint16_t uint16_val = 65535;

    kwargscpp::set(dict, "int_val", int_val);
    kwargscpp::set(dict, "short_val", short_val);
    kwargscpp::set(dict, "uint8_val", uint8_val);
    kwargscpp::set(dict, "uint16_val", uint16_val);

    CHECK(kwargscpp::get_or_die<intmax_t>(dict, "int_val") == int_val);
    CHECK(kwargscpp::get_or_die<intmax_t>(dict, "short_val") == short_val);
    CHECK(kwargscpp::get_or_die<uintmax_t>(dict, "uint8_val") == uint8_val);
    CHECK(kwargscpp::get_or_die<uintmax_t>(dict, "uint16_val") == uint16_val);

    // Floating point types
    float float_val = 3.14f;
    double double_val = 2.71828;

    kwargscpp::set(dict, "float_val", float_val);
    kwargscpp::set(dict, "double_val", double_val);

    CHECK(kwargscpp::get_or_die<double>(dict, "float_val") == doctest::Approx(float_val));
    CHECK(kwargscpp::get_or_die<double>(dict, "double_val") == doctest::Approx(double_val));

    // Boolean type
    bool bool_val = true;
    kwargscpp::set(dict, "bool_val", bool_val);
    CHECK(kwargscpp::get_or_die<bool>(dict, "bool_val") == bool_val);

    // String type
    std::string string_val = "hello";
    kwargscpp::set(dict, "string_val", string_val);
    CHECK(kwargscpp::get_or_die<std::string>(dict, "string_val") == string_val);

    // Implicit conversions
    CHECK(kwargscpp::get_or_die<float>(dict, "int_val") == doctest::Approx(static_cast<float>(int_val)));
    CHECK(kwargscpp::get_or_die<int>(dict, "float_val") == static_cast<int>(float_val));

    std::cout<<kwargscpp::to_string(dict)<<std::endl;
}

TEST_CASE("Test has_key function") {
    kwargscpp::DictType dict;
    kwargscpp::set(dict, "key1", 1);

    CHECK(kwargscpp::has_key(dict, "key1") == true);
    CHECK(kwargscpp::has_key(dict, "key2") == false);

    std::cout<<kwargscpp::to_string(dict)<<std::endl;
}

TEST_CASE("Test get function with default values") {
    kwargscpp::DictType dict;
    kwargscpp::set(dict, "existing_key", 42);

    CHECK(kwargscpp::get<int>(dict, "existing_key", 0) == 42);
    CHECK(kwargscpp::get<int>(dict, "missing_key", -1) == -1);

    std::cout<<kwargscpp::to_string(dict)<<std::endl;
}

TEST_CASE("Test with_prefix function") {
    kwargscpp::DictType dict;
    kwargscpp::set(dict, "key1", 1);
    kwargscpp::set(dict, "key2", 2);

    auto prefixed_dict = kwargscpp::with_prefix(dict, "pre_");

    CHECK(kwargscpp::has_key(prefixed_dict, "pre_key1") == true);
    CHECK(kwargscpp::has_key(prefixed_dict, "pre_key2") == true);
    CHECK(kwargscpp::get<int>(prefixed_dict, "pre_key1", 0) == 1);
    CHECK(kwargscpp::get<int>(prefixed_dict, "pre_key2", 0) == 2);

    std::cout<<kwargscpp::to_string(prefixed_dict)<<std::endl;
}

TEST_CASE("Test merge function") {
    kwargscpp::DictType dict1;
    kwargscpp::set(dict1, "key1", 1);

    kwargscpp::DictType dict2;
    kwargscpp::set(dict2, "key2", 2);

    auto merged_dict = kwargscpp::merge(dict1, dict2);

    CHECK(kwargscpp::has_key(merged_dict, "key1") == true);
    CHECK(kwargscpp::has_key(merged_dict, "key2") == true);
    CHECK(kwargscpp::get<int>(merged_dict, "key1", 0) == 1);
    CHECK(kwargscpp::get<int>(merged_dict, "key2", 0) == 2);

    std::cout<<kwargscpp::to_string(merged_dict)<<std::endl;
}

TEST_CASE("Test exception handling in get_or_die") {
    kwargscpp::DictType dict;
    kwargscpp::set(dict, "key_int", 42);

    CHECK_THROWS_AS(kwargscpp::get_or_die<std::string>(dict, "key_int"), std::bad_variant_access);
    CHECK_THROWS_AS(kwargscpp::get_or_die<int>(dict, "missing_key"), std::runtime_error);

    std::cout<<kwargscpp::to_string(dict)<<std::endl;
}

TEST_CASE("Test std::vector<ValueType> support") {
    kwargscpp::DictType dict;

    // Create a vector of ValueType
    std::vector<kwargscpp::ValueType> vec;
    vec.push_back(42);
    vec.push_back(3.14);
    vec.push_back("hello");

    kwargscpp::set(dict, "vector_val", vec);

    auto retrieved_vec = kwargscpp::get_or_die<std::vector<kwargscpp::ValueType>>(dict, "vector_val");
    CHECK(std::get<intmax_t>(retrieved_vec[0]) == 42);
    CHECK(doctest::Approx(std::get<double>(retrieved_vec[1])) == 3.14);
    CHECK(std::get<std::string>(retrieved_vec[2]) == "hello");

    std::cout<<kwargscpp::to_string(vec)<<std::endl;
}

TEST_CASE("Test nested DictType support") {
    kwargscpp::DictType dict;

    // Create a nested DictType
    kwargscpp::DictType nested_dict;
    kwargscpp::set(nested_dict, "nested_key1", 100);
    kwargscpp::set(nested_dict, "nested_key2", "nested_value");

    kwargscpp::set(dict, "nested_dict", nested_dict);

    auto retrieved_dict = kwargscpp::get_or_die<kwargscpp::DictType>(dict, "nested_dict");
    CHECK(kwargscpp::get_or_die<intmax_t>(retrieved_dict, "nested_key1") == 100);
    CHECK(kwargscpp::get_or_die<std::string>(retrieved_dict, "nested_key2") == "nested_value");

    std::cout<<kwargscpp::to_string(dict)<<std::endl;
}