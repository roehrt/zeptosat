cmake_minimum_required(VERSION 3.10)
project(zeptosat)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wno-sign-conversion -Werror=uninitialized \
-Werror=return-type -Werror=return-local-addr")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O1 -g -ggdb3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb3 -fsanitize=address,undefined -fno-omit-frame-pointer \
-fno-sanitize-recover=all -fmax-errors=2 -D_GLIBCXX_DEBUG -D_LIBCPP_DEBUG -D_GLIBCXX_ASSERTIONS")

add_executable(zeptosat main.cpp)

enable_testing()
file(GLOB_RECURSE tests ${CMAKE_SOURCE_DIR}/tests/*.cnf)
foreach (test IN LISTS tests)
    get_filename_component(test_name ${test} NAME)
    string(REGEX REPLACE "\\.cnf" "" test_name ${test_name})
    add_test(NAME ${test_name} COMMAND python3 ${CMAKE_SOURCE_DIR}/test.py ${CMAKE_BINARY_DIR}/zeptosat ${test})
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 30)
endforeach ()
