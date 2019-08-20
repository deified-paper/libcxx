//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <cmath>
#include <cstdint>
#include <type_traits>

#include "test_macros.h"

template<class T>
struct correct_size_int
{
    typedef typename std::conditional<sizeof(T) <= sizeof(int), int, T>::type type;
};

template <class Source, class Result>
void test_abs()
{
    Source neg_val = -5;
    Source pos_val = 5;
    Result res = 5;

    ASSERT_SAME_TYPE(decltype(std::abs(neg_val)), Result);

    assert(std::abs(neg_val) == res);
    assert(std::abs(pos_val) == res);
}

void test_big()
{
    long long int big_value = std::numeric_limits<long long int>::max(); // a value to big for ints to store
    long long int negative_big_value = -big_value;
    assert(std::abs(negative_big_value) == big_value); // make sure it doesnt get casted to a smaller type
}

int main(int, char**)
{
    test_abs<short int, typename correct_size_int<short int>::type>();
    test_abs<char, typename correct_size_int<char>::type>();
    test_abs<signed char, typename correct_size_int<signed char>::type>();

    test_abs<int, typename correct_size_int<int>::type>();
    test_abs<long int, typename correct_size_int<long int>::type>();
    test_abs<long long int, typename correct_size_int<long long int>::type>();

    test_abs<std::int8_t, typename correct_size_int<std::int8_t>::type>();
    test_abs<std::int16_t, typename correct_size_int<std::int16_t>::type>();
    test_abs<std::int32_t, typename correct_size_int<std::int32_t>::type>();
    test_abs<std::int64_t, typename correct_size_int<std::int64_t>::type>();

    test_abs<long double, long double>();
    test_abs<double, double>();
    test_abs<float, float>();

    test_big();

    return 0;
}
