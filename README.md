# bignumbers

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/9db81fd6023c4df18a42a86820c4cd0a)](https://www.codacy.com/manual/AndreyGRSV/bignumbers?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=AndreyGRSV/bignumbers&amp;utm_campaign=Badge_Grade)

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
