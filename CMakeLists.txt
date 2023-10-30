cmake_minimum_required(VERSION 3.23)

project(bignumbers)

option(BUILD_COVERAGE_INFO "Option which allows build tests with coverage information" OFF)
option(BUILD_PERFORMANCE_INFO "Option which allows build tests with performance functions" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

if (BUILD_PERFORMANCE_INFO)
    add_definitions(-DBUILD_PERFORMANCE_INFO)
    message(">>>> Build performance functions is enabled")
endif()


if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic "$<$<CONFIG:RELEASE>:-msse3>")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options("$<$<CONFIG:RELEASE>:-Ofast3>")
    else()
      # nothing special for gcc at the moment
    endif()

    if (WIN32)
        set(CMAKE_CXX_STANDARD_REQUIRED OFF)
        set(CMAKE_CXX_EXTENSIONS ON)
        add_compile_options(-std=gnu++20)
    else(WIN32)
        if (BUILD_COVERAGE_INFO)
            message(">>>> Coverage target is enabled")
            add_compile_options("-O0" "-coverage")
            find_program(LCOV lcov REQUIRED)
            find_program(GENHTML genhtml REQUIRED)

            add_custom_target(coverage
            # gather data
            COMMAND ${LCOV} --gcov-tool /usr/bin/gcov-13 --directory . --capture --exclude '*/src/*' --exclude '/usr/include/*' --exclude '*/gtest/*' --exclude '*/tests/*' --output-file coverage.info
            # generate report
            COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        endif(BUILD_COVERAGE_INFO)
    endif(WIN32)
endif()

add_subdirectory(samples)
add_subdirectory(tests)
