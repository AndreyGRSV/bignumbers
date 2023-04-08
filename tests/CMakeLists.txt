include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(bdig_tests bdig_tests.cpp)

if (BUILD_COVERAGE_INFO)
  target_link_libraries(bdig_tests GTest::gtest_main gcov)
else()
  target_link_libraries(bdig_tests GTest::gtest_main)
endif()

include(GoogleTest)
gtest_discover_tests(bdig_tests)