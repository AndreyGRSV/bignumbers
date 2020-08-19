#include "../bdig.hpp"
#include <iostream>

int main() {
  ::sag::bdig<20, 100> value1 = 200, value2 = 200, result;
  result = value1 + value2;
  ::std::cout << ::std::string(result) << ::std::endl;
  result -= "0.1111111111";
  ::std::cout << ::std::string(result) << ::std::endl;
  result += "0.1111111111";
  ::std::cout << ::std::string(result) << ::std::endl;
  result -= "0.00000000000000000000000000000000000000000000000001";
  ::std::cout << ::std::string(result) << ::std::endl;
  result += "0.00000000000000000000000000000000000000000000000001";
  ::std::cout << ::std::string(result) << ::std::endl;
  result += 555555555;
  ::std::cout << ::std::string(result) << ::std::endl;
  result -= 555555555;
  ::std::cout << ::std::string(result) << ::std::endl;
  result = 100 + value1 - 300 - value2;
  ::std::cout << ::std::string(result) << ::std::endl;
  result -= 200;
  ::std::cout << ::std::string(result) << ::std::endl;
  result -= -200;
  ::std::cout << ::std::string(result) << ::std::endl;
  result += 200;
  ::std::cout << ::std::string(result) << ::std::endl;
  result += -200;
  ::std::cout << ::std::string(result) << ::std::endl;
}