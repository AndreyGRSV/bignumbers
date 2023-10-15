#include "../bdig.hpp"
#include <vector>
#include <string>
#include <chrono>
#include <type_traits>
#include <thread>

#include <gtest/gtest.h>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

template<typename T>
std::string rttn() {
#ifdef __GNUC__
    int status = 0;
    char* pname = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);
    std::string real_name = pname;
    std::free(pname);
    return real_name;
#else
    return typeid(T).name();
#endif
}


template<class T, int digits = 50, int prec = 0>
void TestAssignmentAndConversion() {
     using bint_t = ::sag::bdig<digits, prec, T>;
     int ivalue = 10000;
     bint_t value = ivalue;
     EXPECT_EQ(value, ivalue);

     EXPECT_EQ(sag::is_bdig<bint_t>::value == true, true);
     EXPECT_EQ(sag::is_bdig<::sag::bdig<100>>::value == true, true);
     EXPECT_EQ((sag::is_bdig<::sag::bdig<100,100>>::value == true), true);
     EXPECT_EQ((sag::is_bdig<::sag::bdig<100,100,T>>::value == true), true);
     EXPECT_EQ(sag::is_bdig<char>::value == true, false);
     EXPECT_EQ(sag::is_bdig<int>::value == true, false);
     EXPECT_EQ(sag::is_bdig<unsigned>::value == true, false);


     std::string svalue = "99999999999999999999";
     value = svalue;
     EXPECT_EQ((std::string)value, svalue);

     std::string svalue_plus = "+99999999999999999999";
     value = svalue_plus;
     EXPECT_EQ((std::string)value, svalue);

     std::string svalue_minus = "-99999999999999999999";
     value = svalue_minus;
     EXPECT_EQ((std::string)value, svalue_minus);

     value = -value;
     EXPECT_EQ((std::string)value, svalue);

     value = value - value;
     EXPECT_EQ(value, 0);

    if constexpr (prec >= 10) {
        std::string svalue2 = "99999999999999999999.99999";
        value = svalue2;
        EXPECT_EQ((std::string)value, svalue2);

        svalue2 = "-99999999999999999999.000";
        value = svalue2;
        EXPECT_EQ((std::string)value, svalue_minus);
    }

     std::string svalue_hex = "H 01 A0 F0 E0";
     value = svalue_hex;
     ivalue = 27324640;
     std::string check_value = value;
     EXPECT_EQ(value, ivalue);

     EXPECT_EQ(value.hex(), "1a0f0e0");

     const char carr[] = "99999999999999999999";
     value = carr;
     const unsigned char ucarr[] = "99999999999999999999";
     value = ucarr;
     const signed char scarr[] = "99999999999999999999";
     value = scarr;
     const char* pc = "99999999999999999999";
     value = pc;
     const unsigned char* puc = ucarr;
     value = puc;
     const signed char* psc = scarr;
     value = psc;
     const int arr_len = 30;
     unsigned char uc_arr[arr_len];
     std::strncpy(reinterpret_cast<char*>(uc_arr), carr, arr_len);
     value = uc_arr;
     signed char sc_arr[arr_len];
     std::strncpy(reinterpret_cast<char*>(sc_arr), carr, arr_len);
     value = sc_arr;

     std::stringstream ss;
     ss << value;
     EXPECT_EQ(ss.str(), carr);
     value = svalue_hex;
     ss.str("");
     ss << std::hex << std::uppercase << value;
     EXPECT_EQ(ss.str(), "1A0F0E0");
}

// Test assignment to and conversion from digit.
TEST(BDigTest, AssignmentAndConversion)
{
    const int digits = 50;
    const int prec = 10;
    TestAssignmentAndConversion<unsigned char>();
    TestAssignmentAndConversion<unsigned short>();
    TestAssignmentAndConversion<unsigned int>();
#ifdef __SIZEOF_INT128__
    TestAssignmentAndConversion<unsigned long long>();
#endif

    TestAssignmentAndConversion<unsigned char, digits, prec>();
    TestAssignmentAndConversion<unsigned short, digits, prec>();
    TestAssignmentAndConversion<unsigned int, digits, prec>();
#ifdef __SIZEOF_INT128__
    TestAssignmentAndConversion<unsigned long long, digits, prec>();
#endif
}


template<class T, int digits = 50>
void TestComparision() {
    const auto number_digits = digits;
    const auto check_value = 100;
    using b_int = ::sag::bdig<number_digits, 0, T>;
    b_int value1{ check_value };
    b_int value2{ check_value };

    bool result = value1 == value2;
    EXPECT_EQ(result, true);
    auto digits10 = b_int::digits10;
    EXPECT_EQ(digits10, number_digits);

    EXPECT_EQ(value1 == "100", true);
    EXPECT_EQ(value1 == check_value, true);
    EXPECT_EQ(value1 != check_value + 1, true);
    EXPECT_EQ(value2 == "100", true);
    EXPECT_EQ(value2 == check_value, true);
    EXPECT_EQ(value2 != check_value + 1, true);

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

    value1 = -1;
    value2 = 1;
    EXPECT_EQ(value2 == value1, false);
}

// Test the comparison operators of digits
TEST(BDigTest, CompareDigits)
{
    TestComparision<unsigned char>();
    TestComparision<unsigned short>();
    TestComparision<unsigned int>();
#ifdef __SIZEOF_INT128__
    TestComparision<unsigned long long>();
#endif
}

template<class T, int digits = 1, int precision = 0>
void TestAdditionSubtractionDigits() {
    using bdig_t = ::sag::bdig<digits, 0, T>;
    using bdigp10_t = ::sag::bdig<digits, 10, T>;
    bdig_t value1;
    bdig_t value2;
    bdigp10_t valdp1 = "1.1234567890";

    valdp1 += valdp1;
    std::string s = valdp1;
    EXPECT_EQ(valdp1, "2.2469135780");
    valdp1 -= "1.1234567890";
    EXPECT_EQ(valdp1, "1.1234567890");

    const auto max_value = std::numeric_limits<T>::max();
    value1 = max_value;
    value2 = max_value;
    bdig_t v = value1 + value2;
    EXPECT_EQ(v, std::numeric_limits<T>::max() - 1);
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

    value1 = -1;
    value1 = -value1;
    EXPECT_EQ(value1, 1);
    value1 = -value1;
    EXPECT_EQ(value1, -1);

    value1 = 0;
    value1 = -value1;
    value1 -= 1;
    EXPECT_EQ(value1, -1);
}

// Test the addition operators of digits
TEST(BDigTest, AdditionSubtractionDigits)
{
    TestAdditionSubtractionDigits<unsigned char>();
    TestAdditionSubtractionDigits<unsigned short>();
    TestAdditionSubtractionDigits<unsigned int>();
#ifdef __SIZEOF_INT128__
    TestAdditionSubtractionDigits<unsigned long long>();
#endif
}

template<class T, int digits = 1, int precision = 0>
void TestMulDivDigits() {
    using bdig_t = ::sag::bdig<digits, precision, T>;
    bdig_t value1;
    bdig_t value2;

    // Normal positive values
    value1 = 1;
    value2 = 1;
    std::string s = value1;
    value1 = value1 * value2;
    s = value1;
    EXPECT_EQ(value1, 1);
    EXPECT_EQ(value1 * 10, 10);
    value1 *= 2;
    EXPECT_EQ(value1 * 10, 20);

    value1 = 1;
    value2 = 1;
    EXPECT_EQ(value1 / value2, 1);
    EXPECT_EQ(10 / value1, 10);
    value1 *= 2;
    EXPECT_EQ(20 / value1, 10);
    if (value2.digits10 > 30) {
        //           1   2   3   4   5   6   7   8   9 = 27
        value2 = "1'000'000'000'000'000'000'000'000'000";
        value1 = "1'000'000'000'000'000'000'000'000'000";
        s = value1;
        EXPECT_EQ(value1 / value2, 1);
        EXPECT_EQ(value1 / "1'000'000'000'000'000'000'000", "1'000'000");
        s = value1 / "1'000'000'000'000'000'000'000";
        EXPECT_EQ(value1 / 10,   "100'000'000'000'000'000'000'000'000");
        s = value1 / 10;
        EXPECT_EQ(value1 / 100,  "10'000'000'000'000'000'000'000'000");
        s = value1 / 100;
        EXPECT_EQ(value1 / 1000, "1'000'000'000'000'000'000'000'000");
        s = value1 / 1000;
        EXPECT_EQ(value1 / 10000, "100'000'000'000'000'000'000'000");
        s = value1 / 10000;
        EXPECT_EQ(value1 / 100000, "10'000'000'000'000'000'000'000");
        s = value1 / 100000;
        EXPECT_EQ(value1 / 2, "500'000'000'000'000'000'000'000'000");
        s = value1 / 2;
        EXPECT_EQ(value1 / 4, "250'000'000'000'000'000'000'000'000");
        s = value1 / 4;
    }

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

    // Remainder
    value1 = 16;
    value2 = 13;
    EXPECT_EQ(value1 % value2, 3);
}

// Test the mul/div operators of digits
TEST(BDigTest, MulDivDigits)
{
    TestMulDivDigits<unsigned char, 1>();
    TestMulDivDigits<unsigned char, 100>();
    TestMulDivDigits<unsigned char, 5, 17>();
    TestMulDivDigits<unsigned short, 1>();
    TestMulDivDigits<unsigned short, 100>();
    TestMulDivDigits<unsigned short, 100, 100>();
    TestMulDivDigits<unsigned int, 1>();
    TestMulDivDigits<unsigned int, 100>();
    TestMulDivDigits<unsigned int, 100, 100>();
#ifdef __SIZEOF_INT128__
    TestMulDivDigits<unsigned long long, 1>();
    TestMulDivDigits<unsigned long long, 100>();
    TestMulDivDigits<unsigned long long, 100, 100>();
#endif
}

struct CheckVariantData
{
    const char *szValue = nullptr;
    const char *szParam = nullptr;
    int iParam;
    const char *wantResult = nullptr;

    const int iterations = 140;

    template <class T, class F>
    std::string callFunc(const T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T (T::*)(int) const>, F> = 0) const
    {
        return (num.*func)(iterations);
    }

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
        return (num.*func)(szParam, iterations);
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

    template <class T, class F>
    std::string callFunc(T num, const F func,
                         typename std::enable_if_t<std::is_same_v<F, T& (T::*)(int)>, F> = 0) const
    {
        return (num.*func)(iParam);
    }
public:
    CheckVariantData(int param, const char *want) : szValue(""), iParam(param), wantResult(want) {}
    CheckVariantData(const char *param1, int param2, const char *want) : szValue(param1), iParam(param2), wantResult(want) {}
    CheckVariantData(const char *param1, const char *want) : szValue(param1), iParam(0), wantResult(want) {}
    CheckVariantData(const char *param1, const char *param2, const char *want) : szValue(param1), szParam(param2), iParam(0), wantResult(want) {}

    template <class T, class F>
    void Check(T num, const F func, const std::string_view& info) const
    {
        // static_assert(std::is_same_v<F1, decltype(T1(T1::*)())>, "func1 is not a sapported. T1(T1::*)()");
        static_assert(std::is_member_function_pointer_v<F>, "T::F func is not a member function.");

        num = szValue;
        auto got = callFunc(num, func);

        EXPECT_EQ(got, wantResult) << info << ", szValue:" << (szValue ? szValue : "") << ", szParam:" << (szParam ? szParam : "") << ", iParam:" << iParam;
    }
};

auto checkAllValues = [](const std::vector<CheckVariantData> &vec, const auto num, const auto func, const std::string_view comment = "")
{
    for (const auto &d : vec) {
        d.Check(num, func, comment);
    }
};

template<class T, int digits = 1, int precision = 20>
void TestMathFunctionsTrigonometry() {
    using bd_type1 = ::sag::bdig<digits, precision, T>;
    bd_type1 value1;

    value1 = -1;
    EXPECT_EQ(value1.abs(), 1);
    value1 = 1;
    EXPECT_EQ(value1.abs(), 1);

    value1.cos(300);
    std::string s = value1;

    const std::vector<CheckVariantData> checkMatrixCos{
        {0, "1"},    //  1,000000000000000
        { 10, "0.98480775301220805937" },   //  0,984807753012208
        { 20, "0.93969262078590838412" },   //  0,939692620785908
        { 30, "0.8660254037844386469" },   //  0,866025403784439
        { 45, "0.70710678118654752468" },   //  0,707106781186548
        { 60, "0.50000000000000000047" },   //  0,500000000000000
        { 90, "0.00000000000000000083" },   //  0,000000000000000
        { 100, "-0.17364817766693034795" }, // -0,173648177666930
        { 125, "-0.57357643635104609518" }, // -0,573576436351046
        { 160, "-0.93969262078590838356" }, // -0,939692620785908
        { 180, "-1" }, // -1,000000000000000
        { 200, "-0.93969262078590838412" }, // -0,939692620785908
        { 240, "-0.50000000000000000047" }, // -0,500000000000000
        { 260, "-0.17364817766693034958" }, // -0,173648177666930
        { 300, "0.49999999999999999902" },  //  0,500000000000000
        { 310, "0.6427876096865393254" },  //  0,642787609686539
        { 340, "0.93969262078590838356" },  //  0,939692620785908
        { 360, "1" },  //  1,000000000000000
        { 400, "0.76604444311897803544" },  //  0,766044443118978
        { 540, "-1" }, // -1,000000000000000
        { 1000, "0.17364817766693034795" }, //  0,173648177666931
        { -0, "1" },   //  1,000000000000000
        { -10, "0.98480775301220805937" },  //  0,984807753012208
        { -90, "0.00000000000000000083" }   //  0,000000000000000
    };

    checkAllValues(checkMatrixCos, value1, static_cast<bd_type1& (bd_type1::*)(int)>(&bd_type1::cos), "bdig::cos(). Limb type: " + rttn<T>());

    const std::vector<CheckVariantData> checkMatrixSin{
        {0, "0"},     //  0,00000000000000
        { 10, "0.17364817766693034795" },    //  0,17364817766693
        { 20, "0.34202014332566873209" },    //  0,34202014332567
        { 30, "0.49999999999999999902" },    //  0,50000000000000
        { 45, "0.70710678118654752352" },    //  0,70710678118655
        { 60, "0.86602540378443864608" },    //  0,86602540378444
        { 90, "1" },    //  1,00000000000000
        { 100, "0.98480775301220805937" },   //  0,98480775301221
        { 125, "0.81915204428899178987" },   //  0,81915204428899
        { 160, "0.34202014332566873366" },   //  0,34202014332567
        { 180, "0" },   //  0,00000000000000
        { 200, "-0.34202014332566873209" },  // -0,34202014332567
        { 240, "-0.86602540378443864608" },  // -0,86602540378444
        { 260, "-0.9848077530122080591" },  // -0,98480775301221
        { 300, "-0.8660254037844386469" },  // -0,86602540378444
        { 310, "-0.76604444311897803544" },  // -0,76604444311898
        { 340, "-0.34202014332566873366" },  // -0,34202014332567
        { 360, "0" },   //  0,00000000000000
        { 400, "0.6427876096865393254" },   //  0,64278760968654
        { 540, "0" },   //  0,00000000000000
        { 1000, "-0.98480775301220805937" }, // -0,98480775301221
        { -0, "0" },    //  0,00000000000000
        { -10, "-0.17364817766693034958" },  // -0,17364817766693
        { -90, "-1" },  // -1,00000000000000
        { -540, "0" }   //  0,00000000000000
    };

    checkAllValues(checkMatrixSin, value1, static_cast<bd_type1& (bd_type1::*)(int)>(&bd_type1::sin), "bdig::sin(). Limb type: " + rttn<T>());
}

// Test the trigonometry functions
TEST(BDigTest, MathFunctionsTrigonometry)
{
    //TestMathFunctionsTrigonometry<unsigned char, 300>();
    //TestMathFunctionsTrigonometry<unsigned char, 10000>();
    TestMathFunctionsTrigonometry<unsigned short, 300>();
    TestMathFunctionsTrigonometry<unsigned short, 10000>();
    TestMathFunctionsTrigonometry<unsigned int, 300>();
    TestMathFunctionsTrigonometry<unsigned int, 10000>();
#ifdef __SIZEOF_INT128__
    TestMathFunctionsTrigonometry<unsigned long long, 300>();
    TestMathFunctionsTrigonometry<unsigned long long, 10000>();
#endif
}

template<class T, int digits = 1, int precision = 20>
void TestMathFunctionsPowSqrt() {
    using bd_type1 = ::sag::bdig<digits, precision, T>;
    bd_type1 value1;

    const std::vector<CheckVariantData> checkMatrixPow{
        {"0", 0, "1"},
        { "1", 0, "1" },
        { "2", 0, "1" },
        { "0.001", 0, "1" },
        { "0", 1, "0" },
        { "1", 1, "1" },
        { "2", 1, "2" },
        { "0.001", 1, "0.001" },
        { "0", 2, "0" },
        { "1", 2, "1" },
        { "2", 2, "4" },
        { "0.001", 2, "0.000001" },
        { "10", 2, "100" },
        { "-1", 2, "1" },
        { "0", 3, "0" },
        { "1", 3, "1" },
        { "2", 3, "8" },
        { "0.001", 3, "0.000000001" },
        { "10", 3, "1000" },
        { "-1", 3, "-1" },
        { "0", -1, "0" },
        { "1", -1, "1" },
        { "2", -1, "0.5" },
        { "0.001", -1, "1000" },
        { "10", -1, "0.1" },
        { "-1", -1, "-1" },
        { "0", -2, "0" },
        { "1", -2, "1" },
        { "2", -2, "0.25" },
        { "0.001", -2, "1000000" },
        { "10", -2, "0.01" },
        { "-1", -2, "1" },
    };

    checkAllValues(checkMatrixPow, value1, &bd_type1::pow, "bdig::pow(). Limb type: " + rttn<T>());

    const std::vector<CheckVariantData> checkMatrixSqrt{
        {"0", "0"},
        { "1", "1" },
        { "2", "1.4142135623730950488" }, // 1,4142135623730950488016887242097
        { "4", "2" },
        { "25", "5" },
        { "0.04", "0.2" },
        { "0.0025", "0.05" },
        { "100", "10" },
        { "-1", "0" },
        { "-2", "0" }, // 1,4142135623730950488016887242097
        { "-4", "0" },
        { "-25", "0" },
        { "-0.04", "0" },
        { "-0.0025", "0" },
        { "-100", "0" },
    };

    checkAllValues(checkMatrixSqrt, value1, &bd_type1::sqrt, "bdig::sqrt(). Limb type: " + rttn<T>());
}
// Test the math functions
TEST(BDigTest, MathFunctionsPowSqrt)
{
    TestMathFunctionsPowSqrt<unsigned char, 300>();
    TestMathFunctionsPowSqrt<unsigned char, 10000>();
    TestMathFunctionsPowSqrt<unsigned short, 300>();
    TestMathFunctionsPowSqrt<unsigned short, 10000>();
    TestMathFunctionsPowSqrt<unsigned int, 300>();
    TestMathFunctionsPowSqrt<unsigned int, 10000>();
#ifdef __SIZEOF_INT128__
    TestMathFunctionsPowSqrt<unsigned long long, 300>();
    TestMathFunctionsPowSqrt<unsigned long long, 10000>();
#endif
}

template<class T, int digits = 300, int precision = 20>
void TestMathFunctionsLog() {
    using bd_type1 = ::sag::bdig<digits, precision, T>;
    bd_type1 value1;

    const std::vector<CheckVariantData> checkMatrixLog2{
        {"0", "0"},
        { "1", "0" },
        { "2", "1" },
        { "4", "2" },
        { "25", "4.6438561897747246956" },      //  4,64385618977472
        { "0.04", "-4.64385618977472469586" },   // -4,64385618977472
        { "0.0025", "-8.64385618977472469586" }, // -8,64385618977473
        { "100", "6.6438561897747246956" },     //  6,64385618977473
        { "-1", "0" },
        { "-2", "0" },
        { "8", "3" },
        { "16", "4" },
        { "32", "5" },
        { "64", "6" },
        { "128", "7" },
    };
    checkAllValues(checkMatrixLog2, value1, static_cast<bd_type1 (bd_type1::*)(const int) const>(&bd_type1::log2), "bdig::log2()");

    const std::vector<CheckVariantData> checkMatrixLn{
        {"0", "0"},
        { "1", "0" },
        { "2", "0.69314718055994530949" },       // 0,69314718055995
        { "4", "1.38629436111989061899" },       // 1,38629436111989
        { "25", "3.21887582486820074948" },      //  3,21887582486820
        { "0.04", "-3.21887582486820074966" },   // -4,64385618977472
        { "0.0025", "-5.99146454710798198765" }, // -8,64385618977473
        { "100", "4.60517018598809136848" },     //  6,64385618977473
        { "-1", "0" },
        { "-2", "0" },
        { "8", "2.07944154167983592849" },   // 2,07944154167984
        { "16", "2.77258872223978123799" },  // 2,77258872223978
        { "32", "3.46573590279972654749" },  // 3,46573590279973
        { "64", "4.15888308335967185699" },  // 4,15888308335967
        { "128", "4.85203026391961716649" }, // 4,85203026391962
    };
    checkAllValues(checkMatrixLn, value1, static_cast<bd_type1 (bd_type1::*)(const int) const>(&bd_type1::ln), "bdig::ln()");

    const std::vector<CheckVariantData> checkMatrixExp{
        {"0", "1"},
        { "1", "2.71828182845904523526" },                                                          // 2,71828182845905
        { "2", "7.38905609893065022713" },                                                          // 7,38905609893065
        { "4", "54.5981500331442390779" },                                                         // 54,59815003314420
        { "25", "72004899337.38587252416135146561" },                                               // 72004899337,38590000000000
        { "0.04", "1.04081077419238822671" },                                                       // 1,04081077419239
        { "0.0025", "1.00250312760579508495" },                                                     // 1,00250312760580
        { "100", "26878707852501517282815748377146333474410329.25858540110041213736" },             // 26881171418161400000000000000000000000000000,00000000000000
        { "-1", "0.3678794411714423216" },                                                         // 0,36787944117144
        { "-2", "0.13533528323661269189" },                                                         // 0,13533528323661
        { "8", "2980.95798704172827474335" },                                                       // 2980,95798704173000
        { "16", "8886110.52050787263676302336" },                                                   // 8886110,52050787000000
        { "32", "78962960182680.69516097802263510763" },                                            // 78962960182680,70000000000000
        { "64", "6235149080811615890313932116.50233310070025470907" },                              // 6235149080811620000000000000,00000000000000
        { "128", "32861299618848347303262577416218686705752852894060805148.62997146378348867557" }, // 38877084059946000000000000000000000000000000000000000000,00000000000000
    };

    checkAllValues(checkMatrixExp, value1, static_cast<bd_type1 (bd_type1::*)(const int) const>(&bd_type1::exp), "bdig::exp() limb type:" + rttn<T>());

    const std::vector<CheckVariantData> checkMatrixExpPow{
        {"0", "0", "1"},
        { "-1", "0", "1" },
        { "-1", "0.5", "1" },
        { "2", "2", "4.00000000000000000046" },     // 4,00000000000000
        { "2", "2.5", "5.65685424949238019608" },   // 5,65685424949238/
        { "4", "0.5", "2.00000000000000000006" },   // 2,00000000000000
        { "4", "0.25", "1.41421356237309504876" },  // 1,41421356237309
        { "4", "0.05", "1.07177346253629316416" },  // 1,07177346253629
        { "4", "-0.05", "0.93303299153680741598" }, // 0,93303299153681
    };
    bd_type1 val = "2";
    std::string ss = val.powe("2.5");
    checkAllValues(checkMatrixExpPow, value1, static_cast<bd_type1 (bd_type1::*)(const bd_type1&, const int) const>(&bd_type1::powe), std::string("bdig::powe() limb type:") + rttn<T>());
}

// Test the math functions
TEST(BDigTest, MathFunctionsLog)
{
    // TestMathFunctionsLog<unsigned char, 300>();
    // TestMathFunctionsLog<unsigned char, 10000>();
    TestMathFunctionsLog<unsigned short, 300>();
    TestMathFunctionsLog<unsigned short, 10000>();
    TestMathFunctionsLog<unsigned int, 300>();
    TestMathFunctionsLog<unsigned int, 10000>();
#ifdef __SIZEOF_INT128__
    TestMathFunctionsLog<unsigned long long, 300>();
    TestMathFunctionsLog<unsigned long long, 10000>();
#endif
}

template<class T, int digits = 300, int precision = 0>
void TestMathFunctionsPrime() {
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
        5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639, 5641 };
    using bd_type = ::sag::bdig<digits, precision, T>;
    bd_type value;

    for (std::size_t i = 0; i < sizeof(PrimeNumbers) / sizeof(unsigned); ++i)
    {
        value = PrimeNumbers[i];
        // std::cout << PrimeNumbers[i] << std::endl;
        EXPECT_EQ(value.is_prime(), true);
        EXPECT_EQ(value.fermatest(), true);
    }

    unsigned NotPrimeNumbers[] = { 10, 12, 60, 1000, 1200, 2245, 3302, 5000, 5001, 102345, 3456789 };
    for (std::size_t i = 0; i < sizeof(NotPrimeNumbers) / sizeof(unsigned); ++i)
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

// Test for Prime check functions
TEST(BDigTest, TestMathFunctionsPrime)
{
    TestMathFunctionsPrime<unsigned char, 100>();
    TestMathFunctionsPrime<unsigned char, 1000>();
    TestMathFunctionsPrime<unsigned short, 100>();
    TestMathFunctionsPrime<unsigned short, 1000>();
    TestMathFunctionsPrime<unsigned int, 100>();
    TestMathFunctionsPrime<unsigned int, 1000>();
#ifdef __SIZEOF_INT128__
    TestMathFunctionsPrime<unsigned long long, 100>();
    TestMathFunctionsPrime<unsigned long long, 1000>();

#endif
}

#ifdef BUILD_PERFORMANCE_INFO

#include <future>

enum class TestOpType{ Mul, Div, Add, Sub};

template<class T, int digits, TestOpType type = TestOpType::Mul>
void PerformanceTest() {

        using namespace std::chrono_literals;
        std::string str;
        if (digits > 100)
            str = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
        else
            str = "1234567890";
        std::string str_value1;
        std::string str_value2;
        std::size_t fill_digits = digits - 1;
        if constexpr (type == TestOpType::Mul)
            fill_digits /= 3;

        while (str_value2.length() < fill_digits)
            str_value2 += str;
        while (str_value1.length() < static_cast<std::size_t>(digits - 1))
            str_value1 += str;

        if constexpr (type == TestOpType::Div)
            str_value2 = "1234567890";
        //if constexpr (type == TestOpType::Mul) {
        //    str_value2 = "98109840984098409156481068456541684065964819841065106865710397464513210416435401645030648036034063974065004951094209420942097421970490274195049120974210974209742190274092740492097420929892490974202241";
        //    str_value1 = "";
        //    for (int i = 0; i < 10; i++)
        //        str_value1 += str_value2;
        //}

        auto getOpText = [] {
                if constexpr (type == TestOpType::Mul)
                    return "Multiplication";
                if constexpr (type == TestOpType::Div)
                    return "Division";
                if constexpr (type == TestOpType::Add)
                    return "Addition";
                if constexpr (type == TestOpType::Sub)
                    return "Subtraction";

                static_assert("Wrong performance operation type");
        };
        std::cout << getOpText() << " max digits:" << digits << " op1:"<< str_value1.length()  << " op2:" << str_value2.length()  <<" Buffer element bits: " << std::numeric_limits<T>::digits; // << std::endl;

        using bigd_t = ::sag::bdig<digits, 0, T>;
        bigd_t vbig = str_value1, vb_res, vbig2 = str_value2;

        std::atomic_bool stop = false;
        std::atomic_int64_t counter = 0;

        std::jthread th([&]() mutable {
            for (;;) {
                if constexpr (type == TestOpType::Mul)
                    vb_res = vbig * vbig2;
                if constexpr (type == TestOpType::Div)
                    vb_res = vbig / vbig2;
                if constexpr (type == TestOpType::Add)
                    vb_res = vbig + vbig2;
                if constexpr (type == TestOpType::Sub)
                    vb_res = vbig - vbig2;
                if (stop) {
                    break;
                }
                counter++;
            }
         });
        std::this_thread::sleep_for(1s);
        stop = true;
        std::cout << "\t" << std::right << std::setw(25) << counter << " operations in second" << std::endl;

        if (th.joinable())
            th.join();

}

// Test for Prime check functions
TEST(BDigTest, Performance) {

    const int DigitsShort = 200;
    const int DigitsMedium = 2'200;
    const int DigitsLong = 20'000;


    PerformanceTest<unsigned char, DigitsShort>();
    PerformanceTest<unsigned char, DigitsMedium>();
    PerformanceTest<unsigned char, DigitsLong>();
    PerformanceTest<unsigned short, DigitsShort>();
    PerformanceTest<unsigned short, DigitsMedium>();
    PerformanceTest<unsigned short, DigitsLong>();
    PerformanceTest<unsigned int, DigitsShort>();
    PerformanceTest<unsigned int, DigitsMedium>();
    PerformanceTest<unsigned int, DigitsLong>();
#ifdef __SIZEOF_INT128__
    PerformanceTest<unsigned long long, DigitsShort>();
    PerformanceTest<unsigned long long, DigitsMedium>();
    PerformanceTest<unsigned long long, DigitsLong>();
#endif

    PerformanceTest<unsigned char, DigitsShort, TestOpType::Div>();
    PerformanceTest<unsigned char, DigitsMedium, TestOpType::Div>();
    PerformanceTest<unsigned char, DigitsLong, TestOpType::Div>();
    PerformanceTest<unsigned short, DigitsShort, TestOpType::Div>();
    PerformanceTest<unsigned short, DigitsMedium, TestOpType::Div>();
    PerformanceTest<unsigned short, DigitsLong, TestOpType::Div>();
    PerformanceTest<unsigned int, DigitsShort, TestOpType::Div>();
    PerformanceTest<unsigned int, DigitsMedium, TestOpType::Div>();
    PerformanceTest<unsigned int, DigitsLong, TestOpType::Div>();
#ifdef __SIZEOF_INT128__
    PerformanceTest<unsigned long long, DigitsShort, TestOpType::Div>();
    PerformanceTest<unsigned long long, DigitsMedium, TestOpType::Div>();
    PerformanceTest<unsigned long long, DigitsLong, TestOpType::Div>();
#endif

    PerformanceTest<unsigned char, DigitsShort, TestOpType::Add>();
    PerformanceTest<unsigned char, DigitsMedium, TestOpType::Add>();
    PerformanceTest<unsigned char, DigitsLong, TestOpType::Add>();
    PerformanceTest<unsigned short, DigitsShort, TestOpType::Add>();
    PerformanceTest<unsigned short, DigitsMedium, TestOpType::Add>();
    PerformanceTest<unsigned short, DigitsLong, TestOpType::Add>();
    PerformanceTest<unsigned int, DigitsShort, TestOpType::Add>();
    PerformanceTest<unsigned int, DigitsMedium, TestOpType::Add>();
    PerformanceTest<unsigned int, DigitsLong, TestOpType::Add>();
#ifdef __SIZEOF_INT128__
    PerformanceTest<unsigned long long, DigitsShort, TestOpType::Add>();
    PerformanceTest<unsigned long long, DigitsMedium, TestOpType::Add>();
    PerformanceTest<unsigned long long, DigitsLong, TestOpType::Add>();
#endif

    PerformanceTest<unsigned char, DigitsShort, TestOpType::Sub>();
    PerformanceTest<unsigned char, DigitsMedium, TestOpType::Sub>();
    PerformanceTest<unsigned char, DigitsLong, TestOpType::Sub>();
    PerformanceTest<unsigned short, DigitsShort, TestOpType::Sub>();
    PerformanceTest<unsigned short, DigitsMedium, TestOpType::Sub>();
    PerformanceTest<unsigned short, DigitsLong, TestOpType::Sub>();
    PerformanceTest<unsigned int, DigitsShort, TestOpType::Sub>();
    PerformanceTest<unsigned int, DigitsMedium, TestOpType::Sub>();
    PerformanceTest<unsigned int, DigitsLong, TestOpType::Sub>();
#ifdef __SIZEOF_INT128__
    PerformanceTest<unsigned long long, DigitsShort, TestOpType::Sub>();
    PerformanceTest<unsigned long long, DigitsMedium, TestOpType::Sub>();
    PerformanceTest<unsigned long long, DigitsLong, TestOpType::Sub>();
#endif
}

#endif