//===------------------------- hq_string.cpp ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hq_string"
#include "charconv"
#include "cstdlib"
#include "cwchar"
#include "cerrno"

_LIBCPP_BEGIN_NAMESPACE_STD

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS hq_basic_string<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS
    hq_basic_string<wchar_t>;

template hq_string operator+
    <char, char_traits<char>, allocator<char> >(char const*, hq_string const&);

namespace {

template <typename T>
inline void throw_helper(const hq_string& msg) {
#ifndef _LIBCPP_NO_EXCEPTIONS
  throw T(msg);
#else
  fprintf(stderr, "%s\n", msg.c_str());
  _VSTD::abort();
#endif
}

inline void throw_from_string_out_of_range(const hq_string& func) {
  throw_helper<out_of_range>(func + ": out of range");
}

inline void throw_from_string_invalid_arg(const hq_string& func) {
  throw_helper<invalid_argument>(func + ": no conversion");
}

// as_integer

template <typename V, typename S, typename F>
inline V as_integer_helper(const hq_string& func, const S& str, size_t* idx,
                           int base, F f) {
  typename S::value_type* ptr = nullptr;
  const typename S::value_type* const p = str.c_str();
  typename remove_reference<decltype(errno)>::type errno_save = errno;
  errno = 0;
  V r = f(p, &ptr, base);
  swap(errno, errno_save);
  if (errno_save == ERANGE)
    throw_from_string_out_of_range(func);
  if (ptr == p)
    throw_from_string_invalid_arg(func);
  if (idx)
    *idx = static_cast<size_t>(ptr - p);
  return r;
}

template <typename V, typename S>
inline V as_integer(const hq_string& func, const S& s, size_t* idx, int base);

// hq_string
template <>
inline int as_integer(const hq_string& func, const hq_string& s, size_t* idx,
                      int base) {
  // Use long as no Standard hq_string to integer exists.
  long r = as_integer_helper<long>(func, s, idx, base, strtol);
  if (r < numeric_limits<int>::min() || numeric_limits<int>::max() < r)
    throw_from_string_out_of_range(func);
  return static_cast<int>(r);
}

template <>
inline long as_integer(const hq_string& func, const hq_string& s, size_t* idx,
                       int base) {
  return as_integer_helper<long>(func, s, idx, base, strtol);
}

template <>
inline unsigned long as_integer(const hq_string& func, const hq_string& s,
                                size_t* idx, int base) {
  return as_integer_helper<unsigned long>(func, s, idx, base, strtoul);
}

template <>
inline long long as_integer(const hq_string& func, const hq_string& s,
                            size_t* idx, int base) {
  return as_integer_helper<long long>(func, s, idx, base, strtoll);
}

template <>
inline unsigned long long as_integer(const hq_string& func, const hq_string& s,
                                     size_t* idx, int base) {
  return as_integer_helper<unsigned long long>(func, s, idx, base, strtoull);
}

// hq_wstring
template <>
inline int as_integer(const hq_string& func, const hq_wstring& s, size_t* idx,
                      int base) {
  // Use long as no Stantard hq_string to integer exists.
  long r = as_integer_helper<long>(func, s, idx, base, wcstol);
  if (r < numeric_limits<int>::min() || numeric_limits<int>::max() < r)
    throw_from_string_out_of_range(func);
  return static_cast<int>(r);
}

template <>
inline long as_integer(const hq_string& func, const hq_wstring& s, size_t* idx,
                       int base) {
  return as_integer_helper<long>(func, s, idx, base, wcstol);
}

template <>
inline unsigned long as_integer(const hq_string& func, const hq_wstring& s,
                                size_t* idx, int base) {
  return as_integer_helper<unsigned long>(func, s, idx, base, wcstoul);
}

template <>
inline long long as_integer(const hq_string& func, const hq_wstring& s,
                            size_t* idx, int base) {
  return as_integer_helper<long long>(func, s, idx, base, wcstoll);
}

template <>
inline unsigned long long as_integer(const hq_string& func, const hq_wstring& s,
                                     size_t* idx, int base) {
  return as_integer_helper<unsigned long long>(func, s, idx, base, wcstoull);
}

// as_float

template <typename V, typename S, typename F>
inline V as_float_helper(const hq_string& func, const S& str, size_t* idx,
                         F f) {
  typename S::value_type* ptr = nullptr;
  const typename S::value_type* const p = str.c_str();
  typename remove_reference<decltype(errno)>::type errno_save = errno;
  errno = 0;
  V r = f(p, &ptr);
  swap(errno, errno_save);
  if (errno_save == ERANGE)
    throw_from_string_out_of_range(func);
  if (ptr == p)
    throw_from_string_invalid_arg(func);
  if (idx)
    *idx = static_cast<size_t>(ptr - p);
  return r;
}

template <typename V, typename S>
inline V as_float(const hq_string& func, const S& s, size_t* idx = nullptr);

template <>
inline float as_float(const hq_string& func, const hq_string& s, size_t* idx) {
  return as_float_helper<float>(func, s, idx, strtof);
}

template <>
inline double as_float(const hq_string& func, const hq_string& s, size_t* idx) {
  return as_float_helper<double>(func, s, idx, strtod);
}

template <>
inline long double as_float(const hq_string& func, const hq_string& s,
                            size_t* idx) {
  return as_float_helper<long double>(func, s, idx, strtold);
}

template <>
inline float as_float(const hq_string& func, const hq_wstring& s, size_t* idx) {
  return as_float_helper<float>(func, s, idx, wcstof);
}

template <>
inline double as_float(const hq_string& func, const hq_wstring& s,
                       size_t* idx) {
  return as_float_helper<double>(func, s, idx, wcstod);
}

template <>
inline long double as_float(const hq_string& func, const hq_wstring& s,
                            size_t* idx) {
  return as_float_helper<long double>(func, s, idx, wcstold);
}

} // unnamed namespace

int hq_stoi(const hq_string& str, size_t* idx, int base) {
  return as_integer<int>("hq_stoi", str, idx, base);
}

int hq_stoi(const hq_wstring& str, size_t* idx, int base) {
  return as_integer<int>("hq_stoi", str, idx, base);
}

long hq_stol(const hq_string& str, size_t* idx, int base) {
  return as_integer<long>("hq_stol", str, idx, base);
}

long hq_stol(const hq_wstring& str, size_t* idx, int base) {
  return as_integer<long>("hq_stol", str, idx, base);
}

unsigned long hq_stoul(const hq_string& str, size_t* idx, int base) {
  return as_integer<unsigned long>("hq_stoul", str, idx, base);
}

unsigned long hq_stoul(const hq_wstring& str, size_t* idx, int base) {
  return as_integer<unsigned long>("hq_stoul", str, idx, base);
}

long long hq_stoll(const hq_string& str, size_t* idx, int base) {
  return as_integer<long long>("hq_stoll", str, idx, base);
}

long long hq_stoll(const hq_wstring& str, size_t* idx, int base) {
  return as_integer<long long>("hq_stoll", str, idx, base);
}

unsigned long long hq_stoull(const hq_string& str, size_t* idx, int base) {
  return as_integer<unsigned long long>("hq_stoull", str, idx, base);
}

unsigned long long hq_stoull(const hq_wstring& str, size_t* idx, int base) {
  return as_integer<unsigned long long>("hq_stoull", str, idx, base);
}

float hq_stof(const hq_string& str, size_t* idx) {
  return as_float<float>("hq_stof", str, idx);
}

float hq_stof(const hq_wstring& str, size_t* idx) {
  return as_float<float>("hq_stof", str, idx);
}

double hq_stod(const hq_string& str, size_t* idx) {
  return as_float<double>("hq_stod", str, idx);
}

double hq_stod(const hq_wstring& str, size_t* idx) {
  return as_float<double>("hq_stod", str, idx);
}

long double hq_stold(const hq_string& str, size_t* idx) {
  return as_float<long double>("hq_stold", str, idx);
}

long double hq_stold(const hq_wstring& str, size_t* idx) {
  return as_float<long double>("hq_stold", str, idx);
}

// to_string

namespace {

// as_string

template <typename S, typename P, typename V>
inline S as_string(P sprintf_like, S s, const typename S::value_type* fmt,
                   V a) {
  typedef typename S::size_type size_type;
  size_type available = s.size();
  while (true) {
    int status = sprintf_like(&s[0], available + 1, fmt, a);
    if (status >= 0) {
      size_type used = static_cast<size_type>(status);
      if (used <= available) {
        s.resize(used);
        break;
      }
      available = used; // Assume this is advice of how much space we need.
    } else
      available = available * 2 + 1;
    s.resize(available);
  }
  return s;
}

template <class S>
struct initial_string;

template <>
struct initial_string<hq_string> {
  hq_string operator()() const {
    hq_string s;
    s.resize(s.capacity());
    return s;
  }
};

template <>
struct initial_string<hq_wstring> {
  hq_wstring operator()() const {
    hq_wstring s(20, wchar_t());
    s.resize(s.capacity());
    return s;
  }
};

typedef int (*wide_printf)(wchar_t* __restrict, size_t,
                           const wchar_t* __restrict, ...);

inline wide_printf get_swprintf() {
#ifndef _LIBCPP_MSVCRT
  return swprintf;
#else
  return static_cast<int(__cdecl*)(wchar_t * __restrict, size_t,
                                   const wchar_t* __restrict, ...)>(_snwprintf);
#endif
}

template <typename S, typename V>
S i_to_string(const V v) {
  //  numeric_limits::digits10 returns value less on 1 than desired for unsigned numbers.
  //  For example, for 1-byte unsigned value digits10 is 2 (999 can not be represented),
  //  so we need +1 here.
  constexpr size_t bufsize =
      numeric_limits<V>::digits10 + 2; // +1 for minus, +1 for digits10
  char buf[bufsize];
  const auto res = to_chars(buf, buf + bufsize, v);
  _LIBCPP_ASSERT(res.ec == errc(),
                 "bufsize must be large enough to accomodate the value");
  return S(buf, res.ptr);
}

} // unnamed namespace

hq_string to_hqstring(int val) { return i_to_string<hq_string>(val); }
hq_string to_hqstring(long val) { return i_to_string<hq_string>(val); }
hq_string to_hqstring(long long val) { return i_to_string<hq_string>(val); }
hq_string to_hqstring(unsigned val) { return i_to_string<hq_string>(val); }
hq_string to_hqstring(unsigned long val) { return i_to_string<hq_string>(val); }
hq_string to_hqstring(unsigned long long val) {
  return i_to_string<hq_string>(val);
}

hq_wstring to_hqwstring(int val) { return i_to_string<hq_wstring>(val); }
hq_wstring to_hqwstring(long val) { return i_to_string<hq_wstring>(val); }
hq_wstring to_hqwstring(long long val) { return i_to_string<hq_wstring>(val); }
hq_wstring to_hqwstring(unsigned val) { return i_to_string<hq_wstring>(val); }
hq_wstring to_hqwstring(unsigned long val) {
  return i_to_string<hq_wstring>(val);
}
hq_wstring to_hqwstring(unsigned long long val) {
  return i_to_string<hq_wstring>(val);
}

hq_string to_hqstring(float val) {
  return as_string(snprintf, initial_string<hq_string>()(), "%f", val);
}
hq_string to_hqstring(double val) {
  return as_string(snprintf, initial_string<hq_string>()(), "%f", val);
}
hq_string to_hqstring(long double val) {
  return as_string(snprintf, initial_string<hq_string>()(), "%Lf", val);
}

hq_wstring to_hqwstring(float val) {
  return as_string(get_swprintf(), initial_string<hq_wstring>()(), L"%f", val);
}
hq_wstring to_hqwstring(double val) {
  return as_string(get_swprintf(), initial_string<hq_wstring>()(), L"%f", val);
}
hq_wstring to_hqwstring(long double val) {
  return as_string(get_swprintf(), initial_string<hq_wstring>()(), L"%Lf", val);
}

_LIBCPP_END_NAMESPACE_STD
