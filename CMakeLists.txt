cmake_minimum_required(VERSION 3.20)
project(poker_trainer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

add_library(core src/card.cpp src/evaluator.cpp)
target_include_directories(core PUBLIC src)

enable_testing()
add_executable(test_card tests/test_card.cpp)
target_link_libraries(test_card PRIVATE core)
add_test(NAME card COMMAND test_card)

add_executable(test_evaluator tests/test_evaluator.cpp)
target_link_libraries(test_evaluator PRIVATE core)
add_test(NAME evaluator COMMAND test_evaluator)