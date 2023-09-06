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

namespace sag {
const char *pPI_1000 = "3.14159265358979323846264338327950288419716939937510"
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
const char *pE_1000 = "2.71828182845904523536028747135266249775724709369995"
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
#define PREC_FOR_MATH_FUNC_TEXT                                                \
  "The precision should be more then " STR(PREC_FOR_MATH_FUNC) " digits"
#define SIZE_FOR_MATH_FUNC 300
#define SIZE_FOR_MATH_FUNC_TEXT                                                \
  "The size should be more then " STR(SIZE_FOR_MATH_FUNC) " digits"
#define FOR_INTEGER_ONLY_TEXT                                                  \
  "This function only for integer digits without precision"
#else // C++98
template <bool B, class T = void> struct enable_if {};
template <class T> struct enable_if<true, T> { typedef T type; };

struct true_type {
  enum { value = true };
};
struct false_type {
  enum { value = false };
};

template <class T, class U> struct is_same : false_type {};
template <class T> struct is_same<T, T> : true_type {};

template <bool B, class T, class F> struct conditional { typedef T type; };
template <class T, class F> struct conditional<false, T, F> { typedef F type; };

#endif
template <typename T> struct is_bdig {
  template <typename U> static char Test(typename U::bdig *);
  template <typename U> static int Test(...);
  static const bool value = (sizeof(Test<T>(0)) == sizeof(char));
};

/* Allocation objects in heap for functions with deep recursion   */
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#define DECLOBJ(name) std::unique_ptr<bdig> name
#define ASSIGNOBJ(name, param)                                                 \
  name.reset(new bdig(param)) // name = std::make_unique<bdig>(param)
#define CRTHEAPOBJ(name, param)                                                \
  DECLOBJ(name) = std::unique_ptr<bdig>(                                       \
      new bdig(param)) // DECLOBJ(name) = std::make_unique<bdig>(param)
#define CRTHEAPOBJNP(name) CRTHEAPOBJ(name, )
#else
#define DECLOBJ(name) std::auto_ptr<bdig> name
#define ASSIGNOBJ(name, param) name.reset(new bdig(param))
#define CRTHEAPOBJ(name, param) std::auto_ptr<bdig> name(new bdig(param))
#define CRTHEAPOBJNP(name) std::auto_ptr<bdig> name(new bdig())
#endif

template <const int digits = 100, const int prec = 0, class T = unsigned char>
class bdig {
  static const typename enable_if<!std::numeric_limits<T>::is_signed,
                                  unsigned int>::type isz =
      ((prec * 2 + digits) / (std::numeric_limits<T>::digits * 301 / 1000)) + 1;

  template <class Tb, unsigned int size = isz> class buffer {
    Tb _buffer[size];
    unsigned int least_significant_index;

  public:
    buffer() { clear(); }
    void clear() {
      memset(_buffer, 0, size * sizeof(Tb));
      least_significant_index = size - 1;
    }
    const T &operator[](const unsigned idx) const {
      return _buffer[idx];
    }
    inline void set(const unsigned idx, const Tb _v) {
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
    inline void copy(int to_idx, const Tb *from, int sz) {
      for (int i = 0; i < sz; i++)
        set(to_idx++, from[i]);
    }
    inline unsigned int get_lsi() const { return least_significant_index; }
  };

  buffer<T> integer;
  bool is_negative;
  static bdig i10;
  const static T mask_max_bit = (T)1 << (std::numeric_limits<T>::digits - 1);

  void shlb(int bytes) // Shift to bytes counter
  {
    if (!bytes)
      return;
    int idx_src = integer.get_lsi();
    int idx_dst = integer.get_lsi() - bytes;
    if (idx_dst < 0)
      return; // UB idx_dst = 0;

    int move_size = isz - integer.get_lsi();
    for (int i = 0; i < move_size; i++, idx_dst++, idx_src++)
      integer.set(idx_dst, integer[idx_src]);

    for (int i = 0, idx_set = (isz - bytes); i < bytes; i++, idx_set++)
      integer.set(idx_set, 0);
  }

  inline T shl_sop(const int idx, const int bits, const T set_bit) {
    if (!integer[idx] && !set_bit)
      return 0;
    T bit = integer[idx] >> (std::numeric_limits<T>::digits - bits);
    integer.set(idx, integer[idx] << bits);
    if (set_bit)
      integer.set(idx, integer[idx] | set_bit);
    return bit;
  }
  void shl(const int bits = 1) {
    if (!bits)
      return;
    int msi = most_significant_index() - 1;
    msi = msi < 0 ? 0 : msi;

    T set_bit = 0;
    for (int i = isz - 1; i >= msi; i--) {
      set_bit = shl_sop(i, bits, set_bit);
    }
  }
  void shrb(const int bytes) {
    for (int i = 0; i < (int)isz - bytes; i++)
      integer.set(isz - i - 1, integer[isz - bytes - i - 1]);
    for (int i = 0; i < bytes; i++)
      integer.set(i, 0);
  }
  inline T shr_sop(const int idx, const int bits, const T set_bit) {
    if (!integer[idx] && !set_bit)
      return 0;
    T bit = integer[idx] << (std::numeric_limits<T>::digits - bits);
    integer.set(idx, integer[idx] >> bits);
    if (set_bit)
      integer.set(idx, integer[idx] | set_bit);
    return bit;
  }
  void shr(const unsigned bits = 1) {
    T set_bit = 0;

    unsigned msi = most_significant_index();
    // msi = std::max (msi, 0);

    for (unsigned i = msi; i < isz; i++) {
      set_bit = shr_sop(i, bits, set_bit);
    }
  }
  inline int most_significant_index() const { return integer.get_lsi(); }

  template <class Ti>
  int most_significant_bit(
      Ti value, typename enable_if<is_same<Ti, unsigned short>::value ||
                                       is_same<Ti, unsigned long>::value
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
          || is_same<Ti, unsigned int>::value || is_same<Ti, unsigned long long>::value || is_same<Ti, uint128_t>::value
#endif
          ,Ti>::type = 0) const {
typedef
    typename conditional<is_same<Ti, unsigned short>::value, unsigned char,
      typename conditional<is_same<Ti, unsigned long>::value, unsigned short,
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
        typename conditional<is_same_v<Ti, unsigned int>, unsigned short,
          typename conditional<is_same_v<Ti, unsigned long long>,
            typename conditional<numeric_limits<Ti>::digits == numeric_limits<unsigned long>::digits, unsigned int, unsigned long>::type,
              typename conditional<is_same_v<Ti, uint128_t>, unsigned long long,
#endif
      void>::type
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
      >::type>::type>::type
#endif
        >::type subT;

    if (value & (Ti)std::numeric_limits<subT>::max()
                    << std::numeric_limits<subT>::digits)
      return most_significant_bit(
                 (subT)(value >> std::numeric_limits<subT>::digits)) +
             std::numeric_limits<subT>::digits;
    return most_significant_bit((subT)value);
  }
  template <class Ti>
  int most_significant_bit(
      Ti value, typename enable_if<is_same<Ti, unsigned char>::value &&
                                       std::numeric_limits<Ti>::digits == 8,
                                   T>::type = 0) const {
    if (value & (Ti)0xF0) {
      if (value & (Ti)0xC0)
        return (value & 0x80) ? 7 : 6;
      return (value & 0x20) ? 5 : 4;
    }
    if (value & (Ti)0x0C)
      return (value & 0x08) ? 3 : 2;
    return (value & 0x02) ? 1 : 0;
  }
  int least_significant_bit(T value) {
    if (!value)
      return std::numeric_limits<T>::digits - 1;

    int result = 0;
    T mask = 1;
    while (!(value & mask)) {
      mask <<= 1;
      result++;
    }
    return result;
  }
  bdig &copy(const bdig &src, int bit_most, int bit_least) {
    int msi = most_significant_index();
    int src_msi = src.most_significant_index();
    if (src_msi < msi)
      return *this;
    int most_idx = bit_most / std::numeric_limits<T>::digits +
                   ((bit_most % std::numeric_limits<T>::digits) ? 1 : 0);
    int least_idx = bit_least / std::numeric_limits<T>::digits +
                    ((bit_least % std::numeric_limits<T>::digits) ? 1 : 0);
    if (least_idx)
      least_idx = 1;

    integer.clear();
    for (int i = isz - (most_idx - least_idx + 1), j = 0; i < isz; i++, j++)
      integer.set(i, src.integer[isz - most_idx + j]);

    int mask = std::numeric_limits<T>::max() >>
               (std::numeric_limits<T>::digits -
                (bit_most + 1) % std::numeric_limits<T>::digits);
    integer.set(isz - most_idx, integer[isz - most_idx] & mask);

    this->shr(bit_least % std::numeric_limits<T>::digits);
    return *this;
  }
    
  bdig &div(const bdig &v, bdig *premainder = NULL) {

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
    unsigned ri = (*remainder).most_significant_index();
    unsigned di = (*divisor).most_significant_index();
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

      int msi_divisor = (*divisor).most_significant_index();
      int msi_shift = (*shift).most_significant_index();
      int msi = std::min(msi_shift, msi_divisor);
      msi -= 1;
      msi = std::max(msi, 0);

      if (bits) {
        T set_bit = 0, set_bit1 = 0;
        if (bits > 0) {
          for (int i = isz - 1; i >= msi; i--) {
            set_bit = (*divisor).shl_sop(i, bits, set_bit);
            set_bit1 = (*shift).shl_sop(i, bits, set_bit1);
          }
        } else {
          for (unsigned i = msi; i < isz; ++i) {
            set_bit = (*divisor).shr_sop(i, -bits, set_bit);
            set_bit1 = (*shift).shr_sop(i, -bits, set_bit1);
          }
        }
      }
      while ((*divisor) > (*remainder)) {
        T set_bit = 0, set_bit1 = 0;
        for (unsigned i = msi; i < isz; i++) {
          set_bit = (*divisor).shr_sop(i, 1, set_bit);
          set_bit1 = (*shift).shr_sop(i, 1, set_bit1);
        }
      }

      int msi_quotient = most_significant_index();
      int msi_remainder = (*remainder).most_significant_index();
      msi = std::min(msi_remainder, msi_quotient);
      msi -= 1;
      msi = msi < 0 ? 0 : msi;

      bool cs = false, csm = false;
      for (int i = isz - 1; i >= msi; --i) {
        csm = sum_sop(i, *shift, csm);
        cs = (*remainder).sub_sop(i, *divisor, cs);
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

  bdig &prec_value() {
    if (!i10) {
      i10.integer.set(isz - 1, (prec ? 10 : 1));
      i10 = i10.pow(prec, false);
    }
    return i10;
  }

  bdig &prec_up() {
    if (prec) {
      return mul<T>(prec_value());
    }
    return *this;
  }
  bdig &prec_down() {
    if (prec)
      return div(prec_value());
    return *this;
  }

typedef typename conditional<is_same<T, unsigned char>::value 
        && std::numeric_limits<unsigned short>::digits / std::numeric_limits<T>::digits == 2 , unsigned short,
        typename conditional<is_same<T, unsigned short>::value 
        && std::numeric_limits<unsigned int>::digits / std::numeric_limits<T>::digits == 2, unsigned int,
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
        typename conditional<is_same<T, unsigned int>::value 
        && std::numeric_limits<unsigned long>::digits / std::numeric_limits<T>::digits == 2, unsigned long, 
        unsigned long long>::type
#else
      unsigned long>::type
#endif
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
          >::type
#endif
      >::type upT;

  template <class Ti> void add(int index, bdig &result, Ti value) {
    if (index < 0) // whole overflow
    {
        // TODO set overfolow value
        return;
    }

    upT x = result.integer[index];
    upT y = value;
    upT d = x + y;
    // upT d = result.integer[index] + value;
    Ti r1 = d >> std::numeric_limits<Ti>::digits;
    Ti r0 = d & std::numeric_limits<Ti>::max();

    if (r1) // overflow
    {
      // add  high part to high order limb
      add(index - 1, result, r1);
    }
    // Set low order part to current limb
    result.integer.set(index, r0);
  }
  template <class Ti>
  bdig &mul(const bdig &v,
            typename enable_if<
            is_same<Ti, unsigned char>::value ||
            is_same<Ti, unsigned short>::value ||
            (is_same<Ti, unsigned int>::value 
             && std::numeric_limits<Ti>::digits != std::numeric_limits<unsigned long>::digits
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
             && std::numeric_limits<Ti>::digits != std::numeric_limits<unsigned long long>::digits
#endif
                                ),
                               Ti>::type = 0) {
    bdig result;
    for (int v_i = isz - 1; v_i >= v.most_significant_index(); v_i--) {
      if (!v.integer[v_i])
        continue;

      for (int idx = isz - 1; idx >= most_significant_index(); idx--) {
        if (!integer[idx])
          continue;

        upT x = v.integer[v_i];
        upT y = integer[idx];
        upT d = x * y;
        Ti r1 = d >> std::numeric_limits<Ti>::digits;
        Ti r0 = d & std::numeric_limits<Ti>::max();
        int set_index = idx - (isz - v_i);
        add(set_index, result, r1);
        add(set_index + 1, result, r0);
      }
    }
    bool store_is_negative = is_negative;
    bool store_is_negative_v = v.is_negative;
    *this = result;
    is_negative = store_is_negative != store_is_negative_v;
    return *this;
  }
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
  template <class Ti>
  bdig &
  mul(const bdig &v,
      typename enable_if<is_same<Ti, unsigned long long>::value || is_same<Ti, uint128_t>::value ||
      (is_same<Ti, unsigned int>::value && std::numeric_limits<Ti>::digits == std::numeric_limits<unsigned long>::digits), Ti>::type =
          0) {
    CRTHEAPOBJ(multiplicant, v);
    *multiplicant = (*multiplicant).abs();

    CRTHEAPOBJ(sum, *this);
    if (sum->isnegative())
      *sum = -(*sum);
    bool store_is_negative = is_negative;
    if (is_negative)
      is_negative = false;
    integer.clear();
    int shift = 0;
    for (int i = (*multiplicant).most_significant_index(); i < isz; i++) {
      for (int b = 0; b < std::numeric_limits<T>::digits; b++) {
        if ((*multiplicant).integer[isz - 1] & (Ti)1) {
          (*sum).shlb(shift / std::numeric_limits<Ti>::digits);

          shift %= std::numeric_limits<T>::digits;
          Ti set_bit = 0;
          bool c = false;
          int msi_sum = (*sum).most_significant_index();
          int msi = most_significant_index();
          msi = std::min(msi, msi_sum);
          msi -= 1;
          msi = std::max(msi, 0);

          for (int j = isz - 1; j >= msi; j--) {
            if (shift)
              set_bit = (*sum).shl_sop(j, shift, set_bit);
            c = sum_sop(j, *sum, c);
          }
          shift = 0;
        }
        (*multiplicant)
            .integer.set(isz - 1, (*multiplicant).integer[isz - 1] >> 1);
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
  bool _Cmp(const bdig &v1, const bdig &v2, bool r1, bool r2) const {

    unsigned lsi1 = v1.integer.get_lsi();
    unsigned lsi2 = v2.integer.get_lsi();

    int res = 0;
    bool zero = true;

    if (lsi1 == lsi2) {
      for (unsigned i = lsi1; i < isz; ++i) {
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
                 r2 ? v2.is_negative :
                    // v1 == v2
                     zero ? true : false;

    // int res1 = memcmp (v1.integer, v2.integer, sizeof (v1.integer)); // Can
    // work only with big-endian
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
  template <class Ti>
  Ti _toll(
      typename enable_if</*std::numeric_limits<Ti>::is_integer &&*/ !is_same<Ti,T>::value, Ti>::type = 0) {
    Ti result = 0;
    for (unsigned i = isz - numeric_limits<Ti>::digits/numeric_limits<T>::digits; i < isz; i++) {
      result += integer[i];
      if (i < isz - 1)
        result <<= std::numeric_limits<T>::digits; // UB if Ti == T !!!
    }
    return result;
  }
  template <class Ti>
  Ti _toll(typename enable_if</*numeric_limits<Ti>::is_integer &&*/ is_same<Ti,T>::value, Ti>::type = 0) {
    return integer[isz - 1];
  }
  inline bool sum_sop(const int idx, const bdig &v, bool c) {
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
  inline bool sub_sop(T *p_integer, const T *p_v_integer, const bool c) {
    if (!*p_v_integer && !c)
      return c;
    bool c2 = false;
    if (c) {
      if (*p_integer == 0)
        c2 = true;
      *p_integer -= 1;
    }
    bool cr = (*p_integer < *p_v_integer) || c2;
    *p_integer -= *p_v_integer;
    return cr;
  }
  bdig &_pow(const int _s, bool with_prec = true) {
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
  const static int size =
      (prec * 2 / (std::numeric_limits<T>::digits * 301 / 1000)) + 1;

  ////////////////////////////////////
  // Constructors
  bdig() { is_negative = false; }
  template <class Ti>
  bdig(Ti v, typename enable_if<std::numeric_limits<Ti>::is_integer &&
                                    !std::numeric_limits<T>::is_signed,
                                Ti>::type = 0)
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
      : bdig() {
#else
  {
    // memset (integer, 0, sizeof (integer));
    is_negative = false;
#endif
    *this = v;
  }
  template <class Ti>
  bdig(const Ti *v, typename enable_if<(is_same<unsigned char, Ti>::value ||
                                        is_same<char, Ti>::value ||
                                        is_same<signed char, Ti>::value) &&
                                           !std::numeric_limits<T>::is_signed,
                                       Ti>::type = 0)
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
      : bdig(){
#else
  {
    // memset (integer, 0, sizeof (integer));
    is_negative = false;
#endif
            *this = v;
}

////////////////////////////////////
// Math methods
bdig &
abs() {
  is_negative = false;
  return *this;
}
bdig &pow(const int _s, bool with_prec = true) {
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
bdig sqrt() const {
  // Invalid value for sqrt
  if (is_negative)
    return 0;

  bdig res;
  bdig v = *this;
  v = v.prec_up();
  bdig bit;
  // bit = bit.prec_down();
  if (!v.most_significant_index()) {
    int msb = v.most_significant_bit(v.integer[0]);
    if (msb + 1 == std::numeric_limits<T>::digits - 1){
      // Overflow
      return res;
    }
    bit.set_bit(0, false);
    bit.set_bit((msb/2 + 1) * 2 + ((isz - 1) * std::numeric_limits<T>::digits));
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
bdig sin(int a, const int iterations = 140) const {
  if (!(a % 180))
    return 0;
  return -cos(a + 90, iterations);
}
bdig cos(int a, const int iterations = 140) const {
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
  static_assert(prec >= PREC_FOR_MATH_FUNC, PREC_FOR_MATH_FUNC_TEXT);
#endif
  if (a == 180)
    return -1;
  bdig bPI = pPI_1000;
  bdig x = a > 180 ? a % 180 : a;

  x *= bPI / 180;
  bdig t = 1;
  bdig sum = 1;

  for (int i = 1; i <= iterations; i++) {
    t = -t * x * x / (2 * i * (2 * i - 1));
    sum += t;
  }
  return ((a / 180 % 2) ? -sum : sum);
}
bdig log2(const bdig &n, const int iterations = 100) const {
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
bdig log(const bdig &a, const bdig &b, const int iterations = 100) const {
  return log2(a, iterations) / log2(b, iterations);
}
bdig ln(const bdig &a, const int iterations = 100) const {
  return log2(a, iterations) / log2(pE_1000, iterations);
}
bdig exp(const bdig &x, const int iterations = prec * 2) const {
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
  static_assert(digits >= SIZE_FOR_MATH_FUNC, SIZE_FOR_MATH_FUNC_TEXT);
#endif

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
bdig powe(const bdig &x, const bdig &y, const int iterations = 100) const {
  if (y == 0) {
    bdig tmp = 1;
    return tmp;
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
bool is_prime() const {
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
bdig modular_pow(const bdig &base, const bdig &exponent,
                 const bdig &modulus) const {
  if (modulus == 1)
    return 0;
  bdig c = 1;
  for (bdig e_i = 0; e_i < exponent; e_i += 1)
    c = (c * base) % modulus;
  return c;
}
bdig modular_pow2(bdig base, bdig exponent, const bdig modulus) const {
  CRTHEAPOBJ(v, *this);
  if (modulus == 1) {
    return 0;
  }
  *v = 1;
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
bool fermatest(const int iterations = 10) const {
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
  static_assert(prec == 0, FOR_INTEGER_ONLY_TEXT);
#endif
  if (*this <= 11) {
    return (*this == 1
                ? true
                : *this == 2
                      ? true
                      : *this == 3
                            ? true
                            : *this == 5
                                  ? true
                                  : *this == 7 ? true
                                               : *this == 11 ? true : false);
  }
  bdig step = *this / (iterations + 1);

  for (int i = 0; i < iterations; i++) {
    bdig a = step + i;
    if (modular_pow2(a, *this - 1, *this) != 1)
      return false;
  }
  return true;
}
bool LucasLehmer(int p) {
  if (p == 1 || p == 2)
    return true;

  *this = 2;
  *this = pow(p) - 1;
  CRTHEAPOBJ(S, 2);
  {
    CRTHEAPOBJ(S1, 4);
    *S = *S1 % *this;
  }
  CRTHEAPOBJ(two, 2);
  int k = 1;
  while (k < p - 1) {
    *S = ((*S * *S) - *two) % *this;
    k++;
  }
  return (!*S);
}
bdig Karatsuba(const bdig &x, const bdig &y) const {
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
  static_assert(prec == 0, FOR_INTEGER_ONLY_TEXT);
#endif

  int sz_x = isz - x.most_significant_index();
  int sz_y = isz - y.most_significant_index();
  int sz = std::max(sz_x, sz_y);

  if (sz == 1)
    return x * y;

  int hi = sz / 2;
  int lo = sz - hi;

  CRTHEAPOBJNP(a0);
  CRTHEAPOBJNP(a1);
  (*a0).integer.copy(isz - hi, &x.integer[isz - sz], hi);
  (*a1).integer.copy(isz - lo, &x.integer[isz - lo], lo);
  CRTHEAPOBJNP(b0);
  CRTHEAPOBJNP(b1);
  (*b0).integer.copy(isz - hi, &y.integer[isz - sz], hi);
  (*b1).integer.copy(isz - lo, &y.integer[isz - lo], lo);

  CRTHEAPOBJNP(c0);
  CRTHEAPOBJNP(c1);
  CRTHEAPOBJNP(c2);
  *c0 = Karatsuba(*a0, *b0); // sz / 2 + sz % 2);
  *c1 = Karatsuba(*a1, *b1); // sz / 2 + sz % 2);
  *c2 = Karatsuba(*a0 + *a1, *b0 + *b1) - (*c0 + *c1);

  (*c0) <<= std::numeric_limits<T>::digits * lo;
  *c0 += *c2;
  (*c0) <<= std::numeric_limits<T>::digits * lo;
  *c0 += *c1;
  return *c0;
}
////////////////////////////////////
// Unary operators
bdig operator-() const {
  bdig result = *this;
  result.is_negative = !is_negative;
  return result;
}
bool operator!() const {
  for (int i = isz - 1; i >= 0; --i)
    if (integer[i])
      return false;
  return true;
}

////////////////////////////////////
// Binary operators
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type
operator+(const Ti v) const {
  return *this + bdig(v);
}
bdig operator+(const bdig &v) const {
  bdig tmp = *this;
  tmp += v;
  return tmp;
}
bdig &operator+=(const bdig &v) {
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
  int maxsi = v.most_significant_index();
  maxsi--;
  maxsi = maxsi < 0 ? 0 : maxsi;
  for (int i = isz - 1; i >= 0; i--) {
    if (i <= maxsi && !v.integer[i] && !c)
      break;
    c = sum_sop(i, v, c);
  }
  return *this;
}
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type
operator-(const Ti v) const {
  return *this - bdig(v);
}
bool isnegative() const { return is_negative; }
bdig operator-(const bdig &v) const {
  CRTHEAPOBJ(tmp, *this);
  *tmp -= v;
  return *tmp;
}
inline bool sub_sop(const int idx, const bdig &v, const bool c) {
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
bdig &operator-=(const bdig &v) {
  if ((!is_negative && !v.is_negative) || (is_negative && v.is_negative)) {
    if (*this < v) {
      // v - this
      CRTHEAPOBJ(tmp, v);
      *tmp -= *this;
      *this = *tmp;
      is_negative = !is_negative;
    } else {
      // this - v
      bool c = false;
      int maxsi = v.most_significant_index();
      maxsi--;
      maxsi = maxsi < 0 ? 0 : maxsi;
      for (int i = isz - 1; i >= 0; --i) {
        if (i <= maxsi && !v.integer[i] && !c)
          break;
        c = sub_sop(i, v, c);
      }
    }
  } else {
    *this += -v;
  }
  return *this;
}
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type
operator/(const Ti v) const {
  return *this / bdig(v);
}
bdig operator/(const bdig &v) const {
  bdig tmp = *this;
  tmp /= v;
  return tmp;
}
bdig &operator/=(const bdig &v) {
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
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type
operator%(const Ti v) const {
  if (v == 2)
    return integer[isz - 1] % 2;
  if (most_significant_index() == isz - 1 &&
      std::numeric_limits<T>::digits >= std::numeric_limits<Ti>::digits)
    return integer[isz - 1] % v;
  return *this % bdig(v);
}
bdig operator%(const bdig &v) const {
  CRTHEAPOBJ(ptmp, *this);
  *ptmp %= v;
  return *ptmp;
}
bdig &operator%=(const bdig &v) { return div(v, this); }
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type
operator*(const Ti v) const {
  return *this * bdig(v);
}
bdig operator*(const bdig &v) const {
  bdig tmp = *this;
  tmp *= v;
  return tmp;
}
bdig &operator*=(const bdig &v) { return mul<T>(v).prec_down(); }

////////////////////////////////////
// Shift operators
bdig &operator>>=(int b) {
  shrb(b / std::numeric_limits<T>::digits);
  shr(b % std::numeric_limits<T>::digits);
  return *this;
}
bdig operator>>(int b) const {
  bdig tmp = *this;
  tmp >>= b;
  return tmp;
}
bdig &operator<<=(const int b) {
  shlb(b / std::numeric_limits<T>::digits);
  shl(b % std::numeric_limits<T>::digits);
  return *this;
}
bdig operator<<(const int b) const {
  bdig tmp = *this;
  tmp <<= b;
  return tmp;
}

////////////////////////////////////
// Comparison operators
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type
operator>(const Ti v) const {
  return *this > bdig(v);
}
bool operator>(const bdig &v) const { return _Cmp(*this, v, false, true); }
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type
operator<(const Ti v) const {
  return *this < bdig(v);
}
bool operator<(const bdig &v) const { return _Cmp(*this, v, true, false); }
template <class Ti>
const typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type
operator==(const Ti v) const {
  return *this == bdig(v);
}
bool operator==(const bdig &v) const { return _Cmp(*this, v, false, false); }
template <class Ti>
const typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type
operator>=(const Ti v) const {
  bdig tmp = v;
  return *this == tmp || *this > tmp;
}
bool operator>=(const bdig &v) const { return *this == v || *this > v; }
template <class Ti>
const typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type
operator<=(const Ti v) const {
  bdig tmp = v;
  return *this == tmp || *this < tmp;
}
bool operator<=(const bdig &v) const { return *this == v || *this < v; }
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer, bool>::type
operator!=(const Ti v) const {
  if (!v)
    return !(!(*this));
  return *this != bdig(v);
}
bool operator!=(const bdig &v) const { return !(*this == v); }

////////////////////////////////////
// Assignment operators
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer &&
                       std::numeric_limits<Ti>::is_signed,
                   bdig>::type &
operator=(Ti v) {
  if (v < 0)
    is_negative = true;

  // if char
  typedef typename conditional<
      is_same<Ti, char>::value, unsigned char,
      // else if short
      typename conditional<
          is_same<Ti, short>::value, unsigned short,
          // else if int
          typename conditional<
              is_same<Ti, int>::value, unsigned int,
              // else if long
              typename conditional<
                  is_same<Ti, long>::value, unsigned long,
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                  // else if long long
                  typename conditional<is_same<Ti, long long>::value,
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
template <class Ti>
typename enable_if<std::numeric_limits<Ti>::is_integer &&
                       !std::numeric_limits<Ti>::is_signed,
                   bdig>::type &
operator=(Ti v) {
  integer.clear();
  const int Ti_digits = std::numeric_limits<Ti>::digits +
                        (std::numeric_limits<Ti>::is_signed ? 1 : 0);
  const int locate_size = (Ti_digits >= std::numeric_limits<T>::digits
                               ? Ti_digits / std::numeric_limits<T>::digits
                               : std::numeric_limits<T>::digits / Ti_digits);
  int idx_to = isz > locate_size ? isz - locate_size : 0;
  for (int i = isz - 1; i >= idx_to; i--) {
    integer.set(i, v & (T)-1);
    v = v >> (std::numeric_limits<T>::digits < std::numeric_limits<Ti>::digits
                  ? std::numeric_limits<T>::digits - 1
                  : std::numeric_limits<Ti>::digits - 1);
    v = v >> 1;
  }
  return this->prec_up();
}
bdig &operator=(const char *pstr) {
  if (pstr) {
    bdig tmp, tmp10 = 10;
    tmp10 = tmp10.prec_down();
    int prc = prec;
    bool point = false;
    bool negative = false;
    if (*pstr == '-') {
      negative = true;
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
    tmp = tmp.mul<T>(tmp10.pow(prc, false));
    *this = tmp;
    is_negative = negative;
  }
  return *this;
}

operator std::string() const {
  std::string str(digits + prec +
                      std::numeric_limits<
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
                            uint128_t
                          //unsigned long long
#else
                          unsigned long
#endif
                          >::digits10 +
                      2,
                  0);
  str = "";
  bdig tmp = *this;
  tmp.abs();
  bdig tmp10 = 10;
  std::size_t d10 = std::numeric_limits<
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
      uint128_t
      //unsigned long long
#else
      unsigned long
#endif
      >::digits10;
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
  if (isz * sizeof(T) >= sizeof(uint128_t))
    d10 = std::numeric_limits<uint128_t>::digits10;
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
  tmp10 = tmp10.prec_down();
  tmp10 = tmp10.pow((int)d10, false);
  // int prc = prec;
  while (!(!tmp)) {
    bdig remainder;
    tmp = tmp.div(tmp10, &remainder);
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
#ifndef __SIZEOF_INT128__
     unsigned long long c = remainder._toll<unsigned long long>();
     std::string d = std::to_string(c);
#else
    uint128_t div = 1;
    d10 = std::numeric_limits<unsigned long long>::digits10;
    for (int i = 0; i < d10; i++)
      div *= 10;
    uint128_t c = remainder._toll<uint128_t>();
    std::vector<unsigned long long> vals;
    for (int i = 0; i < 2 || c; i++) {
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
      sprintf(&d[0], "%lu", c);l
      d = d.data();
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
  std::size_t size = str.length();
  if (size <= prec)
    str = std::string(prec - size + 1, '0') + str;
  if (prec)
    str.insert(str.end() - prec, '.');
  if (is_negative)
    str.insert(str.begin(), '-');
  return str;
}
void set_bit(int number, bool val = true) {
  int pos = isz - (number / std::numeric_limits<T>::digits) - 1;
  T mask = ((T)1 << (number % std::numeric_limits<T>::digits));
  if (val) {
    integer.set(pos, integer[pos] | mask);
  } else {
    integer.set(pos, integer[pos] & ~mask);
  }
}
int get_bit(int number) const {
  int pos = isz - (number / std::numeric_limits<T>::digits) - 1;
  T mask = ((T)1 << number % std::numeric_limits<T>::digits);
  return ((integer[pos] & mask) ? 1 : 0);
}

#ifdef TEST_SUPPORT_FUNC
////////////////////////////////////
// Test support fuction

int contains_digits() {
  int msd = most_significant_index();
  int bits = (isz - msd - 1) * 8;
  unsigned char b = integer[msd];
  int bit = 0;
  for (int i = 0; i < 8; i++) {
    if (b & 1)
      bit = i + 1;
    b >>= 1;
  }
  bits += bit;
  return (int)(bits * 301 / 1000) + 1;
}
friend std::ostream &operator<<(std::ostream &output, const bdig &rhs) {
  std::vector<std::string> data;
  int digits_cnt = 1000; // std::numeric_limits <long long>::digits10
  bdig tmp10 = 10;
  tmp10 = tmp10.pow(digits_cnt, false);
  bdig tmp = rhs.abs();
  while (!(!tmp)) {
    bdig remainder;
    tmp = tmp.div(tmp10, &remainder);
    std::string d = remainder;
    if (d.length() < digits_cnt)
      d = std::string(digits_cnt - d.length(), '0') + d;
    std::cout << d << std::endl;
    data.push_back(d);
  }
  for (std::vector<std::string>::reverse_iterator it = data.rbegin();
       it != data.rend(); ++it)
    output << *it << std::endl;

  // int msi = rhs.most_significant_index();
  // output.write ((std::ostream::char_type *)&rhs.integer[msi], (isz -
msi) * sizeof(T) / sizeof (std::ostream::char_type));
return output;
}
void store(const char *name) {
  std::ofstream out;
  out.exceptions(std::ios::failbit);
  try {
    out.open(name, out.binary);
    out.write((std::ostream::char_type *)&integer[0],
              isz * sizeof(T) / sizeof(std::ostream::char_type));
    out.close();
  } catch (std::ios_base::failure f) {
    std::cout << "Caught an exception: " << f.what() << std::endl;
  }
}
void load(const char *name) {
  std::ifstream in;
  in.exceptions(std::ios::failbit);
  try {
    in.open(name, in.binary);
    in.seekg(-(isz * sizeof(T) / sizeof(std::ifstream::char_type)), in.end);
    std::streamoff pos = in.tellg();
    in.read((std::ifstream::char_type *)integer,
            isz * sizeof(T) / sizeof(std::ifstream::char_type));
    in.close();
  } catch (std::ios_base::failure f) {
    std::cout << "Caught an exception: " << f.what()
              << "char_type:" << sizeof(std::ifstream::char_type) << std::endl;
    std::cout << "offset:"
              << (int)(isz * sizeof(T) / sizeof(std::ifstream::char_type))
              << std::endl;
    // cout << " pos:" << (std::streamoff)in.tellg() << endl;
  }
}
#endif

template <class Ti>
friend typename sag::enable_if<
    std::numeric_limits<
        Ti>::is_integer /*|| std::numeric_limits<Ti>::is_iec559*/,
    bdig>::type
operator+(const Ti v1, bdig &v2) {
  return (v2 + v1);
}
template <class Ti>
friend typename sag::enable_if<
    std::numeric_limits<
        Ti>::is_integer /* || std::numeric_limits<Ti>::is_iec559*/,
    bdig>::type
operator-(const Ti v1, bdig &v2) {
  return (bdig(v1) - v2);
}
template <class Ti>
friend typename sag::enable_if<std::numeric_limits<Ti>::is_integer, bdig>::type
operator*(const Ti v1, bdig &v2) {
  return (v2 * v1);
}
template <class Ti>
friend typename sag::enable_if<
    std::numeric_limits<
        Ti>::is_integer /* || std::numeric_limits<Ti>::is_iec559*/,
    bdig>::type
operator/(const Ti v1, bdig &v2) {
  return (bdig(v1) / v2);
}

}; // namespace sag

template <const int isz, const int prec, class T>
bdig<isz, prec, T> bdig<isz, prec, T>::i10;
} // namespace sag
