add_library(dummy_lib_shared SHARED library_shared.cpp)
target_link_libraries(dummy_lib_shared PRIVATE kwargscpp)
add_library(dummy_lib_static STATIC library_static.cpp)
target_link_libraries(dummy_lib_static PRIVATE kwargscpp)

add_executable(tests_link main.cpp)
target_link_libraries(tests_link PUBLIC kwargscpp PRIVATE doctest::doctest)

target_link_libraries(tests_link PRIVATE dummy_lib_static)
target_compile_definitions(tests_link PRIVATE -DWITH_STATIC)

target_link_libraries(tests_link PRIVATE dummy_lib_shared)
target_compile_definitions(tests_link PRIVATE -DWITH_SHARED)

add_test(NAME tests_link COMMAND tests_link)
