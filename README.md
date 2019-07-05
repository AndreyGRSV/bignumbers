# bignumbers

Trying to create template for simple usage of big numbers.

Usage
Very simple, just include several files:
<br>
#include \<limits\> 
<br>
#include \<string\>
<br>
#include \"bdig.hpp\"

// and set
<br>
using namespace sag;

<code>
int main()
{
  bdig <200, 200, unsigned long> digit = 1234567890;
  digit *= 100;
}
</code>


