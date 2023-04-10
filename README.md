# Bignumbers

[![Minimum C++ Standard](https://img.shields.io/badge/standard-C%2B%2B98-blue)](https://img.shields.io/badge/standard-C%2B%2B98-blue)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ef991af3383e45afa5935294098e794f)](https://app.codacy.com/gh/AndreyGRSV/bignumbers/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![codecov](https://codecov.io/gh/AndreyGRSV/bignumbers/branch/develop/graph/badge.svg?token=W2YSS77274)](https://codecov.io/gh/AndreyGRSV/bignumbers)
[![Build](https://github.com/AndreyGRSV/bignumbers/actions/workflows/build.yml/badge.svg)](https://github.com/AndreyGRSV/bignumbers/actions/workflows/build.yml)
[![CodeQL](https://github.com/AndreyGRSV/bignumbers/actions/workflows/codeql.yml/badge.svg)](https://github.com/AndreyGRSV/bignumbers/actions/workflows/codeql.yml)


Main goal is to create template for simple usage of big numbers with fixed point that can be used with compilers of C++98 standard.

Usage:

```C++
//Just include several files:
#include <limits> 
#include <string>
#include "bdig.hpp"

// and set
using namespace sag;

int main()
{
   // first template parameter - digits before point
   // second template parameter - digits after point
   // third template parameter - element type of array for representation 
   // Can initialize with integral int
  bdig <200, 200, unsigned long> digit = 1234567890;
  digit *= 100;
  // Can initialize with string
  digit = "1234567890.123456789012345678901234567890";
  digit -= "1234567890";
}
```
