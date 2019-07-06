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

