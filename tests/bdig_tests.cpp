#include "../bdig.hpp"
#include <vector>

#include <gtest/gtest.h>

// Test the comparison operators of digits
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
    EXPECT_EQ(value2 != check_value + 1, true);

    const char *max_value = "99999999999999999999999999999999999999999999999999";
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

    value1 = -1;
    value2 = 1;
    EXPECT_EQ(value2 == value1, false);
}

// Test the addition operators of digits
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

// Test the mul/div operators of digits
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

class CheckVariantData
{
    const char *szValue;
    const char *szParam;
    int iParam;
    const char *wantResult;

    const int iterations = 140;

    template <class T, class F>
    std::string callFunc(const T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T (T::*)(int, int) const>, F> = 0) const
    {
        return (num.*func)(iParam, iterations);
    }

    template <class T, class F>
    std::string callFunc(const T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T &(T::*)(int, bool)>, F> = 0) const
    {
        auto numtmp = num;
        return (numtmp.*func)(iParam, true);
    }

    template <class T, class F>
    std::string callFunc(const T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T (T::*)() const>, F> = 0) const
    {
        return (num.*func)();
    }

    template <class T, class F>
    std::string callFunc(const T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T (T::*)(const T &, int) const>, F> = 0) const
    {
        return (num.*func)(szValue, iterations);
    }

    template <class T, class F>
    std::string callFunc(const T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T (T::*)(const T &, const T &, int) const>, F> = 0) const
    {
        return (num.*func)(szValue, szParam, iterations);
    }

    template <class T, class F>
    std::string callFunc(const T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T (T::*)(const T &, const T &) const>, F> = 0) const
    {
        return (num.*func)(szValue, szParam);
    }

public:
    CheckVariantData(int param, const char *want) : szValue(""), iParam(param), wantResult(want) {}
    CheckVariantData(const char *param1, int param2, const char *want) : szValue(param1), iParam(param2), wantResult(want) {}
    CheckVariantData(const char *param1, const char *want) : szValue(param1), iParam(0), wantResult(want) {}
    CheckVariantData(const char *param1, const char *param2, const char *want) : szValue(param1), szParam(param2), iParam(0), wantResult(want) {}

    template <class T, class F>
    void Check(T num, const F func) const
    {
        // static_assert(std::is_same_v<F1, decltype(T1(T1::*)())>, "func1 is not a sapported. T1(T1::*)()");
        static_assert(std::is_member_function_pointer_v<F>, "T::F func is not a member function.");

        num = szValue;
        auto got = callFunc(num, func);

        // std::cout << szValue << ":\t" << iParam << " " << szValue << "\t" << got << "\t" << wantResult << std::endl;
        EXPECT_EQ(got, wantResult);
    }
};

auto checkAllValues = [](const std::vector<CheckVariantData> &vec, const auto num1, const auto func1, const auto num2, const auto func2)
{
    for (const auto &d : vec)
    {
        d.Check(num1, func1);
        d.Check(num2, func2);
    }
};

// Test the trigonometry functions
TEST(BDigTest, MathFunctionsTrigonometry)
{
    const int precision = 20;
    using bd_type1 = ::sag::bdig<1, precision>;
    using bd_type2 = ::sag::bdig<10000, precision>;
    bd_type1 value1;
    bd_type2 value2;

    value1 = -1;
    value2 = -1;
    EXPECT_EQ(value1.abs(), 1);
    EXPECT_EQ(value2.abs(), 1);
    value1 = 1;
    value2 = 1;
    EXPECT_EQ(value1.abs(), 1);
    EXPECT_EQ(value2.abs(), 1);

    const std::vector<CheckVariantData> checkMatrixCos{
        {0, "1.00000000000000000000"},    //  1,000000000000000
        {10, "0.98480775301220805937"},   //  0,984807753012208
        {20, "0.93969262078590838412"},   //  0,939692620785908
        {30, "0.86602540378443864690"},   //  0,866025403784439
        {45, "0.70710678118654752468"},   //  0,707106781186548
        {60, "0.50000000000000000047"},   //  0,500000000000000
        {90, "0.00000000000000000083"},   //  0,000000000000000
        {100, "-0.17364817766693034795"}, // -0,173648177666930
        {125, "-0.57357643635104609518"}, // -0,573576436351046
        {160, "-0.93969262078590838356"}, // -0,939692620785908
        {180, "-1.00000000000000000000"}, // -1,000000000000000
        {200, "-0.93969262078590838412"}, // -0,939692620785908
        {240, "-0.50000000000000000047"}, // -0,500000000000000
        {260, "-0.17364817766693034958"}, // -0,173648177666930
        {300, "0.49999999999999999902"},  //  0,500000000000000
        {310, "0.64278760968653932540"},  //  0,642787609686539
        {340, "0.93969262078590838356"},  //  0,939692620785908
        {360, "1.00000000000000000000"},  //  1,000000000000000
        {400, "0.76604444311897803544"},  //  0,766044443118978
        {540, "-1.00000000000000000000"}, // -1,000000000000000
        {1000, "0.17364817766693034795"}, //  0,173648177666931
        {-0, "1.00000000000000000000"},   //  1,000000000000000
        {-10, "0.98480775301220805937"},  //  0,984807753012208
        {-90, "0.00000000000000000083"}   //  0,000000000000000
    };

    checkAllValues(checkMatrixCos, value1, &bd_type1::cos, value2, &bd_type2::cos);

    const std::vector<CheckVariantData> checkMatrixSin{
        {0, "0.00000000000000000000"},     //  0,00000000000000
        {10, "0.17364817766693034795"},    //  0,17364817766693
        {20, "0.34202014332566873209"},    //  0,34202014332567
        {30, "0.49999999999999999902"},    //  0,50000000000000
        {45, "0.70710678118654752352"},    //  0,70710678118655
        {60, "0.86602540378443864608"},    //  0,86602540378444
        {90, "1.00000000000000000000"},    //  1,00000000000000
        {100, "0.98480775301220805937"},   //  0,98480775301221
        {125, "0.81915204428899178987"},   //  0,81915204428899
        {160, "0.34202014332566873366"},   //  0,34202014332567
        {180, "0.00000000000000000000"},   //  0,00000000000000
        {200, "-0.34202014332566873209"},  // -0,34202014332567
        {240, "-0.86602540378443864608"},  // -0,86602540378444
        {260, "-0.98480775301220805910"},  // -0,98480775301221
        {300, "-0.86602540378443864690"},  // -0,86602540378444
        {310, "-0.76604444311897803544"},  // -0,76604444311898
        {340, "-0.34202014332566873366"},  // -0,34202014332567
        {360, "0.00000000000000000000"},   //  0,00000000000000
        {400, "0.64278760968653932540"},   //  0,64278760968654
        {540, "0.00000000000000000000"},   //  0,00000000000000
        {1000, "-0.98480775301220805937"}, // -0,98480775301221
        {-0, "0.00000000000000000000"},    //  0,00000000000000
        {-10, "-0.17364817766693034958"},  // -0,17364817766693
        {-90, "-1.00000000000000000000"},  // -1,00000000000000
        {-540, "0.00000000000000000000"}   //  0,00000000000000
    };

    checkAllValues(checkMatrixSin, value1, &bd_type1::sin, value2, &bd_type2::sin);
}

// Test the math functions
TEST(BDigTest, MathFunctionsPowSqrt)
{
    const int precision = 20;
    using bd_type1 = ::sag::bdig<1, precision>;
    using bd_type2 = ::sag::bdig<10000, precision>;
    bd_type1 value1;
    bd_type2 value2;

    const std::vector<CheckVariantData> checkMatrixPow{
        {"0", 0, "1.00000000000000000000"},
        {"1", 0, "1.00000000000000000000"},
        {"2", 0, "1.00000000000000000000"},
        {"0.001", 0, "1.00000000000000000000"},
        {"0", 1, "0.00000000000000000000"},
        {"1", 1, "1.00000000000000000000"},
        {"2", 1, "2.00000000000000000000"},
        {"0.001", 1, "0.00100000000000000000"},
        {"0", 2, "0.00000000000000000000"},
        {"1", 2, "1.00000000000000000000"},
        {"2", 2, "4.00000000000000000000"},
        {"0.001", 2, "0.00000100000000000000"},
        {"10", 2, "100.00000000000000000000"},
        {"-1", 2, "1.00000000000000000000"},
        {"0", 3, "0.00000000000000000000"},
        {"1", 3, "1.00000000000000000000"},
        {"2", 3, "8.00000000000000000000"},
        {"0.001", 3, "0.00000000100000000000"},
        {"10", 3, "1000.00000000000000000000"},
        {"-1", 3, "-1.00000000000000000000"},
        {"0", -1, "0.00000000000000000000"},
        {"1", -1, "1.00000000000000000000"},
        {"2", -1, "0.50000000000000000000"},
        {"0.001", -1, "1000.00000000000000000000"},
        {"10", -1, "0.10000000000000000000"},
        {"-1", -1, "-1.00000000000000000000"},
        {"0", -2, "0.00000000000000000000"},
        {"1", -2, "1.00000000000000000000"},
        {"2", -2, "0.25000000000000000000"},
        {"0.001", -2, "1000000.00000000000000000000"},
        {"10", -2, "0.01000000000000000000"},
        {"-1", -2, "1.00000000000000000000"},
    };

    checkAllValues(checkMatrixPow, value1, &bd_type1::pow, value2, &bd_type2::pow);

    const std::vector<CheckVariantData> checkMatrixSqrt{
        {"0", "0.00000000000000000000"},
        {"1", "1.00000000000000000000"},
        {"2", "1.41421356237309504880"}, // 1,4142135623730950488016887242097
        {"4", "2.00000000000000000000"},
        {"25", "5.00000000000000000000"},
        {"0.04", "0.20000000000000000000"},
        {"0.0025", "0.05000000000000000000"},
        {"100", "10.00000000000000000000"},
        {"-1", "0.00000000000000000000"},
        {"-2", "0.00000000000000000000"}, // 1,4142135623730950488016887242097
        {"-4", "0.00000000000000000000"},
        {"-25", "0.00000000000000000000"},
        {"-0.04", "0.00000000000000000000"},
        {"-0.0025", "0.00000000000000000000"},
        {"-100", "0.00000000000000000000"},
    };

    checkAllValues(checkMatrixSqrt, value1, &bd_type1::sqrt, value2, &bd_type2::sqrt);
}

// Test the math functions
TEST(BDigTest, MathFunctionsLog)
{
    const int precision = 20;
    using bd_type1 = ::sag::bdig<1000, precision>;
    using bd_type2 = ::sag::bdig<10000, precision>;
    bd_type1 value1;
    bd_type2 value2;

    const std::vector<CheckVariantData> checkMatrixLog2{
        {"0", "0.00000000000000000000"},
        {"1", "0.00000000000000000000"},
        {"2", "1.00000000000000000000"},
        {"4", "2.00000000000000000000"},
        {"25", "4.64385618977472469560"},      //  4,64385618977472
        {"0.04", "-4.64385618977472469586"},   // -4,64385618977472
        {"0.0025", "-8.64385618977472469586"}, // -8,64385618977473
        {"100", "6.64385618977472469560"},     //  6,64385618977473
        {"-1", "0.00000000000000000000"},
        {"-2", "0.00000000000000000000"},
        {"8", "3.00000000000000000000"},
        {"16", "4.00000000000000000000"},
        {"32", "5.00000000000000000000"},
        {"64", "6.00000000000000000000"},
        {"128", "7.00000000000000000000"},
    };
    checkAllValues(checkMatrixLog2, value1, &bd_type1::log2, value2, &bd_type2::log2);

    const std::vector<CheckVariantData> checkMatrixLn{
        {"0", "0.00000000000000000000"},
        {"1", "0.00000000000000000000"},
        {"2", "0.69314718055994530949"},       // 0,69314718055995
        {"4", "1.38629436111989061899"},       // 1,38629436111989
        {"25", "3.21887582486820074948"},      //  3,21887582486820
        {"0.04", "-3.21887582486820074966"},   // -4,64385618977472
        {"0.0025", "-5.99146454710798198765"}, // -8,64385618977473
        {"100", "4.60517018598809136848"},     //  6,64385618977473
        {"-1", "0.00000000000000000000"},
        {"-2", "0.00000000000000000000"},
        {"8", "2.07944154167983592849"},   // 2,07944154167984
        {"16", "2.77258872223978123799"},  // 2,77258872223978
        {"32", "3.46573590279972654749"},  // 3,46573590279973
        {"64", "4.15888308335967185699"},  // 4,15888308335967
        {"128", "4.85203026391961716649"}, // 4,85203026391962
    };
    checkAllValues(checkMatrixLn, value1, &bd_type1::ln, value2, &bd_type2::ln);

    const std::vector<CheckVariantData> checkMatrixExp{
        {"0", "1.00000000000000000000"},
        {"1", "2.71828182845904523526"},                                                          // 2,71828182845905
        {"2", "7.38905609893065022713"},                                                          // 7,38905609893065
        {"4", "54.59815003314423907790"},                                                         // 54,59815003314420
        {"25", "72004899337.38587252416135146561"},                                               // 72004899337,38590000000000
        {"0.04", "1.04081077419238822671"},                                                       // 1,04081077419239
        {"0.0025", "1.00250312760579508495"},                                                     // 1,00250312760580
        {"100", "26878707852501517282815748377146333474410329.25858540110041213736"},             // 26881171418161400000000000000000000000000000,00000000000000
        {"-1", "0.36787944117144232160"},                                                         // 0,36787944117144
        {"-2", "0.13533528323661269189"},                                                         // 0,13533528323661
        {"8", "2980.95798704172827474335"},                                                       // 2980,95798704173000
        {"16", "8886110.52050787263676302336"},                                                   // 8886110,52050787000000
        {"32", "78962960182680.69516097802263510763"},                                            // 78962960182680,70000000000000
        {"64", "6235149080811615890313932116.50233310070025470907"},                              // 6235149080811620000000000000,00000000000000
        {"128", "32861299618848347303262577416218686705752852894060805148.62997146378348867557"}, // 38877084059946000000000000000000000000000000000000000000,00000000000000
    };
    checkAllValues(checkMatrixExp, value1, &bd_type1::exp, value2, &bd_type2::exp);

    const std::vector<CheckVariantData> checkMatrixExpPow{
        {"0", "0", "1.00000000000000000000"},
        {"-1", "0", "1.00000000000000000000"},
        {"-1", "0.5", "1.00000000000000000000"},
        {"2", "2", "4.00000000000000000046"},     // 4,00000000000000
        {"2", "2.5", "5.65685424949238019608"},   // 5,65685424949238
        {"4", "0.5", "2.00000000000000000006"},   // 2,00000000000000
        {"4", "0.25", "1.41421356237309504876"},  // 1,41421356237309
        {"4", "0.05", "1.07177346253629316416"},  // 1,07177346253629
        {"4", "-0.05", "0.93303299153680741598"}, // 0,93303299153681
    };
    checkAllValues(checkMatrixExpPow, value1, &bd_type1::powe, value2, &bd_type2::powe);
}

// Test for Karatsuba multiplication
TEST(BDigTest, MathFunctionsKaratsuba)
{
    const int precision = 0;
    using bd_type1 = ::sag::bdig<1000, precision>;
    using bd_type2 = ::sag::bdig<1000, precision>;
    bd_type1 value1;
    bd_type2 value2;

    const std::vector<CheckVariantData> checkMatrixKaratsuba{
        {"0", "0", "0"},
        {"1", "1", "1"},
        {"1", "2", "2"},
        {"2", "2", "4"},
        //{"-1", "0", "0"},
        {"-1", "-1", "1"},
        {"100", "100", "10000"},
        {"100000000000000000000000000000", "100000000000000000000000000000", "10000000000000000000000000000000000000000000000000000000000"},
        {"123456789012345678901234567890", "123456789012345678901234567890",
         "15241578753238836750495351562536198787501905199875019052100"}, // 1,5241578753238836750495351562536e+58
        {"123456789012345678901234567890123456789012345678901234567890", "123456789012345678901234567890123456789012345678901234567890",
         "15241578753238836750495351562566681945008382873376009755225087639153757049236500533455762536198787501905199875019052100"},
    };
    checkAllValues(checkMatrixKaratsuba, value1, &bd_type1::Karatsuba, value2, &bd_type2::Karatsuba);
}

TEST(BDigTest, MathFunctionsPrime)
{
    unsigned PrimeNumbers[] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
        53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
        127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197,
        199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
        283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379,
        383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
        467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571,
        577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
        661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761,
        769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
        877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977,
        983, 991, 997, 1009, 1013, 1019,
        1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153,
        1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297,
        1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453,
        1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597,
        1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741,
        1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901,
        1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063,
        2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221,
        2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371,
        2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539,
        2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689,
        2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833,
        2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011,
        3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 3191,
        3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359,
        3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533,
        3539, 3541, 3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671, 3673, 3677, 3691,
        3697, 3701, 3709, 3719, 3727, 3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923, 3929,
        3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057, 4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139, 4153, 4157, 4159, 4177,
        4201, 4211, 4217, 4219, 4229, 4231, 4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409, 4421, 4423, 4441, 4447, 4451, 4457, 4463,
        4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583, 4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729,
        4733, 4751, 4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937, 4943, 4951, 4957, 4967, 4969, 4973,
        4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087, 5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279, 5281, 5297,
        5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639, 5641};
    using bd_type = ::sag::bdig<10000, 0>;
    bd_type value;

    for (int i = 0; i < sizeof(PrimeNumbers) / sizeof(unsigned); ++i)
    {
        value = PrimeNumbers[i];
        // std::cout << PrimeNumbers[i] << std::endl;
        EXPECT_EQ(value.is_prime(), true);
        EXPECT_EQ(value.fermatest(), true);
    }

    unsigned NotPrimeNumbers[] = {10, 12, 60, 1000, 1200, 2245, 3302, 5000, 5001, 102345, 3456789};
    for (int i = 0; i < sizeof(NotPrimeNumbers) / sizeof(unsigned); ++i)
    {
        value = NotPrimeNumbers[i];
        // std::cout << NotPrimeNumbers[i] << std::endl;
        EXPECT_EQ(value.is_prime(), false);
        EXPECT_EQ(value.fermatest(), false);
    }

    // M_1 = 1        (4)
    // M_2 = 3        (5)
    // M_3 = 7        (6)
    // M_4 = 3*5      (7)
    // M_5 = 31       (8)
    // M_6 = 3*3*7    (9)
    // M_7 = 127      (10)
    // M_8 = 3*5*17   (11)
    // M_9 = 7*73     (12)
    // M_(10) = 3*11*31

    EXPECT_EQ(value.LucasLehmer(1), true);
    EXPECT_EQ(value.LucasLehmer(2), true);
    EXPECT_EQ(value.LucasLehmer(3), true);
    EXPECT_EQ(value.LucasLehmer(4), false);
    EXPECT_EQ(value.LucasLehmer(5), true);
    EXPECT_EQ(value.LucasLehmer(6), false);
    EXPECT_EQ(value.LucasLehmer(7), true);
    EXPECT_EQ(value.LucasLehmer(8), false);
    EXPECT_EQ(value.LucasLehmer(9), false);
    EXPECT_EQ(value.LucasLehmer(10), false);
}