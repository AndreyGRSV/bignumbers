# bignumbers

Trying to create template for simple usage of big numbers.

Usage
Very simple, just include several files:

```C++
#include <limits> 
#include <string>
#include "bdig.hpp"

// and set
using namespace sag;

<code>
int main()
{
  bdig <200, 200, unsigned long> digit = 1234567890;
  digit *= 100;
}
```

