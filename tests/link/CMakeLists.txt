add_library(dummy_lib_shared SHARED library_shared.cpp)
add_library(dummy_lib_static STATIC library_static.cpp)

add_executable(tests_link main.cpp)
target_link_libraries(tests_link PRIVATE dummy_lib_shared dummy_lib_static doctest::doctest)

add_test(NAME tests_link COMMAND tests_link)
