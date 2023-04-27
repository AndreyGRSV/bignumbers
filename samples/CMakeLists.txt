add_executable(calc_prime_num calc_prime_num.cpp)
add_executable(add_sub add_sub.cpp)

if (BUILD_COVERAGE_INFO)
  target_link_libraries(calc_prime_num gcov)
  target_link_libraries(add_sub gcov)
endif()

