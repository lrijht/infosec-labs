cmake_minimum_required(VERSION 3.16)
project(bdp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(biguint STATIC src/biguint.cpp)
target_include_directories(biguint PUBLIC src)

add_executable(bdp src/main.cpp)
target_link_libraries(bdp PRIVATE biguint)

enable_testing()
add_executable(test_biguint tests/test_biguint.cpp)
target_link_libraries(test_biguint PRIVATE biguint)
add_test(NAME biguint COMMAND test_biguint ${CMAKE_CURRENT_SOURCE_DIR}/tests/vectors.txt)

add_test(NAME cli_powmod COMMAND bdp powmod 4 13 497)
set_tests_properties(cli_powmod PROPERTIES PASS_REGULAR_EXPRESSION "^445[\r\n]*$")
add_test(NAME cli_trace COMMAND bdp powmod 4 13 497 --trace)
set_tests_properties(cli_trace PROPERTIES PASS_REGULAR_EXPRESSION "0\t1\t484\t445")
add_test(NAME cli_invalid_digit COMMAND bdp add 12a 1)
set_tests_properties(cli_invalid_digit PROPERTIES WILL_FAIL TRUE)
add_test(NAME cli_zero_modulus COMMAND bdp mod 5 0)
set_tests_properties(cli_zero_modulus PROPERTIES WILL_FAIL TRUE)
