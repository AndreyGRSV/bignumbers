cmake_minimum_required(VERSION 3.23)

project(bignumbers)

option(BUILD_COVERAGE_INFO "Option which allows build tests with coverage information")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options("-pedantic")

    if (WIN32)
        set(CMAKE_CXX_STANDARD_REQUIRED OFF)
        set(CMAKE_CXX_EXTENSIONS ON)
        add_compile_options(-std=gnu++17)
    else(WIN32)
        if (BUILD_COVERAGE_INFO)
            message("Coverage target is enabled")
            add_compile_options("-O0" "-coverage")
            find_program(LCOV lcov REQUIRED)
            find_program(GENHTML genhtml REQUIRED)

            add_custom_target(coverage
            # gather data
            COMMAND ${LCOV} --directory . --capture --exclude '*/src/*' --exclude '/usr/include/*' --exclude '*/gtest/*' --exclude '*/tests/*' --output-file coverage.info
            # generate report
            COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        endif(BUILD_COVERAGE_INFO)
    endif(WIN32)
endif()

add_subdirectory(samples)
add_subdirectory(tests)
