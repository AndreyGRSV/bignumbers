#include "../bdig.hpp"

#include <gtest/gtest.h>

// Test comparison of digits
TEST(BDigTest, CompareDigits)
{
    const auto number_digits = 50;
    const auto check_value = 100;
    using b_int = ::sag::bdig<number_digits>;
    b_int value1{check_value};
    b_int value2{check_value};
    bool result = value1 == value2;
    EXPECT_EQ(result, true);
    auto digits = b_int::digits10;
    EXPECT_EQ(digits, number_digits);

    EXPECT_EQ(value1 == "100", true);
    EXPECT_EQ(value1 == check_value, true);
    EXPECT_EQ(value1 != check_value + 1, true);
    EXPECT_EQ(value2 == "100", true);
    EXPECT_EQ(value2 == check_value, true);
    EXPECT_EQ(value2 != check_value+1, true);

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
TEST(BDigTest, AdditionSubtractionDigits)
{
    ::sag::bdig<1> value1;
    ::sag::bdig<1> value2;
    const auto max_value = 0xFF;
    value1 = max_value;
    value2 = max_value;
    EXPECT_EQ(value1 + value2, max_value - 1);
    value1 += 1;
    EXPECT_EQ(value1, 0);
    value1 += -1;
    EXPECT_EQ(value1, -1);
    value1 = value1 - value2;
    EXPECT_EQ(value1, 0);
    EXPECT_EQ(max_value - value1, max_value);
    EXPECT_EQ(value2 - max_value, 0);
    value1 -= -1;
    EXPECT_EQ(value1, 1);
    value1 -= 1;
    EXPECT_EQ(value1, 0);

    value1 = max_value;
    value2 = max_value;
    value2 = value1 - max_value;
    EXPECT_EQ(value2, 0);

    value2 = max_value - max_value;
    EXPECT_EQ(value2, 0);

    value2 = max_value - value1;
    EXPECT_EQ(value2, 0);

}

// Test mul/div of digits
TEST(BDigTest, MulDivDigits)
{
    ::sag::bdig<1> value1;
    ::sag::bdig<1> value2;

    // Normal positive values
    value1 = 1;
    value2 = 1;
    EXPECT_EQ(value1 * value2, 1);
    EXPECT_EQ(value1 * 10, 10);
    value1 *= 2;
    EXPECT_EQ(value1 * 10, 20);

    value1 = 1;
    value2 = 1;
    EXPECT_EQ(value1 / value2, 1);
    EXPECT_EQ(10 / value1, 10);
    value1 *= 2;
    EXPECT_EQ(20 / value1, 10);

    // Normal negative values
    value1 = -1;
    value2 = -1;
    EXPECT_EQ(value1 * value2, 1);
    EXPECT_EQ(value1 * -10, 10);
    EXPECT_EQ(value1 * 10, -10);
    EXPECT_EQ(value1 * -100, 100);
    EXPECT_EQ(value1 * 100, -100);

    // Manipulation with zero

    value1 = 0;
    value2 = 0;
    EXPECT_EQ(value1 * value2, 0);
    value1 = 1;
    EXPECT_EQ(value1 * 0, 0);
    EXPECT_EQ(0 * value1, 0);
    // Dividing by zero is not trigger exception for working
    // in embedded systems without stopping.
    // Value of dividend is returned.
    value1 = value1 / value2;
    EXPECT_EQ(value1 / value2, value1);
    EXPECT_EQ(value1 / 0, value1);
    EXPECT_EQ(value2 / 0, 0);
    EXPECT_EQ(0 / value1, 0);
}
