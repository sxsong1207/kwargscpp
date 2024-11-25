#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "kwargspp/kwargs.h"

TEST_CASE("Test set and get with various types") {
    kwargspp::DictType dict;

    // Integer types
    int int_val = -42;
    short short_val = 123;
    uint8_t uint8_val = 255;
    uint16_t uint16_val = 65535;

    kwargspp::set(dict, "int_val", int_val);
    kwargspp::set(dict, "short_val", short_val);
    kwargspp::set(dict, "uint8_val", uint8_val);
    kwargspp::set(dict, "uint16_val", uint16_val);

    CHECK(kwargspp::get_or_die<intmax_t>(dict, "int_val") == int_val);
    CHECK(kwargspp::get_or_die<intmax_t>(dict, "short_val") == short_val);
    CHECK(kwargspp::get_or_die<uintmax_t>(dict, "uint8_val") == uint8_val);
    CHECK(kwargspp::get_or_die<uintmax_t>(dict, "uint16_val") == uint16_val);

    // Floating point types
    float float_val = 3.14f;
    double double_val = 2.71828;

    kwargspp::set(dict, "float_val", float_val);
    kwargspp::set(dict, "double_val", double_val);

    CHECK(kwargspp::get_or_die<double>(dict, "float_val") == doctest::Approx(float_val));
    CHECK(kwargspp::get_or_die<double>(dict, "double_val") == doctest::Approx(double_val));

    // Boolean type
    bool bool_val = true;
    kwargspp::set(dict, "bool_val", bool_val);
    CHECK(kwargspp::get_or_die<bool>(dict, "bool_val") == bool_val);

    // String type
    std::string string_val = "hello";
    kwargspp::set(dict, "string_val", string_val);
    CHECK(kwargspp::get_or_die<std::string>(dict, "string_val") == string_val);

    // Implicit conversions
    CHECK(kwargspp::get_or_die<float>(dict, "int_val") == doctest::Approx(static_cast<float>(int_val)));
    CHECK(kwargspp::get_or_die<int>(dict, "float_val") == static_cast<int>(float_val));
}

TEST_CASE("Test has_key function") {
    kwargspp::DictType dict;
    kwargspp::set(dict, "key1", 1);

    CHECK(kwargspp::has_key(dict, "key1") == true);
    CHECK(kwargspp::has_key(dict, "key2") == false);
}

TEST_CASE("Test get function with default values") {
    kwargspp::DictType dict;
    kwargspp::set(dict, "existing_key", 42);

    CHECK(kwargspp::get<int>(dict, "existing_key", 0) == 42);
    CHECK(kwargspp::get<int>(dict, "missing_key", -1) == -1);
}

TEST_CASE("Test with_prefix function") {
    kwargspp::DictType dict;
    kwargspp::set(dict, "key1", 1);
    kwargspp::set(dict, "key2", 2);

    auto prefixed_dict = kwargspp::with_prefix(dict, "pre_");

    CHECK(kwargspp::has_key(prefixed_dict, "pre_key1") == true);
    CHECK(kwargspp::has_key(prefixed_dict, "pre_key2") == true);
    CHECK(kwargspp::get<int>(prefixed_dict, "pre_key1", 0) == 1);
    CHECK(kwargspp::get<int>(prefixed_dict, "pre_key2", 0) == 2);
}

TEST_CASE("Test merge function") {
    kwargspp::DictType dict1;
    kwargspp::set(dict1, "key1", 1);

    kwargspp::DictType dict2;
    kwargspp::set(dict2, "key2", 2);

    auto merged_dict = kwargspp::merge(dict1, dict2);

    CHECK(kwargspp::has_key(merged_dict, "key1") == true);
    CHECK(kwargspp::has_key(merged_dict, "key2") == true);
    CHECK(kwargspp::get<int>(merged_dict, "key1", 0) == 1);
    CHECK(kwargspp::get<int>(merged_dict, "key2", 0) == 2);
}

TEST_CASE("Test exception handling in get_or_die") {
    kwargspp::DictType dict;
    kwargspp::set(dict, "key_int", 42);

    CHECK_THROWS_AS(kwargspp::get_or_die<std::string>(dict, "key_int"), std::bad_variant_access);
    CHECK_THROWS_AS(kwargspp::get_or_die<int>(dict, "missing_key"), std::runtime_error);
}

TEST_CASE("Test std::vector<ValueType> support") {
    kwargspp::DictType dict;

    // Create a vector of ValueType
    std::vector<kwargspp::ValueType> vec;
    vec.push_back(42);
    vec.push_back(3.14);
    vec.push_back("hello");

    kwargspp::set(dict, "vector_val", vec);

    auto retrieved_vec = kwargspp::get_or_die<std::vector<kwargspp::ValueType>>(dict, "vector_val");
    CHECK(std::get<intmax_t>(retrieved_vec[0]) == 42);
    CHECK(doctest::Approx(std::get<double>(retrieved_vec[1])) == 3.14);
    CHECK(std::get<std::string>(retrieved_vec[2]) == "hello");
}

TEST_CASE("Test nested DictType support") {
    kwargspp::DictType dict;

    // Create a nested DictType
    kwargspp::DictType nested_dict;
    kwargspp::set(nested_dict, "nested_key1", 100);
    kwargspp::set(nested_dict, "nested_key2", "nested_value");

    kwargspp::set(dict, "nested_dict", nested_dict);

    auto retrieved_dict = kwargspp::get_or_die<kwargspp::DictType>(dict, "nested_dict");
    CHECK(kwargspp::get_or_die<intmax_t>(retrieved_dict, "nested_key1") == 100);
    CHECK(kwargspp::get_or_die<std::string>(retrieved_dict, "nested_key2") == "nested_value");
}