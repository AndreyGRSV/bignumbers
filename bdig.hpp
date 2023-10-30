/*
   Template class of big numbers with fixed point arithmetic for
   simplifying usage of big numbers. Usage from C++98.

    Author: Andrey Svyatovets
*/

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#include <regex>
#endif

namespace sag {
const char* pPI_1000 = "3.14159265358979323846264338327950288419716939937510"
                       "58209749445923078164062862089986280348253421170679"
                       "82148086513282306647093844609550582231725359408128"
                       "48111745028410270193852110555964462294895493038196"
                       "44288109756659334461284756482337867831652712019091"
                       "45648566923460348610454326648213393607260249141273"
                       "72458700660631558817488152092096282925409171536436"
                       "78925903600113305305488204665213841469519415116094"
                       "33057270365759591953092186117381932611793105118548"
                       "07446237996274956735188575272489122793818301194912"
                       "98336733624406566430860213949463952247371907021798"
                       "60943702770539217176293176752384674818467669405132"
                       "00056812714526356082778577134275778960917363717872"
                       "14684409012249534301465495853710507922796892589235"
                       "42019956112129021960864034418159813629774771309960"
                       "51870721134999999837297804995105973173281609631859"
                       "50244594553469083026425223082533446850352619311881"
                       "71010003137838752886587533208381420617177669147303"
                       "59825349042875546873115956286388235378759375195778"
                       "18577805321712268066130019278766111959092164201989";
const char* pE_1000 = "2.71828182845904523536028747135266249775724709369995"
                      "95749669676277240766303535475945713821785251664274"
                      "27466391932003059921817413596629043572900334295260"
                      "59563073813232862794349076323382988075319525101901"
                      "15738341879307021540891499348841675092447614606680"
                      "82264800168477411853742345442437107539077744992069"
                      "55170276183860626133138458300075204493382656029760"
                      "67371132007093287091274437470472306969772093101416"
                      "92836819025515108657463772111252389784425056953696"
                      "77078544996996794686445490598793163688923009879312"
                      "77361782154249992295763514822082698951936680331825"
                      "28869398496465105820939239829488793320362509443117"
                      "30123819706841614039701983767932068328237646480429"
                      "53118023287825098194558153017567173613320698112509"
                      "96181881593041690351598888519345807273866738589422"
                      "87922849989208680582574927961048419844436346324496"
                      "84875602336248270419786232090021609902353043699418"
                      "49146314093431738143640546253152096183690888707016"
                      "76839642437814059271456354906130310720851038375051"
                      "01157477041718986106873969655212671546889570350354";

#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)

#ifndef __SIZEOF_INT128__
using uint128_t = unsigned long long;
#else
using uint128_t = unsigned __int128;
#endif

using namespace std;

#define QUOTE(x) #x
#define STR(x) QUOTE(x)
#define PREC_FOR_MATH_FUNC 5
#define PREC_FOR_MATH_FUNC_TEXT                                                                    \
    "The precision should be more then " STR(PREC_FOR_MATH_FUNC) " digits"
#define SIZE_FOR_MATH_FUNC 300
#define SIZE_FOR_MATH_FUNC_TEXT "The size should be more then " STR(SIZE_FOR_MATH_FUNC) " digits"
#define FOR_INTEGER_ONLY_TEXT "This function only for integer digits without precision"
#else // C++98
template<bool B, class T = void>
struct enable_if {
};
template<class T>
struct enable_if<true, T> {
    typedef T type;
};

struct true_type {
    enum { value = true };
};
struct false_type {
    enum { value = false };
};

template<class T, class U>
struct is_same : false_type {
};
template<class T>
struct is_same<T, T> : true_type {
};

template<bool B, class T, class F>
struct conditional {
    typedef T type;
};
template<class T, class F>
struct conditional<false, T, F> {
    typedef F type;
};

#endif
template<typename T>
struct is_bdig {
    template<typename U>
    static char Test(typename U::bdig*);
    template<typename U>
    static int Test(...);
    static const bool value = (sizeof(Test<T>(0)) == sizeof(char));
};

/* Allocation objects in heap for functions with deep recursion   */
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#define DECLOBJ(name) std::unique_ptr<bdig> name
#define ASSIGNOBJ(name, param) name.reset(new bdig(param)) // name = std::make_unique<bdig>(param)
#define CRTHEAPOBJ(name, param)                                                                    \
    DECLOBJ(name)                                                                                  \
        = std::unique_ptr<bdig>(new bdig(param)) // DECLOBJ(name) = std::make_unique<bdig>(param)
#define CRTHEAPOBJNP(name) CRTHEAPOBJ(name, )
#else
#define DECLOBJ(name) std::auto_ptr<bdig> name
#define ASSIGNOBJ(name, param) name.reset(new bdig(param))
#define CRTHEAPOBJ(name, param) std::auto_ptr<bdig> name(new bdig(param))
#define CRTHEAPOBJNP(name) std::auto_ptr<bdig> name(new bdig())
#endif

/// @brief This template class cover manipulation with big numbers. Number size up to indicated in parameters.
/// @tparam T Type of element in array to hold number. This type can be unsigned integer type from char to long long and specific to compiler int128_t
/// @tparam digits Number of digits in integer part. Depends on system resources and can be as big as it required. Tested values up to 20'000
/// @tparam prec Number of digits after point.
template<const int digits = 100, const int prec = 0, class T = unsigned char>
class bdig {

    static const typename enable_if<!std::numeric_limits<T>::is_signed, std::size_t>::type isz
        = ((prec * 2 + digits) / (std::numeric_limits<T>::digits * 301 / 1000)) + 1; //!< Size of required buffer size of bdig value

    /// @brief Internal buffer for holding value of bdig type
    /// @tparam Tb Type of element of array of buffer
    /// @tparam size of the buffer array
    template<class Tb, std::size_t size = isz>
    class buffer {
        Tb _buffer[size]; //!< Array of holding bdig value
        std::size_t least_significant_index; //!< Most index of element with value in array.

    public:

        /// @brief Default constructor of Buffer class
        buffer() {
            clear();
        }
        /// @brief Clear buffer and reset buffer data
        void clear() {
            memset(_buffer, 0, size * sizeof(Tb));
            least_significant_index = size - 1;
        }
        /// @brief Subscript operator of Buffer type
        /// @param idx index to element in buffer
        /// @return reference to element value in array
        const T& operator[](std::size_t idx) const {
            return _buffer[idx];
        }
        /// @brief Method for setting value to array of bdig value
        /// @param idx index to element in buffer
        /// @param _v value to set
        inline void set(std::size_t idx, const Tb _v) {
            if (idx < size) {
                _buffer[idx] = _v;
                if (idx < least_significant_index && _v)
                    least_significant_index = idx;
                else if (idx == least_significant_index && !_v) {
                    while (!_buffer[least_significant_index] && least_significant_index < size - 1)
                        least_significant_index++;
                }
            }
        }
        /// @brief Getter of most index of element with value in array.
        /// @return index value
        inline std::size_t get_lsi() const {
            return least_significant_index;
        }
    };

    buffer<T> integer; //!< Buffer of value.
    bool is_negative;  //!< holer of negative flag.
    static bdig i10;   //!< value of precision divider.
    const static T mask_max_bit = static_cast<T>(1) << (std::numeric_limits<T>::digits - 1); //!< mask of maximum bit for element type.


    /// @brief Shift buffer to left on elements counter
    /// @param bytes Counter of elements to shift. If number of bytes more then buffer size then bdig value will be cleared.
    void shlb(std::size_t bytes)
    {
        if (!bytes)
            return;
        if (bytes >= isz) {
            integer.clear();
            return;
        }
        std::size_t idx_src = integer.get_lsi();
        std::size_t idx_dst = integer.get_lsi() - bytes;

        std::size_t move_size = isz - integer.get_lsi();
        for (std::size_t i = 0; i < move_size; i++, idx_dst++, idx_src++)
            integer.set(idx_dst, integer[idx_src]);

        for (std::size_t i = 0, idx_set = (isz - bytes); i < bytes; i++, idx_set++)
            integer.set(idx_set, 0);
    }

    /// @brief Shift to left one element in digit buffer.
    /// @param idx Index of element in the buffer
    /// @param bits Number of bits to shift the buffer element.
    /// @param set_bit Bitmask from previous shifted element for fill of freed part.
    /// @return Bitmask for next element
    inline T shl_sop(std::size_t idx, const int bits, const T set_bit)
    {
        if (!integer[idx] && !set_bit)
            return 0;
        T bit = integer[idx] >> (std::numeric_limits<T>::digits - bits);
        integer.set(idx, integer[idx] << bits);
        if (set_bit)
            integer.set(idx, integer[idx] | set_bit);
        return bit;
    }
    /// @brief Shift buffer to left on bits counter less then element size in bits.
    /// @param bits Counter of bits to shift
    void shl(const int bits = 1)
    {
        if (!bits)
            return;
        std::size_t msi = most_significant_index();
        msi = msi == 0 ? 0 : msi - 1;

        T set_bit = 0;
        for (std::size_t i = isz - 1; i >= msi; i--) {
            set_bit = shl_sop(i, bits, set_bit);
            if (!i)
                break;
        }
    }

    /// @brief Shift buffer to right on elements counter
    /// @param bytes Number of elements to shift
    void shrb(std::size_t bytes)
    {
        for (std::size_t i = 0; i < isz - bytes; i++)
            integer.set(isz - i - 1, integer[isz - bytes - i - 1]);
        for (std::size_t i = 0; i < bytes; i++)
            integer.set(i, 0);
    }
    /// @brief Shift to right one element in digit buffer.
    /// @param idx Index of element in the buffer
    /// @param bits Number of bits to shift the buffer element.
    /// @param set_bit Bitmask from previous shifted element for fill of freed part.
    /// @return Bitmask for next element
    inline T shr_sop(std::size_t idx, const int bits, const T set_bit)
    {
        if (!integer[idx] && !set_bit)
            return 0;
        T bit = integer[idx] << (std::numeric_limits<T>::digits - bits);
        integer.set(idx, integer[idx] >> bits);
        if (set_bit)
            integer.set(idx, integer[idx] | set_bit);
        return bit;
    }
    /// @brief Shift buffer to right on bits counter less then element size in bits.
    /// @param bits Counter of bits to shift
    void shr(const unsigned bits = 1)
    {
        T set_bit = 0;
        std::size_t msi = most_significant_index();
        for (std::size_t i = msi; i < isz; i++) {
            set_bit = shr_sop(i, bits, set_bit);
        }
    }
    inline std::size_t most_significant_index() const {
        return integer.get_lsi();
    }

    /// @brief Find most significant bit in value
    /// @tparam Ti Type of value
    /// @param value parameter where to find most significant bit
    /// @param  dummy parameter for template substitution
    /// @return number of most significant bit
    template<class Ti>
    int most_significant_bit(
        Ti value,
        typename enable_if<
            is_same<Ti, unsigned short>::value || is_same<Ti, unsigned long>::value
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                || is_same<Ti, unsigned int>::value || is_same<Ti, unsigned long long>::value
                || is_same<Ti, uint128_t>::value
#endif
            ,
            Ti>::type
        = 0) const
    {
        typedef typename conditional<
            is_same<Ti, unsigned short>::value,
            unsigned char,
            typename conditional<
                is_same<Ti, unsigned long>::value,
                unsigned short,
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                typename conditional<
                    is_same_v<Ti, unsigned int>,
                    unsigned short,
                    typename conditional<
                        is_same_v<Ti, unsigned long long>,
                        typename conditional<
                            numeric_limits<Ti>::digits == numeric_limits<unsigned long>::digits,
                            unsigned int,
                            unsigned long>::type,
                        typename conditional<
                            is_same_v<Ti, uint128_t>,
                            unsigned long long,
#endif
                            void>::type
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                        >::type>::type>::type
#endif
            >::type subT;

        if (value & (Ti)std::numeric_limits<subT>::max() << std::numeric_limits<subT>::digits)
            return most_significant_bit((subT)(value >> std::numeric_limits<subT>::digits))
                + std::numeric_limits<subT>::digits;
        return most_significant_bit((subT)value);
    }
    /// @brief Overloaded method for finding most significant bit in type unsigned char.
    /// @param value unsigned char value
    /// @return number of most significant bit
    int most_significant_bit(unsigned char value) const
    {
        if (value & 0xF0) {
            if (value & 0xC0)
                return (value & 0x80) ? 7 : 6;
            return (value & 0x20) ? 5 : 4;
        }
        if (value & 0x0C)
            return (value & 0x08) ? 3 : 2;
        return (value & 0x02) ? 1 : 0;
    }

    /// @brief Division method for bdig type value by elements in buffer array.
    /// @param v value of right side operand of division
    /// @param premainder pointer to remainder after division. If NULL then id not filled.
    /// @return reference to himself
    bdig& div(const bdig& v, bdig* premainder = NULL)
    {

        if (!v)
            return *this;

        CRTHEAPOBJNP(remainder);
        CRTHEAPOBJNP(remainder_tmp);
        CRTHEAPOBJNP(borrow);
        CRTHEAPOBJ(divisor, v);
        CRTHEAPOBJ(result, 0);

        bool store_is_negative = is_negative;
        is_negative = false;
        *remainder = *this;
        integer.clear();

        *divisor = (*divisor).abs();
        // Shift Divider to Value max significant index
        const std::size_t ri = (*remainder).most_significant_index();
        const std::size_t di = (*divisor).most_significant_index();
        std::size_t shift = 0;
        if (ri < di) {
            shift = di - ri;
            (*divisor).shlb(shift);
        }
        // If Divider > Value Shit Divider back to one position(In current sample is not needeed)
        if ((*divisor) > (*remainder) && shift) {
            (*divisor).shrb(1);
            shift--;
        }

        // Both items have value only in one first limb
        std::size_t index = isz - 1;
        if (ri == index && di == index) {
            (*result).integer.set(index, (*remainder).integer[index] / (*divisor).integer[index]);
            (*remainder).integer.set(index, (*remainder).integer[index] % (*divisor).integer[index]);
        } else {
            const upT BaseValue = static_cast<upT>(std::numeric_limits<T>::max()) + 1;

            while ((*divisor) <= (*remainder)) {

                upT result_value = 0;
                upT last_diff = 0;
                const std::size_t result_index = (*divisor).most_significant_index();
                const std::size_t remainder_index = (*remainder).most_significant_index();
                std::size_t current_index = result_index;
                while (current_index < isz) {
                    if (result_index == current_index) {
                        result_value = (*remainder).integer[current_index];
                        if (remainder_index < result_index) {
                            result_value += (*remainder).integer[current_index - 1] * BaseValue;
                        }
                        result_value -= (*borrow).integer[current_index];
                        upT remainder_value = result_value;
                        result_value /= (*divisor).integer[current_index];
                        if (result_value >= BaseValue)
                            result_value = BaseValue - 1;
                        remainder_value -= (*divisor).integer[current_index] * result_value;
                        (*remainder_tmp).integer.set(current_index, static_cast<T>(remainder_value));
                        current_index++;
                    } else {
                        upT need_value = (*divisor).integer[current_index] * result_value;
                        upT borrow_value = 0;
                        if (need_value > (*remainder).integer[current_index]) {
                            upT full_value = need_value + (*borrow).integer[current_index] - (*remainder).integer[current_index];
                            borrow_value = full_value / BaseValue + ((full_value % BaseValue) ? 1 : 0);
                        }
                        upT diff = borrow_value < (*borrow).integer[current_index - 1] ?
                            (*borrow).integer[current_index - 1] - borrow_value :
                            borrow_value - (*borrow).integer[current_index - 1];
                        if (diff && diff == last_diff) {
                            if ((*borrow).integer[current_index - 1] != borrow_value) {
                                (*borrow).integer.set(current_index - 1, static_cast<T>(borrow_value));
                            }

                            (*remainder).div_bits((*divisor), remainder_tmp.get());
                            result_value = (*remainder).integer[isz - 1];
                            break;
                        }
                        last_diff = diff;
                        if (borrow_value != (*borrow).integer[current_index - 1]) {
                            (*borrow).integer.set(current_index - 1, static_cast<T>(borrow_value));
                            current_index--;
                        } else {
                            upT remainder_value = (*borrow).integer[current_index - 1] * BaseValue
                                + (*remainder).integer[current_index]
                                - (*borrow).integer[current_index]
                                - (*divisor).integer[current_index] * result_value;
                            if (remainder_value >= BaseValue) {
                                (*remainder_tmp).integer.set(current_index - 1, (*remainder_tmp).integer[current_index - 1] + 1);
                            }
                            (*remainder_tmp).integer.set(current_index, static_cast<T>(remainder_value));
                            current_index++;
                        }
                    }
                }

                (*result).integer.set(isz - 1 - shift, static_cast<T>(result_value));
                (*remainder) = (*remainder_tmp);
                (*remainder_tmp) = 0;
                (*borrow) = 0;

                if (!shift)
                    break;

                shift--;
                (*divisor).shrb(1);
            }
        }

        is_negative = store_is_negative != v.is_negative;
        (*remainder).is_negative = store_is_negative != v.is_negative;
        if (premainder)
            *premainder = *remainder;

        (*result).is_negative = store_is_negative != v.is_negative;
        if (premainder != this)
            *this = *result;
        return *this;
    }
    /// @brief Method for subtraction of v element from this element by index.
    /// @param idx index in buffer array.
    /// @param v value for subtraction
    /// @param c carry flag
    /// @return new carry flag
    inline bool sub_sop(std::size_t idx, const bdig& v, const bool c)
    {
        if (!v.integer[idx] && !c)
            return c;

        bool c2 = false;
        if (c) {
            if (integer[idx] == 0)
                c2 = true;
            integer.set(idx, integer[idx] - 1);
        }
        bool cr = (integer[idx] < v.integer[idx]) || c2;
        integer.set(idx, integer[idx] - v.integer[idx]);
        return cr;
    }

    /// @brief Classic division method for bdig type value by bits.
    /// @param v value of right side operand of division
    /// @param premainder pointer to remainder after division. If NULL then id not filled.
    /// @return reference to himself
    bdig& div_bits(const bdig& v, bdig* premainder = NULL)
    {

        if (!v)
            return *this;

        CRTHEAPOBJNP(remainder);
        CRTHEAPOBJ(divisor, v);

        bool store_is_negative = is_negative;
        is_negative = false;
        *remainder = *this;
        integer.clear();

        CRTHEAPOBJNP(shift);
        (*shift).integer.set(isz - 1, 1);
        *divisor = (*divisor).abs();

        // Align divisor with remainder in array
        std::size_t ri = (*remainder).most_significant_index();
        std::size_t di = (*divisor).most_significant_index();
        if (ri < di) {
            (*divisor).shlb(di - ri);
            (*shift).shlb(di - ri);
        }

        while (*remainder >= v) {
            while (!(*remainder).integer[ri] && ri < isz) {
                if ((*divisor).integer[ri]) {
                    int d_msb = most_significant_bit((*divisor).integer[ri]);
                    (*divisor).shr(d_msb + 1);
                    (*shift).shr(d_msb + 1);
                }
                ri++;
            }
            int d_msb = most_significant_bit((*divisor).integer[ri]);
            int r_msb = most_significant_bit((*remainder).integer[ri]);
            int bits = r_msb - d_msb;

            std::size_t msi_divisor = (*divisor).most_significant_index();
            std::size_t msi_shift = (*shift).most_significant_index();
            std::size_t msi = std::min(msi_shift, msi_divisor);
            msi -= msi ? 1 : 0;

            if (bits) {
                T set_bit = 0, set_bit1 = 0;
                if (bits > 0) {
                    for (std::size_t i = isz - 1; i >= msi; i--) {
                        set_bit = (*divisor).shl_sop(i, bits, set_bit);
                        set_bit1 = (*shift).shl_sop(i, bits, set_bit1);
                        if (!i)
                            break;
                    }
                } else {
                    for (std::size_t i = msi; i < isz; ++i) {
                        set_bit = (*divisor).shr_sop(i, -bits, set_bit);
                        set_bit1 = (*shift).shr_sop(i, -bits, set_bit1);
                    }
                }
            }
            while ((*divisor) > (*remainder)) {
                T set_bit = 0, set_bit1 = 0;
                for (std::size_t i = msi; i < isz; i++) {
                    set_bit = (*divisor).shr_sop(i, 1, set_bit);
                    set_bit1 = (*shift).shr_sop(i, 1, set_bit1);
                }
            }

            std::size_t msi_quotient = most_significant_index();
            std::size_t msi_remainder = (*remainder).most_significant_index();
            msi = std::min(msi_remainder, msi_quotient);
            msi -= msi ? 1 : 0;

            bool cs = false, csm = false;
            for (std::size_t i = isz - 1; i >= msi; --i) {
                csm = sum_sop(i, *shift, csm);
                cs = (*remainder).sub_sop(i, *divisor, cs);
                if (!i)
                    break;
            }
            if (!*divisor)
                break;
        }
        is_negative = store_is_negative != v.is_negative;
        (*remainder).is_negative = store_is_negative != v.is_negative;
        if (premainder)
            *premainder = *remainder;
        return *this;
    }

    /// @brief Method to prepare and update i10 static value of precision divider.
    /// @return i10 static value of precision divider
    bdig& prec_value()
    {
        if (!i10) {
            i10.integer.set(isz - 1, (prec ? 10 : 1));
            i10 = i10.pow(prec, false);
        }
        return i10;
    }

    /// @brief Make bdig value integer by multiply to prec_value i10.
    /// @return reference to himself
    bdig& prec_up()
    {
        if (prec) {
            return mul<T>(prec_value());
        }
        return *this;
    }
    /// @brief Make bdig value with digits after point by dividing to prec_value i10.
    /// @return reference to himself
    bdig& prec_down()
    {
        if (prec)
            return div(prec_value());
        return *this;
    }

    /// @brief Found type bigger as type of buffer array element
    typedef typename conditional<
        is_same<T, unsigned char>::value
            && std::numeric_limits<unsigned short>::digits / std::numeric_limits<T>::digits == 2,
        unsigned short,
        typename conditional<
            is_same<T, unsigned short>::value
                && std::numeric_limits<unsigned int>::digits / std::numeric_limits<T>::digits == 2,
            unsigned int,
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
            typename conditional<is_same<T, unsigned int>::value && std::numeric_limits<unsigned long>::digits / std::numeric_limits<T>::digits == 2,
                unsigned long,
#ifndef __SIZEOF_INT128__
            unsigned long long>::type
#else
            typename conditional<is_same<T, unsigned long>::value && std::numeric_limits<unsigned long long>::digits / std::numeric_limits<T>::digits == 2,
            unsigned long long,
            uint128_t>::type>::type
#endif
#else
            unsigned long>::type
#endif
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
            >::type
#endif
        >::type upT;

    /// @brief Static method for add value to buffer element by index
    /// @param index index of element to add value
    /// @param result bdig object to modify value
    /// @param value value to add
    static void add(std::size_t index, bdig& result, T value)
    {
        upT x = result.integer[index];
        upT y = value;
        upT d = x + y;
        // upT d = result.integer[index] + value;
        T r1 = d >> std::numeric_limits<T>::digits;
        T r0 = d & std::numeric_limits<T>::max();

        if (r1) // overflow
        {
            // add  high part to high order limb
            add(index - 1, result, r1);
        }
        // Set low order part to current limb
        result.integer.set(index, r0);
    }
    /// @brief Template of multiplication method for types with can have the bigger type.
    /// @tparam Ti type to check and work with.
    /// @param v right side value to multiply
    /// @param  dummy parameter for method substitution
    /// @return reference to himself with result
    template<class Ti>
    bdig& mul(
        const bdig& v,
        typename enable_if<
            is_same<Ti, unsigned char>::value || is_same<Ti, unsigned short>::value
                || (is_same<Ti, unsigned int>::value
                    && std::numeric_limits<Ti>::digits != std::numeric_limits<unsigned long>::digits
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                    && std::numeric_limits<Ti>::digits
                        != std::numeric_limits<unsigned long long>::digits
#endif
                    ),
            Ti>::type
        = 0)
    {
        bdig result;
        std::size_t lsi = v.most_significant_index();
        for (std::size_t v_i = isz - 1; v_i >= lsi; v_i--) {
            if (v.integer[v_i]) {
                for (std::size_t idx = isz - 1; idx >= most_significant_index(); idx--) {
                    if (integer[idx]) {
                        upT d = static_cast<upT>(v.integer[v_i]) * static_cast<upT>(integer[idx]);
                        Ti r1 = d >> std::numeric_limits<Ti>::digits;
                        Ti r0 = d & std::numeric_limits<Ti>::max();
                        const std::size_t set_index = idx - (isz - v_i);
                        add(set_index, result, r1);
                        add(set_index + 1, result, r0);
                    }
                    if (!idx)
                        break;
                }
            }
            if (!v_i)
                break;
        }
        bool store_is_negative = is_negative;
        bool store_is_negative_v = v.is_negative;
        *this = result;
        is_negative = store_is_negative != store_is_negative_v;
        return *this;
    }
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    /// @brief Template of multiplication method for types which does not have the bigger type. Multiplication with bitwise method
    /// @tparam Ti type to check and work with.
    /// @param v right side value to multiply
    /// @param  dummy parameter for method substitution
    /// @return reference to himself with result
    template<class Ti>
    bdig& mul(
        const bdig& v,
        typename enable_if<
            is_same<Ti, unsigned long long>::value || is_same<Ti, uint128_t>::value
                || (is_same<Ti, unsigned int>::value
                    && std::numeric_limits<Ti>::digits
                        == std::numeric_limits<unsigned long>::digits),
            Ti>::type
        = 0)
    {
        CRTHEAPOBJ(multiplicant, v);
        *multiplicant = (*multiplicant).abs();

        CRTHEAPOBJ(sum, *this);
        if (sum->isnegative())
            *sum = -(*sum);
        bool store_is_negative = is_negative;
        if (is_negative)
            is_negative = false;
        integer.clear();
        std::size_t shift = 0;
        for (std::size_t i = (*multiplicant).most_significant_index(); i < isz; i++) {
            for (int b = 0; b < std::numeric_limits<T>::digits; b++) {
                if ((*multiplicant).integer[isz - 1] & (Ti)1) {
                    (*sum).shlb(shift / std::numeric_limits<Ti>::digits);

                    shift %= std::numeric_limits<T>::digits;
                    Ti set_bit = 0;
                    bool c = false;
                    std::size_t msi_sum = (*sum).most_significant_index();
                    std::size_t msi = most_significant_index();
                    msi = std::min(msi, msi_sum);
                    msi -= msi ? 1 : 0;

                    for (std::size_t j = isz - 1; j >= msi; j--) {
                        if (shift)
                            set_bit = (*sum).shl_sop(j, shift, set_bit);
                        c = sum_sop(j, *sum, c);
                        if (!j)
                            break;
                    }
                    shift = 0;
                }
                (*multiplicant).integer.set(isz - 1, (*multiplicant).integer[isz - 1] >> 1);
                shift += 1;
            }
            (*multiplicant).shrb(1);
        }

        if (&v == this) {
            is_negative = store_is_negative;
        }
        is_negative = store_is_negative != v.is_negative;
        return *this;
    }
#endif
    /// @brief Method for comparison of two bdig values.
    /// @param v1 left bdig value
    /// @param v2 right bdig value
    /// @param r1 flag of left bdig value check
    /// @param r2 flag of right bdig value check
    /// @return Combination r1 - true, r2 false
    ///                result true - v1 < v2, false  v1 > v2.
    ///         Combination r1 - false, r2 true
    ///                result true - v1 > v2, false  v1 < v2.
    ///         Combination r1 - false, r2 false
    ///                result true - v1 == v2, false  v1 != v2.
    ///         Combination r1 - true, r2 true is equivalent to r1 - true, r2 false
    static bool _Cmp(const bdig& v1, const bdig& v2, bool r1, bool r2)
    {

        std::size_t lsi1 = v1.integer.get_lsi();
        std::size_t lsi2 = v2.integer.get_lsi();

        int res = 0;
        bool zero = true;

        if (lsi1 == lsi2) {
            for (std::size_t i = lsi1; i < isz; ++i) {
                res = (int)(v1.integer[i] < v2.integer[i]
                        ? -1
                        : v1.integer[i] == v2.integer[i] ? 0 : 1);
                if (v1.integer[i] || v2.integer[i])
                    zero = false;

                if (res)
                    break;
            }
        } else {
            res = lsi2 < lsi1 ? -1 : 1;
            zero = false;
        }

        if (v1.is_negative != v2.is_negative)
            // v1 < v2
            return r1 ? v1.is_negative :
                      // v1 > v2
                r2 ? v2.is_negative
                   :
                   // v1 == v2
                zero ? true
                     : false;

        // int res1 = memcmp (v1.integer, v2.integer, sizeof (v1.integer)); // Can
        // works only with big-endian
        // v1 < v2
        if (r1)
            return (res < 0);
        // v1 > v2
        if (r2)
            return (res > 0);
        // v1 == v2
        // !r1 && !r2
        return (res == 0);
    }
    /// @brief Convert starting part of buffer array to bigger integer value.
    /// @tparam Ti type to check. Type size bigger then element type size.
    /// @param  dummy parameter for template substitution
    /// @return converted value
    template<class Ti>
    Ti _toll(typename enable_if<!is_same<Ti, T>::value, Ti>::type = 0) const {
        Ti result = 0;
        const int type_size = numeric_limits<Ti>::digits / numeric_limits<T>::digits;
        for (std::size_t i = isz > type_size ? isz - type_size : 0; i < isz; i++) {
            result += integer[i];
            if (i < isz - 1)
                result <<= std::numeric_limits<T>::digits; // UB if Ti == T !!!
        }
        return result;
    }
    /// @brief Variant for same type element. Convert starting part of buffer array to bigger integer value.
    /// @tparam Ti type to check. Type size is equivalent to buffer element type size.
    /// @param  dummy parameter for template substitution
    /// @return value of element
    template<class Ti>
    Ti _toll(typename enable_if<is_same<Ti, T>::value, Ti>::type = 0) const {
        return integer[isz - 1];
    }
    /// @brief Method for addition of v element to this element by index.
    /// @param idx index in buffer array.
    /// @param v value for add
    /// @param c carry flag
    /// @return new carry flag
    inline bool sum_sop(std::size_t idx, const bdig& v, bool c)
    {
        if (v.integer[idx] == 0 && !c)
            return c;

        T one = (c ? 1 : 0);
        const T diff = std::numeric_limits<T>::max() - integer[idx];
        c = (diff < v.integer[idx] || (diff == v.integer[idx] && one));
        if (c)
            integer.set(idx, (v.integer[idx] + one) - diff - 1);
        else
            integer.set(idx, integer[idx] + v.integer[idx] + one);
        return c;
    }
    /// @brief Calculate power of value of bdig type. Internal method.
    /// @param _s value to calculating power of value of bdig type.
    /// @param with_prec calculate digits with digits after point
    /// @return result of method as reference to himself
    bdig& _pow(const int _s, bool with_prec = true)
    {
        if (_s == 1)
            return *this;
        if (_s == 0) {
            if (with_prec)
                *this = 1;
            else
                integer.set(isz - 1, 1);
            return *this;
        }
        if (_s == 2) {
            mul<T>(*this);
            if (with_prec)
                prec_down();
            return *this;
        }
        CRTHEAPOBJ(result, *this);
        int v = (_s < 0 ? -_s : _s);
        _pow(v / 2, with_prec)._pow(2, with_prec);
        if (v % 2) {
            mul<T>(*result);
            if (with_prec)
                prec_down();
        }
        return *this;
    }

public:
    const static int digits10 = digits + prec;
    const static int size = (prec * 2 / (std::numeric_limits<T>::digits * 301 / 1000)) + 1;

    ////////////////////////////////////
    // Constructors

    /// @brief Default constructor
    bdig() : is_negative(false) {
    }
    /// @brief Constructor for all unsigned integer values.
    /// @param v unsigned integer value
    template<class Ti>
    bdig(
        Ti v,
        typename enable_if<
            std::numeric_limits<Ti>::is_integer && !std::numeric_limits<T>::is_signed,
            Ti>::type
        = 0)
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
        : bdig()
    {
#else
    {
        is_negative = false;
#endif
        *this = v;
    }
    /// @brief Constructor for all char pointers values.
    /// @param v char pointer value
    template<class Ti>
    bdig(
        const Ti* v,
        typename enable_if<
            (is_same<unsigned char, Ti>::value || is_same<char, Ti>::value
             || is_same<signed char, Ti>::value)
                && !std::numeric_limits<T>::is_signed,
            Ti>::type
        = 0)
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
        : bdig(){
#else
    {
        is_negative = false;
#endif
        *this = v;
    }
    /// @brief Constructor for std::string values.
    /// @param v std::string value
    bdig(const std::string& v)
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
        : bdig(){
#else
    {
        is_negative = false;
#endif
        *this = v;
    }

////////////////////////////////////
// Math methods

/// @brief Absolute value of current bdig type.
/// @return Result as reference to himself
bdig& abs()
{
    is_negative = false;
    return *this;
}
/// @brief Calculate power of value of bdig type.
/// @param _s value to calculate power of value bdig type
/// @param with_prec calculate with digits after point
/// @return result of method as reference to himself.
bdig& pow(const int _s, bool with_prec = true)
{
    _pow(_s, with_prec);
    if (_s < 0) {
        // dividing by zero
        if (*this == 0) {
            return *this;
        }

        bdig one;
        one = 1;
        *this = one.prec_up().div(*this);
    }
    return *this;
}
/// @brief Calculate square root of current value of bdig type
/// @return calculated bdig type value.
bdig sqrt() const
{
    // Invalid value for sqrt
    if (is_negative)
        return 0;

    bdig res;
    bdig v = *this;
    v = v.prec_up();
    bdig bit;
    if (!v.most_significant_index()) {
        std::size_t msb = v.most_significant_bit(v.integer[0]);
        if (msb + 1 == std::numeric_limits<T>::digits - 1) {
            // Overflow
            return res;
        }
        bit.set_bit(0, false);
        bit.set_bit((msb / 2 + 1) * 2 + ((isz - 1) * std::numeric_limits<T>::digits));
    } else {
        bit.set_bit(((isz - v.most_significant_index()) * std::numeric_limits<T>::digits));
    }

    while (bit > v)
        bit >>= 2;

    while (bit != 0) {
        if (v >= res + bit) {
            v -= res + bit;
            res = (res >> 1) + bit;
        } else
            res >>= 1;
        bit >>= 2;
    }
    return res;
}
/// @brief Calculate sinus of angle and place result to current bdig type. Iterations of calculating of series convergence is 140.
/// @param a angle in degrees to calculate sinus
/// @return calculated bdig type value.
bdig& sin(int a){
    *this = sin(a,140);
    return *this;
}
/// @brief Static method to calculate sinus of angle.
/// @param a angle in degrees to calculate sinus
/// @param iterations iterations of calculating of series convergence
/// @return calculated bdig type value.
static bdig sin(int a, const int iterations)
{
    if (!(a % 180))
        return 0;
    return -cos(a + 90, iterations);
}
/// @brief Calculate cosines of angle and place result to current bdig type. Iterations of calculating of series convergence is 140.
/// @param a angle in degrees to calculate sinus
/// @return calculated bdig type value.
bdig& cos(int a){
    *this = cos(a, 140);
    return *this;
}
/// @brief Static method to calculate cosines of angle.
/// @param a angle in degrees to calculate cosines
/// @param iterations iterations of calculating of series convergence
/// @return calculated bdig type value.
static bdig cos(int a, const int iterations)
{
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    static_assert(prec >= PREC_FOR_MATH_FUNC, PREC_FOR_MATH_FUNC_TEXT);
#endif
    if (a == 180)
        return -1;
    CRTHEAPOBJ(bPI,pPI_1000);
    CRTHEAPOBJ(x, a > 180 ? a % 180 : a);

    (*x) *= (*bPI) / 180;
    CRTHEAPOBJ(t, 1);
    CRTHEAPOBJ(sum, 1);

    for (int i = 1; i <= iterations; i++) {
        (*t) = -(*t) * (*x) * (*x) / (2 * i * (2 * i - 1));
        (*sum) += (*t);
    }
    return ((a / 180 % 2) ? -(*sum) : (*sum));
}
/// @brief Calculate logarithm of current bdig value by base two.
/// @param iterations iterations of calculating of series convergence
/// @return Calculated value
bdig log2(const int iterations = 100) const
{
    return log2(*this, iterations);
}
/// @brief Static method to calculate logarithm of passed bdig value by base two.
/// @param n bdig value for calculate logarithm
/// @param iterations iterations of calculating of series convergence
/// @return Computed value
static bdig log2(const bdig& n, const int iterations = 100)
{
    // Invalid value
    if (n <= 0)
        return 0;
    bdig result = 0, x = n, one = 1, two = 2;
    bdig b = "0.5";
    do {
        while (x < one) {
            x = x << 1; // x *= 2;
            result -= one;
        }
        while (x >= two) {
            x = x >> 1; // x /= 2;
            result += one;
        }
    } while (!(x >= one && x < two));

    for (int i = 0; i < iterations; i++) {
        x *= x;
        if (x >= two) {
            x = x >> 1; // x /= 2;
            result += b;
        }
        b = b >> 1; // b /= 2;
    }
    return result;
}
/// @brief Compute logarithm of current value by base passed in parameter
/// @param b base of logarithm
/// @param iterations iterations of calculating of series convergence
/// @return Computed value
bdig log(const bdig& b, const int iterations = 100) const
{
    return log(*this, b, iterations);
}
/// @brief Static method to compute logarithm of parameter by base also passed in parameter
/// @param a value to compute logarithm
/// @param b base of logarithm
/// @param iterations iterations of calculating of series convergence
/// @return Computed value
static bdig log(const bdig& a, const bdig& b, const int iterations = 100)
{
    return log2(a, iterations) / log2(b, iterations);
}
/// @brief Compute natural logarithm of current bdig type value.
/// @param iterations iterations of calculating of series convergence
/// @return Computed value of natural logarithm
bdig ln(const int iterations = 100) const
{
    return ln(*this, iterations);
}
/// @brief Static method to compute natural logarithm of passed bdig type value as parameter.
/// @param a bdig type value to compute natural logarithm.
/// @param iterations iterations of calculating of series convergence
/// @return Computed value
static bdig ln(const bdig& a, const int iterations = 100)
{
    return log2(a, iterations) / log2(pE_1000, iterations);
}
/// @brief Computes e (Euler's number) of current bdig type value.
/// @param iterations iterations of calculating of series convergence
/// @return Computed value of e (Euler's number)
bdig exp(const int iterations = prec * 2) const {
    return exp(*this, iterations);
}
/// @brief Computes e (Euler's number) of of passed bdig type value as parameter.
/// @param x bdig type value to compute e (Euler's number).
/// @param iterations iterations of calculating of series convergence
/// @return Computed value of e (Euler's number)
static bdig exp(const bdig& x, const int iterations = prec * 2)
{
    bdig result = 1, b = 1, d = 1, prev = 1;
    for (int i = 1; i < iterations; i++) {
        d *= i;
        b *= x;
        result = result + b / d;
        if (prev == result)
            break;
        prev = result;
    }
    return result;
}
/// @brief Compute power of bdig value by passed parameter using e Euler's number.
/// @param y value to power parameter
/// @param iterations iterations of calculating of series convergence
/// @return Computed value
bdig powe(const bdig& y, const int iterations = 100) const
{
    return powe(*this, y, iterations);
}
/// @brief Static function to compute power of bdig value by passed parameter using e Euler's number.
/// @param x value of bdig type to compute
/// @param y exponent value to calculate
/// @param iterations iterations of calculating of series convergence
/// @return Computed value
static bdig powe(const bdig& x, const bdig& y, const int iterations = 100)
{
    if (y == 0) {
        return {1};
    }
    if (!prec) {
        bdig tmp = y - 1, result = x;
        while (tmp > 0) {
            result *= x;
            tmp -= 1;
        }
        return result;
    }
    return exp(y * ln(x, iterations));
}
/// @brief Simple check primally of current bdig value. Use it only for bdig value with up to 30 digits
/// because it to slow for to big numbers. For bdig values with more then 30 digits more preferable checks
/// with fermatest method.
/// @return bool value of result. true - number is prime number otherwise false.
bool is_prime() const
{
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    static_assert(prec == 0, FOR_INTEGER_ONLY_TEXT);
#endif

    if (*this == 1 || *this == 2)
        return true;

    bdig max = sqrt() + 1;
    for (bdig i = 2; i <= max; i += 1)
        if (*this % i == 0)
            return false;
    return true;
}
/// @brief Iterative static method to calculate (base ^ exponent) % modulus
/// @param base base value of bdig type
/// @param exponent exponent value of bdig type
/// @param modulus modulus value of bdig type
/// @return Computed result value as bdig type.
static bdig modular_pow(const bdig& base, const bdig& exponent, const bdig& modulus)
{
    if (modulus == 1)
        return 0;
    bdig c = 1;
    for (bdig e_i = 0; e_i < exponent; e_i += 1)
        c = (c * base) % modulus;
    return c;
}
/// @brief Iterative static method to calculate (base ^ exponent) % modulus using bitwise method
/// @param base base value of bdig type
/// @param exponent exponent value of bdig type
/// @param modulus modulus value of bdig type
/// @return Computed result value as bdig type.
static bdig modular_pow2(bdig base, bdig exponent, const bdig& modulus)
{
    if (modulus == 1) {
        return 0;
    }
    CRTHEAPOBJ(v, 1);
    base %= modulus;
    while (!(!exponent)) {
        if (exponent.integer[isz - 1] & 1) {
            *v *= base;
            *v %= modulus;
        }
        exponent >>= 1;
        base *= base;
        base %= modulus;
    }
    return *v;
}
/// @brief Test current value of bdig type for possible primality.
/// @param iterations iterations for test
/// @return bool value of result. true - number is prime number otherwise false.
bool fermatest(const int iterations = 10) const
{
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    static_assert(prec == 0, FOR_INTEGER_ONLY_TEXT);
#endif
    if (*this <= 11) {
        return (
            *this == 1        ? true
                : *this == 2  ? true
                : *this == 3  ? true
                : *this == 5  ? true
                : *this == 7  ? true
                : *this == 11 ? true
                              : false);
    }
    bdig step = *this / (iterations + 1);

    for (int i = 0; i < iterations; i++) {
        bdig a = step + i;
        if (modular_pow2(a, *this - 1, *this) != 1)
            return false;
    }
    return true;
}
/// @brief Test value of bdig type for possible primality.
/// @param p value for test
/// @return bool value of result. true - number is prime number otherwise false.
static bool LucasLehmer(const bdig& p)
{
    if (p == 1 || p == 2)
        return true;

    CRTHEAPOBJ(tmp, 2);
    *tmp = tmp->powe(p) - 1;
    CRTHEAPOBJ(S, 2);
    {
        CRTHEAPOBJ(S1, 4);
        *S = *S1 % *tmp;
    }
    CRTHEAPOBJ(two, 2);
    int k = 1;
    while ((p - 1) > k) {
        *S = ((*S * *S) - *two) % *tmp;
        k++;
    }
    return (!*S);
}
////////////////////////////////////
// Unary operators

/// @brief Overload negation operator of current bdig value
/// @return modified value
bdig operator-() const
{
    bdig result = *this;
    result.is_negative = !is_negative;
    return result;
}
/// @brief Overload negation operator for current bdig type values.
/// @return negated bdig type value
bdig& operator-()
{
    is_negative = !is_negative;
    return *this;
}
/// @brief Overload not operator of current bdig value
/// @return bool value. true - if value is zero and false - if value non zero.
bool operator!() const
{
    if (most_significant_index() == isz - 1)
            return !integer[isz - 1];
    return false;
}

////////////////////////////////////
// Binary operators

/// @brief Overload addition operator for integer values.
/// @tparam Ti type of the integer value (int, long, long long)
/// @param v right value of operator
/// @return result of operation
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type operator+(const Ti v) const
{
    return *this + bdig(v);
}
/// @brief Overload addition operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig operator+(const bdig& v) const
{
    bdig tmp = *this;
    tmp += v;
    return tmp;
}
/// @brief Overload of addition assignment operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig& operator+=(const bdig& v)
{
    // a + -v = a - v positive
    // -a + v = v - a negative
    if (is_negative != v.is_negative) {
        CRTHEAPOBJNP(tmp);
        *tmp = is_negative ? v : *this;
        CRTHEAPOBJ(tmp2, is_negative ? *this : v);
        (*tmp2).is_negative = false;
        *tmp -= *tmp2;
        *this = *tmp;
        return *this;
    }

    bool c = false;
    std::size_t maxsi = v.most_significant_index();
    maxsi -= maxsi ? 1 : 0;
    for (std::size_t i = isz - 1;; i--) {
        if (i <= maxsi && !v.integer[i] && !c)
            break;
        c = sum_sop(i, v, c);
        if (!i)
            break;
    }
    return *this;
}

/// @brief Overload of subtraction operator for integer type values.
/// @tparam Ti type of the integer value (int, long, long long)
/// @param v right value of operator
/// @return result of operation
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type operator-(const Ti v) const
{
    return *this - bdig(v);
}
/// @brief Getter of negative flag of bdig value.
/// @return bool value, true - value is negative, false - value is positive.
bool isnegative() const
{
    return is_negative;
}
/// @brief Overload of subtraction operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig operator-(const bdig& v) const
{
    CRTHEAPOBJ(tmp, *this);
    *tmp -= v;
    return *tmp;
}
/// @brief Overload of subtraction assignment operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig& operator-=(const bdig& v)
{
    if ((!is_negative && !v.is_negative) || (is_negative && v.is_negative)) {
        if (*this < v) {
            // v - this
            CRTHEAPOBJ(tmp, v);
            *tmp -= *this;
            *this = *tmp;
            is_negative = !is_negative;
        } else {

            std::size_t msi_value = most_significant_index();

            // Substruct Value from Divider while Divider > Value
            for (std::size_t i = isz - 1; i >= msi_value; i--) {
                if ((*this).integer[i] < v.integer[i]) {
                    // Borrow value from higher limb. If value == 0 shit to next
                    std::size_t j = i - 1;
                    for (; !(*this).integer[j] && j >= msi_value; j--) {
                        (*this).integer.set(j, std::numeric_limits<T>::max());
                        if (!j)
                            break;
                    }
                    (*this).integer.set(j, (*this).integer[j] - 1);
                }
                (*this).integer.set(i, (*this).integer[i] - v.integer[i]);
                if (!i)
                    break;
            }
        }
    } else {
        *this += -v;
    }
    return *this;
}
/// @brief Overload of division operator for integer type values.
/// @tparam Ti type of right value (int, long, long long)
/// @param v right value of operator
/// @return result of operation
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type operator/(const Ti v) const
{
    return *this / bdig(v);
}
/// @brief Overload of division operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig operator/(const bdig& v) const
{
    bdig tmp = *this;
    tmp /= v;
    return tmp;
}
/// @brief Overload of division assignment operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig& operator/=(const bdig& v)
{
    if (v != 0) {
        if (v == *this) {
            *this = 1;
            is_negative = is_negative != v.is_negative;
        } else {
            *this = prec_up().div(v);
        }
    }
    return *this;
}
/// @brief Overload of remainder operator for integer type values.
/// @tparam Ti type of right value (int, long, long long)
/// @param v right value of operator
/// @return result of operation
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type operator%(const Ti v) const
{
    if (v == 2)
        return integer[isz - 1] % 2;
    if (most_significant_index() == isz - 1
        && std::numeric_limits<T>::digits >= std::numeric_limits<Ti>::digits)
        return integer[isz - 1] % v;
    return *this % bdig(v);
}
/// @brief Overload of remainder operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig operator%(const bdig& v) const
{
    CRTHEAPOBJ(ptmp, *this);
    *ptmp %= v;
    return *ptmp;
}
/// @brief Overload of remainder assignment operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig& operator%=(const bdig& v)
{
    return div(v, this);
}
/// @brief Overload of multiplication operator for integer type values.
/// @tparam Ti type of right value (int, long, long long)
/// @param v right value of operator
/// @return result of operation
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type operator*(const Ti v) const
{
    return *this * bdig(v);
}
/// @brief Overload of multiplication operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig operator*(const bdig& v) const
{
    bdig tmp = *this;
    tmp *= v;
    return tmp;
}
/// @brief Overload of multiplication assignment operator for bdig type values.
/// @param v right value of operator
/// @return result of operation
bdig& operator*=(const bdig& v)
{
    return mul<T>(v).prec_down();
}

////////////////////////////////////
// Shift operators

/// @brief Overload of bitwise right shift assignment operator for current bdig type values.
/// @param b number of bits to shift
/// @return result of operation
bdig& operator>>=(std::size_t b)
{
    shrb(b / std::numeric_limits<T>::digits);
    shr(b % std::numeric_limits<T>::digits);
    return *this;
}
/// @brief Overload of bitwise right shift operator for current bdig type values.
/// @param b right value of operator
/// @return result of operation
bdig operator>>(std::size_t b) const
{
    bdig tmp = *this;
    tmp >>= b;
    return tmp;
}
/// @brief Bitwise left shift assignment operator.
/// @param b number of bits to shift. The right values filled with zero.
/// @return bdig type shifted value.
bdig& operator<<=(const std::size_t b)
{
    shlb(b / std::numeric_limits<T>::digits);
    shl(b % std::numeric_limits<T>::digits);
    return *this;
}
/// @brief Bitwise left shift operator.
/// @param b number of bits to shift. The right values filled with zero.
/// @return bdig type shifted value.
bdig operator<<(const std::size_t b) const
{
    bdig tmp = *this;
    tmp <<= b;
    return tmp;
}

////////////////////////////////////
// Comparison operators

/// @brief More operator for bdig and integer.
/// @param v left side integer value
/// @return bool
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type operator>(const Ti v) const
{
    return *this > bdig(v);
}
/// @brief More operator for bdig instances.
/// @param v left side bdig value
/// @return bool
bool operator>(const bdig& v) const
{
    return _Cmp(*this, v, false, true);
}
/// @brief Less operator for bdig and integer.
/// @param v left side integer value
/// @return bool
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type operator<(const Ti v) const
{
    return *this < bdig(v);
}
/// @brief Less operator for bdig instances.
/// @param v left side bdig value
/// @return bool
bool operator<(const bdig& v) const
{
    return _Cmp(*this, v, true, false);
}
/// @brief Equivalent operator for bdig and integer.
/// @param v left side integer value
/// @return bool
template<class Ti>
const typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type operator==(
    const Ti v) const
{
    return *this == bdig(v);
}
/// @brief Equivalent operator for bdig instances.
/// @param v left side bdig value
/// @return bool
bool operator==(const bdig& v) const
{
    return _Cmp(*this, v, false, false);
}
/// @brief More or equivalent operator for bdig and integer.
/// @param v left side integer value
/// @return bool
template<class Ti>
const typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type operator>=(
    const Ti v) const
{
    bdig tmp = v;
    return *this == tmp || *this > tmp;
}
/// @brief More or equivalent operator for bdig instances.
/// @param v left side bdig value
/// @return bool
bool operator>=(const bdig& v) const
{
    return *this == v || *this > v;
}
/// @brief Less or equivalent operator for bdig and integer.
/// @param v left side integer value
/// @return bool
template<class Ti>
const typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type operator<=(
    const Ti v) const
{
    bdig tmp = v;
    return *this == tmp || *this < tmp;
}
/// @brief Less or equivalent operator for bdig instances.
/// @param v left side bdig value
/// @return bool
bool operator<=(const bdig& v) const
{
    return *this == v || *this < v;
}
/// @brief Not equivalent operator for bdig and integer.
/// @param v left side integer value
/// @return bool
template<class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type operator!=(const Ti v) const
{
    if (!v)
        return !(!(*this));
    return *this != bdig(v);
}
/// @brief Not equivalent operator for bdig instances.
/// @param v left side bdig value
/// @return bool
bool operator!=(const bdig& v) const
{
    return !(*this == v);
}

////////////////////////////////////
// Assignment operators

/// @brief Assignment operator of all types of signed integer (long,int,long long) to bdig value
/// @tparam Ti Type of the right value of operator.
/// @param v left side signed integer value
/// @return Returns reference to himself
template<class Ti>
typename enable_if<
    std::numeric_limits<Ti>::is_integer && std::numeric_limits<Ti>::is_signed,
    bdig>::type&
operator=(Ti v)
{
    if (v < 0)
        is_negative = true;

    // if char
    typedef typename conditional<
        is_same<Ti, char>::value,
        unsigned char,
        // else if short
        typename conditional<
            is_same<Ti, short>::value,
            unsigned short,
            // else if int
            typename conditional<
                is_same<Ti, int>::value,
                unsigned int,
                // else if long
                typename conditional<
                    is_same<Ti, long>::value,
                    unsigned long,
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                    // else if long long
                    typename conditional<
                        is_same<Ti, long long>::value,
                        unsigned long long,
#endif
                        void>::type>::type>::type
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
            >::type
#endif
        >::type uTi;

    uTi uv = static_cast<uTi>(v < 0 ? -v : v);
    return *this = uv;
}
/// @brief Assignment operator of all types of unsigned integer (long,int,long long) to bdig value
/// @tparam Ti Type of the right value of operator.
/// @param v left side unsigned integer value
/// @return Returns reference to himself
template<class Ti>
typename enable_if<
    std::numeric_limits<Ti>::is_integer && !std::numeric_limits<Ti>::is_signed,
    bdig>::type&
operator=(Ti v)
{
    integer.clear();
    const int Ti_digits
        = std::numeric_limits<Ti>::digits + (std::numeric_limits<Ti>::is_signed ? 1 : 0);
    const int locate_size
        = (Ti_digits >= std::numeric_limits<T>::digits
               ? Ti_digits / std::numeric_limits<T>::digits
               : std::numeric_limits<T>::digits / Ti_digits);
    std::size_t idx_to = isz > locate_size ? isz - locate_size : 0;
    for (std::size_t i = isz - 1; i >= idx_to; i--) {
        integer.set(i, v & std::numeric_limits<T>::max());
        v = v
            >> (std::numeric_limits<T>::digits < std::numeric_limits<Ti>::digits
                    ? std::numeric_limits<T>::digits - 1
                    : std::numeric_limits<Ti>::digits - 1);
        v = v >> 1;
        if (!i)
            break;
    }
    return this->prec_up();
}
/// @brief Assignment operator of (unsigned|signed) char* string to bdig value
/// @tparam Ti Type of the right value of operator.
/// @param v left side pointer to char string value
/// @return Returns reference to himself
template<class Ti>
typename enable_if<(is_same<unsigned char, Ti>::value || is_same<char, Ti>::value || is_same<signed char, Ti>::value),bdig>::type&
operator=(const Ti* v)
{
    *this = std::string{reinterpret_cast<const char*>(v)};
    return *this;
}
/// @brief Assignment operator of std::string to bdig value
/// @tparam Ti Type of the right value of operator.
/// @param str left side std::string value
/// @return Returns reference to himself
template<class Ti>
typename enable_if< (is_same<std::string, Ti>::value),bdig>::type&
operator=(const Ti& str)
{
    const char *pstr = str.data();
    if (pstr) {
        bdig tmp;
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
        if ( (*pstr == 'H' && pstr++) || (str.find("0x") == 0 && pstr++)) {
            std::regex expr("[0-9,A-F,a-f]{2}");
            std::smatch results;
            std::string val{pstr};
            int shift = 0;
            while (std::regex_search(val, results, expr)) {
                const uint8_t v = static_cast<uint8_t>(std::stoul(results[0], nullptr, 16));

                tmp.integer.set(isz - 1, (tmp.integer[isz - 1] << std::numeric_limits<uint8_t>::digits) + v);
                shift++;
                if (shift == std::numeric_limits<T>::digits / std::numeric_limits<uint8_t>::digits) {
                    shift = 0;
                    tmp.shlb(1);
                }
                // tmp.integer.set(isz - 1, v);
                val = results.suffix();
            }
            if (!shift)
                tmp.shrb(1);
            tmp.prec_up();

            if (tmp != 0) {
                *this = tmp;
                return *this;
            }
        }
#endif
        bdig tmp10;
        tmp10.integer.set(isz - 1, 10);
        int prc = prec;
        bool point = false;
        bool negative = false;
        if (*pstr == '-') {
            negative = true;
            pstr++;
        }
        if (*pstr == '+') {
            negative = false;
            pstr++;
        }
        bdig t;
        while (*pstr) {
            if (tmp != 0 && *pstr != '.' && *pstr != '\'')
                tmp = tmp.mul<T>(tmp10);

            if (0 <= *pstr - '0' && *pstr - '0' <= 9) {
                t.integer.set(isz - 1, *pstr - '0');
                tmp += t;
            } else if (*pstr == '.') {
                point = true;
                if (!prec)
                    break;
                prc++;
            } else if (*pstr == '\'') {
                // skip
            } else
                break;

            if (point)
                prc--;
            if (isz && !prc && prec)
                break;

            pstr++;
        }
        if (prc) {
            tmp = tmp.mul<T>(tmp10.pow(prc, false));
        }
        *this = tmp;
        is_negative = negative;
    }
    return *this;
}

/// @brief Operator of converting bdig object to string of decimal digits.
operator std::string() const
{
    std::string str(
        digits + prec
            + std::numeric_limits<
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                uint128_t
#else
                unsigned long
#endif
                >::digits10
            + 2,
        0);
    str = "";
    bdig tmp = *this;
    tmp.abs();
    bdig tmp10;
    tmp10.integer.set(isz - 1, 10);
    std::size_t d10 = std::numeric_limits<
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    unsigned long long
#else
    unsigned long
#endif
        >::digits10;
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    if (isz * sizeof(T) >= sizeof(uint128_t)) {
        const int uint128_digits10 = 38;
        d10 = std::numeric_limits<uint128_t>::digits10;
        if (!d10)
            d10 = uint128_digits10;
    }
    else if (isz * sizeof(T) >= sizeof(unsigned long long))
        d10 = std::numeric_limits<unsigned long long>::digits10;
    else
#endif
        if (isz * sizeof(T) >= sizeof(unsigned long))
        d10 = std::numeric_limits<unsigned long>::digits10;
    else if (isz * sizeof(T) >= sizeof(unsigned int))
        d10 = std::numeric_limits<unsigned int>::digits10;
    else if (isz * sizeof(T) >= sizeof(unsigned short))
        d10 = std::numeric_limits<unsigned short>::digits10;
    else if (isz * sizeof(T) >= sizeof(unsigned char))
        d10 = std::numeric_limits<unsigned char>::digits10;
    else
        d10 = 2;

#ifdef __SIZEOF_INT128__
    tmp10 = tmp10.pow((int)d10, false);
    uint128_t div = 1;
    d10 = std::numeric_limits<unsigned long long>::digits10;
    for (size_t i = 0; i < d10; i++)
        div *= 10;
#else
    d10--;
    tmp10 = tmp10.pow((int)d10, false);
#endif
    while (!(!tmp)) {
        bdig remainder = tmp;
        tmp = tmp.div(tmp10, &remainder);
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#ifndef __SIZEOF_INT128__
        unsigned long long c = remainder._toll<unsigned long long>();
        std::string d = std::to_string(c);
#else
        uint128_t c = remainder._toll<uint128_t>();
        std::vector<unsigned long long> vals;
        for (int i = 0; i < 2; i++) {
            vals.push_back(c % div);
            c /= div;
        }
        std::string d;
        std::for_each(vals.begin(), vals.end(), [&](auto& val){
          d = std::to_string(val);
#endif
#else
            unsigned long c = remainder._toll<unsigned long>();
            std::string d(d10 + 1, 0);
            sprintf(&d[0], "%lu", c);
            l d = d.data();
#endif
        std::size_t dl = d.length();
        if (dl < d10)
            d = std::string(d10 - dl, '0') + d;
        std::reverse(d.begin(), d.end());
        str += d;
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#ifdef __SIZEOF_INT128__
    });
#endif
#endif
    }
    if (str.length()) {
        size_t pos = str.find_last_not_of('0');
        if (pos != std::string::npos) {
            std::string::iterator sit = str.begin() + pos;
            if (sit != str.end())
                str.erase(sit + 1, str.end());
        }
    }
    std::reverse(str.begin(), str.end());
    std::string::size_type sz = str.length();
    if (sz <= prec)
        str = std::string(prec - sz + 1, '0') + str;
    if (prec){
        str.insert(str.end() - prec, '.');
        std::string::size_type pos = str.find_last_not_of("0", std::string::npos);
        if (pos != std::string::npos) {
            str.erase(str.begin() + pos + (str[pos] == '.' ? 0 : 1), str.end());
        }
    }
    if (is_negative)
        str.insert(str.begin(), '-');
    return str;
}

/// @brief Method for setting bit value to any position of value array.
/// @param number position of the bit. Started form 0 up to possible max position.
/// If position is out of range method do nothing.
/// @param val value of bit to set true or false. By default true.
void set_bit(int number, bool val = true)
{
    std::size_t elements = number / std::numeric_limits<T>::digits;
    if (elements >= isz)
        return;
    std::size_t pos = isz - elements - 1;
    T mask = ((T)1 << (number % std::numeric_limits<T>::digits));
    if (val) {
        integer.set(pos, integer[pos] | mask);
    } else {
        integer.set(pos, integer[pos] & ~mask);
    }
}

/// @brief Method for getting bit value from any position of value array.
/// @param number position of the bit. Started form 0 up to possible max position.
/// @return Value of the indicated bit or 0 if position is out of range.
int get_bit(std::size_t number) const
{
    std::size_t elements = number / std::numeric_limits<T>::digits;
    if (elements >= isz)
        return 0;
    std::size_t pos = isz - elements - 1;
    T mask = ((T)1 << number % std::numeric_limits<T>::digits);
    return ((integer[pos] & mask) ? 1 : 0);
}

/// @brief  Convert bdig to hexadecimal string
/// @return string of hexadecimal value of bdig object.
std::string hex() const {
    std::stringstream sin;
    bdig tmp = *this;
    tmp.prec_down();
    for (unsigned i = tmp.most_significant_index(); i < isz; i++) {
        sin << std::hex << static_cast<typename conditional<is_same<unsigned char, T>::value, int, T>::type >(tmp.integer[i]);
    }
    return sin.str();
}

/// @brief Stream operator for output of bdig into stream. Support std::hex and std::uppercase for hex flags.
/// @param out stream to output
/// @param rdig bdig type value to output
/// @return return reference to stream
friend std::ostream& operator<<(std::ostream& out, const bdig& rdig) {
    if (out.flags() & std::ios::hex) {
        std::string hex = rdig.hex();
        if (out.flags() & std::ios::uppercase)
            transform(hex.begin(), hex.end(), hex.begin(), ::toupper);
        out << hex;
    }
    else {
        out << static_cast<std::string>(rdig);
    }
    return out;
}

/// @brief Global addition operator for usage bdig from right side of integer types
/// @tparam Ti Integer type
/// @param v1 left integer parameter
/// @param v2 right bdig parameter
/// @return bdig value
template<class Ti>
friend typename sag::enable_if<
    std::numeric_limits<Ti>::is_integer /*|| std::numeric_limits<Ti>::is_iec559*/,
    bdig>::type
operator+(const Ti v1, bdig& v2)
{
    return (v2 + v1);
}
/// @brief Global subtraction operator for usage bdig from right side of integer types
/// @tparam Ti Integer type
/// @param v1 left integer parameter
/// @param v2 right bdig parameter
/// @return bdig value
template<class Ti>
friend typename sag::enable_if<
    std::numeric_limits<Ti>::is_integer /* || std::numeric_limits<Ti>::is_iec559*/,
    bdig>::type
operator-(const Ti v1, bdig& v2)
{
    return (bdig(v1) - v2);
}
/// @brief Global multiplication operator for usage bdig from right side of integer types
/// @tparam Ti Integer type
/// @param v1 left integer parameter
/// @param v2 right bdig parameter
/// @return bdig value
template<class Ti>
friend typename sag::enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type operator*(
    const Ti v1, bdig& v2)
{
    return (v2 * v1);
}
/// @brief Global division operator for usage bdig from right side of integer types
/// @tparam Ti Integer type
/// @param v1 left integer parameter
/// @param v2 right bdig parameter
/// @return bdig value
template<class Ti>
friend typename sag::enable_if<
    std::numeric_limits<Ti>::is_integer /* || std::numeric_limits<Ti>::is_iec559*/,
    bdig>::type
operator/(const Ti v1, bdig& v2)
{
    return (bdig(v1) / v2);
}
}; // class bdig

/// @brief Static member of point precision value for prec_up and prec_down methods which
/// makes bdig value integer and beck with point value.
/// @tparam T type of value array element
/// @tparam isz size of integer part of bdig in decimal digits
/// @tparam prec size of part after point of bdig in decimal digits
template<const int isz, const int prec, class T>
bdig<isz, prec, T> bdig<isz, prec, T>::i10;
} // namespace sag
