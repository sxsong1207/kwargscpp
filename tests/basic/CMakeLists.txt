add_executable(tests_basic main.cpp)

target_link_libraries(tests_basic PRIVATE doctest::doctest)

add_test(NAME tests_basic COMMAND tests_basic)