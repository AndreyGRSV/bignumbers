#include "../bdig.hpp"

#include <gtest/gtest.h>

// Test comparison of digits
TEST(BDigTest, CompareDigits) {
  ::sag::bdig<50> value1 = 100;
  ::sag::bdig<50> value2 = 100;
  bool result = value1 == value2;
  EXPECT_EQ(result, true);
  EXPECT_EQ(value1.digits10, 50);
  EXPECT_EQ(value2.digits10, 50);

  EXPECT_EQ(value1 == "100", true);
  EXPECT_EQ(value1 == 100, true);
  EXPECT_EQ(value1 != 101, true);
  EXPECT_EQ(value2 == "100", true);
  EXPECT_EQ(value2 == 100, true);
  EXPECT_EQ(value2 != 101, true);

  const char* max_value = "99999999999999999999999999999999999999999999999999";
  value1 = max_value;
  result = value1 == max_value;
  EXPECT_EQ(value1 == max_value, true);

  value2 = max_value;
  result = value2 == max_value;
  EXPECT_EQ(value2 == max_value, true);
  EXPECT_EQ(value1 == value2, true);

  value1 = -1;
  EXPECT_EQ(value1 == -1, true);
  EXPECT_EQ(value1 == value2, false);
  EXPECT_EQ(value1 != value2, true);
  EXPECT_EQ(value1 < value2, true);
  EXPECT_EQ(value1 <= value2, true);
  EXPECT_EQ(value2 > value1, true);
  EXPECT_EQ(value2 >= value1, true);
  EXPECT_EQ(value2 != 0, true);
  EXPECT_EQ(value2 >= 0, true);

  value1 = value2;
  value1 -= 1;
  EXPECT_EQ(value2 >= value1, true);
}

// Test addition of digits
TEST(BDigTest, AdditionSubtractionDigits) {
  ::sag::bdig<> value1;
  ::sag::bdig<> value2;
  value1 = 1;
  value2 = 1;
  EXPECT_EQ(value1 + value2, 2);
  value1 += 1;
  EXPECT_EQ(value1, 2);
  value1 += -1;
  EXPECT_EQ(value1, 1);
  EXPECT_EQ(value1 - value2, 0);
  EXPECT_EQ(value1, 1);
  EXPECT_EQ(value2, 1);
  value1 -= 2;
  EXPECT_EQ(value1, -1);

  // ++value1;
  // EXPECT_EQ(value1, 3);

}

// Test addition of digits
TEST(BDigTest, MulDivDigits) {
  ::sag::bdig<> value1;
  ::sag::bdig<> value2;
  value1 = 1;
  value2 = 1;
  EXPECT_EQ(value1 * value2, 1);
  EXPECT_EQ(value1 * 10, 10);
  value1 *= 2;
  EXPECT_EQ(value1 * 10, 20);
  // EXPECT_EQ(value1 - value2, 0);
  // EXPECT_EQ(value1, 1);
  // EXPECT_EQ(value2, 1);
  // value1 -= 2;
  // EXPECT_EQ(value1, -1);

  // ++value1;
  // EXPECT_EQ(value1, 3);

}
