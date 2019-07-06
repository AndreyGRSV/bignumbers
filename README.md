# bignumbers

Trying to create template for simple usage of big numbers.

Usage

Just include several files:

```C++
#include <limits> 
#include <string>
#include "bdig.hpp"

// and set
using namespace sag;

int main()
{
  bdig <200, 200, unsigned long> digit = 1234567890;
  digit *= 100;
}
```

