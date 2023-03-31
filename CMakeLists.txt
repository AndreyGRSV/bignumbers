cmake_minimum_required(VERSION 3.23)

project(bignumbers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options("-pedantic")
    # google tests requred GNU extention and c++17 starndard for cygwin64
    if (WIN32)
        set(CMAKE_CXX_STANDARD_REQUIRED OFF)
        set(CMAKE_CXX_EXTENSIONS ON)
        add_compile_options(-std=gnu++17)
    endif(WIN32)
endif()

add_subdirectory(samples)
add_subdirectory(tests)
