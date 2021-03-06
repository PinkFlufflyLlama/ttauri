// Copyright Take Vos 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "required.hpp"
#include "numeric_array.hpp"
#include <fmt/format.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <emmintrin.h>
#include <cstdint>
#include <stdexcept>
#include <array>
#include <type_traits>
#include <ostream>

namespace tt {

/** A 4D vector.
 *
 * If you need a 3D vector or point, you can use this vector class
 * as a homogenious coordinate.
 *
 *
 * This class supports swizzeling. Swizzeling is done using member functions which
 * will return a `vec`. The name of the member function consists of 2 to 4 of the
 * following characters: 'x', 'y', 'z', 'w', 'r', 'g', 'b', 'a', '0' & '1'.
 * If the swizzle member function name would start with a '0' or '1' character it
 * will be prefixed with an underscore '_'.
 *
 * Since swizzle member functions always return a 4D f32x4, the third and forth
 * element will default to '0' and 'w'. This allows a 2D vector to maintain its
 * homogeniousness.
 */ 
class i32x4 {
    /* Intrinsic value of the f32x4.
     * The elements in __m128i are assigned as follows.
     *  - [127:96] w
     *  - [95:64] z
     *  - [63:32] y
     *  - [31:0] x
     */
    __m128i v;

public:
    /* Create a zeroed out f32x4.
     */
    i32x4() noexcept : i32x4(_mm_setzero_si128()) {}
    i32x4(i32x4 const &rhs) = default;
    i32x4 &operator=(i32x4 const &rhs) = default;
    i32x4(i32x4 &&rhs) = default;
    i32x4 &operator=(i32x4 &&rhs) = default;

    /** Create a i32x4 out of a __m128i
     */
    i32x4(__m128i rhs) noexcept :
        v(rhs) {}

    /** Create a i32x4 out of a __m128i
     */
    i32x4 &operator=(__m128i rhs) noexcept {
        v = rhs;
        return *this;
    }

    /** Convert a i32x4 to a __m128i.
     */
    operator __m128i () const noexcept {
        return v;
    }

    i32x4(f32x4 const &rhs) noexcept :
       i32x4(_mm_cvtps_epi32(rhs)) {}

    i32x4 &operator=(f32x4 const &rhs) noexcept {
        return *this = _mm_cvtps_epi32(rhs);
    }

    operator f32x4 () const noexcept {
        return _mm_cvtepi32_ps(*this);
    }

    explicit operator std::array<int32_t,4> () const noexcept {
        std::array<int32_t,4> r;
        _mm_storeu_si128(reinterpret_cast<__m128i*>(r.data()), *this);
        return r;
    }

    /** Initialize a i32x4 with all elements set to a value.
     * Useful as a scalar converter, when combined with an
     * arithmetic operator.
     */
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>,int> = 0>
    explicit i32x4(T rhs) noexcept:
        i32x4(_mm_set1_epi32(narrow_cast<int32_t>(rhs))) {}

    /** Initialize a i32x4 with all elements set to a value.
     * Useful as a scalar converter, when combined with an
     * arithmetic operator.
     */
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>,int> = 0>
    i32x4 &operator=(T rhs) noexcept {
        return *this = _mm_set1_epi32(narrow_cast<int32_t>(rhs));
    }

    /** Create a i32x4 out of 2 to 4 values.
    * This vector is used as a homogeneous coordinate, meaning:
    *  - vectors have w=0 (A direction and distance)
    *  - points have w=1 (A position in space)
    */
    template<typename T, typename U, typename V=int, typename W=int,
        std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V> && std::is_arithmetic_v<W>,int> = 0>
    i32x4(T x, U y, V z=0, W w=0) noexcept :
        i32x4(_mm_set_epi32(
            narrow_cast<int32_t>(w),
            narrow_cast<int32_t>(z),
            narrow_cast<int32_t>(y),
            narrow_cast<int32_t>(x)
            )) {}

    /** Create a i32x4 out of 2 to 4 values.
    * This vector is used as a homogeneous coordinate, meaning:
    *  - vectors have w=0 (A direction and distance)
    *  - points have w=1 (A position in space)
    */
    template<typename T, typename U, typename V=int, typename W=int,
    std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V> && std::is_arithmetic_v<W>,int> = 0>
    [[nodiscard]] static i32x4 point(T x, U y, V z=0, W w=1) noexcept {
        return i32x4(x, y, z, w);
    }

    template<size_t I, typename T, std::enable_if_t<std::is_arithmetic_v<T>,int> = 0>
    i32x4 &set(T rhs) noexcept {
        static_assert(I <= 3);
        return *this = _mm_insert_epi32(*this, narrow_cast<int32_t>(rhs), I);
    }

    template<size_t I>
    int get() const noexcept {
        static_assert(I <= 3);
        return _mm_extract_epi32(*this, I);
    }

    constexpr size_t size() const noexcept { return 4; }

    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>,int> = 0>
    i32x4 &x(T rhs) noexcept { return set<0>(rhs); }

    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>,int> = 0>
    i32x4 &y(T rhs) noexcept { return set<1>(rhs); }

    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>,int> = 0>
    i32x4 &z(T rhs) noexcept { return set<2>(rhs); }

    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>,int> = 0>
    i32x4 &w(T rhs) noexcept { return set<3>(rhs); }

    int x() const noexcept { return get<0>(); }
    int y() const noexcept { return get<1>(); }
    int z() const noexcept { return get<2>(); }
    int w() const noexcept { return get<3>(); }
    int width() const noexcept { return get<0>(); }
    int height() const noexcept { return get<1>(); }

    i32x4 &operator+=(i32x4 const &rhs) noexcept {
        return *this = _mm_add_epi32(*this, rhs);
    }

    i32x4 &operator-=(i32x4 const &rhs) noexcept {
        return *this = _mm_sub_epi32(*this, rhs);
    }

    i32x4 &operator*=(i32x4 const &rhs) noexcept {
        return *this = _mm_mullo_epi32(*this, rhs);
    }


    [[nodiscard]] friend i32x4 operator+(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_add_epi32(lhs, rhs);
    }

    [[nodiscard]] friend i32x4 operator-(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_sub_epi32(lhs, rhs);
    }

    [[nodiscard]] friend i32x4 operator*(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_mullo_epi32(lhs, rhs);
    }

    [[nodiscard]] friend i32x4 max(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_max_epi32(lhs, rhs);
    }

    [[nodiscard]] friend i32x4 min(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_min_epi32(lhs, rhs);
    }

    [[nodiscard]] friend bool operator==(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        ttlet tmp2 = _mm_movemask_epi8(_mm_cmpeq_epi32(lhs, rhs));
        return tmp2 == 0xffff;
    }

    [[nodiscard]] friend bool operator!=(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return !(lhs == rhs);
    }

    /** Equal to.
    * @return boolean nibble field, bit [3:0]=x, [7:4]=y, [11:8]=z, [15:12]=w.
    */
    [[nodiscard]] friend int eq(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_movemask_epi8(_mm_cmpeq_epi32(lhs, rhs));
    }

    /** Less than.
    * @return boolean nibble field, bit [3:0]=x, [7:4]=y, [11:8]=z, [15:12]=w.
    */
    [[nodiscard]] friend int operator<(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_movemask_epi8(_mm_cmplt_epi32(lhs, rhs));
    }

    /** Greater than.
    * @return boolean nibble field, bit [3:0]=x, [7:4]=y, [11:8]=z, [15:12]=w.
    */
    [[nodiscard]] friend int operator>(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return _mm_movemask_epi8(_mm_cmpgt_epi32(lhs, rhs));
    }

    [[nodiscard]] friend int operator<=(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return (~(lhs > rhs)) & 0xffff;
    }

    [[nodiscard]] friend int operator>=(i32x4 const &lhs, i32x4 const &rhs) noexcept {
        return (~(lhs < rhs)) & 0xffff;
    }

    [[nodiscard]] friend std::string to_string(i32x4 const &rhs) noexcept {
        return fmt::format("({}, {}, {}, {})", rhs.x(), rhs.y(), rhs.z(), rhs.w());
    }

    std::ostream friend &operator<<(std::ostream &lhs, i32x4 const &rhs) noexcept {
        return lhs << to_string(rhs);
    }

    template<std::size_t I>
    [[nodiscard]] friend int get(i32x4 const &rhs) noexcept {
        return rhs.get<I>();
    }

    template<char a, char b, char c, char d>
    [[nodiscard]] constexpr static int swizzle_permute_mask() noexcept {
        int r = 0;
        switch (a) {
        case 'x': r |= 0b00'00'00'00; break;
        case 'y': r |= 0b00'00'00'01; break;
        case 'z': r |= 0b00'00'00'10; break;
        case 'w': r |= 0b00'00'00'11; break;
        case '0': r |= 0b00'00'00'00; break;
        case '1': r |= 0b00'00'00'00; break;
        }
        switch (b) {
        case 'x': r |= 0b00'00'00'00; break;
        case 'y': r |= 0b00'00'01'00; break;
        case 'z': r |= 0b00'00'10'00; break;
        case 'w': r |= 0b00'00'11'00; break;
        case '0': r |= 0b00'00'01'00; break;
        case '1': r |= 0b00'00'01'00; break;
        }
        switch (c) {
        case 'x': r |= 0b00'00'00'00; break;
        case 'y': r |= 0b00'01'00'00; break;
        case 'z': r |= 0b00'10'00'00; break;
        case 'w': r |= 0b00'11'00'00; break;
        case '0': r |= 0b00'10'00'00; break;
        case '1': r |= 0b00'10'00'00; break;
        }
        switch (d) {
        case 'x': r |= 0b00'00'00'00; break;
        case 'y': r |= 0b01'00'00'00; break;
        case 'z': r |= 0b10'00'00'00; break;
        case 'w': r |= 0b11'00'00'00; break;
        case '0': r |= 0b11'00'00'00; break;
        case '1': r |= 0b11'00'00'00; break;
        }
        return r;
    }

    template<char a, char b, char c, char d>
    [[nodiscard]] i32x4 swizzle() const noexcept {
        constexpr int permute_mask = f32x4::swizzle_permute_mask<a,b,c,d>();

        __m128i swizzled;
        // Clang is able to optimize these intrinsics, MSVC is not.
        if constexpr (permute_mask != 0b11'10'01'00) {
            swizzled = _mm_shuffle_epi32(*this, permute_mask);
        } else {
            swizzled = *this;
        }

        if constexpr (a == '0' || a == '1') {
            swizzled = _mm_insert_epi32(swizzled, a == '0' ? 0 : 1, 0);
        }
        if constexpr (b == '0' || b == '1') {
            swizzled = _mm_insert_epi32(swizzled, b == '0' ? 0 : 1, 1);
        }
        if constexpr (c == '0' || c == '1') {
            swizzled = _mm_insert_epi32(swizzled, c == '0' ? 0 : 1, 2);
        }
        if constexpr (d == '0' || d == '1') {
            swizzled = _mm_insert_epi32(swizzled, d == '0' ? 0 : 1, 3);
        }

        return swizzled;
    }

#define SWIZZLE4(name, A, B, C, D)\
    [[nodiscard]] i32x4 name() const noexcept {\
        return swizzle<A, B, C, D>();\
    }

#define SWIZZLE4_GEN3(name, A, B, C)\
    SWIZZLE4(name ## 0, A, B, C, '0')\
    SWIZZLE4(name ## 1, A, B, C, '1')\
    SWIZZLE4(name ## x, A, B, C, 'x')\
    SWIZZLE4(name ## y, A, B, C, 'y')\
    SWIZZLE4(name ## z, A, B, C, 'z')\
    SWIZZLE4(name ## w, A, B, C, 'w')

#define SWIZZLE4_GEN2(name, A, B)\
    SWIZZLE4_GEN3(name ## 0, A, B, '0')\
    SWIZZLE4_GEN3(name ## 1, A, B, '1')\
    SWIZZLE4_GEN3(name ## x, A, B, 'x')\
    SWIZZLE4_GEN3(name ## y, A, B, 'y')\
    SWIZZLE4_GEN3(name ## z, A, B, 'z')\
    SWIZZLE4_GEN3(name ## w, A, B, 'w')

#define SWIZZLE4_GEN1(name, A)\
    SWIZZLE4_GEN2(name ## 0, A, '0')\
    SWIZZLE4_GEN2(name ## 1, A, '1')\
    SWIZZLE4_GEN2(name ## x, A, 'x')\
    SWIZZLE4_GEN2(name ## y, A, 'y')\
    SWIZZLE4_GEN2(name ## z, A, 'z')\
    SWIZZLE4_GEN2(name ## w, A, 'w')

    SWIZZLE4_GEN1(_0, '0')
    SWIZZLE4_GEN1(_1, '1')
    SWIZZLE4_GEN1(x, 'x')
    SWIZZLE4_GEN1(y, 'y')
    SWIZZLE4_GEN1(z, 'z')
    SWIZZLE4_GEN1(w, 'w')

#define SWIZZLE3(name, A, B, C)\
    [[nodiscard]] i32x4 name() const noexcept {\
        return swizzle<A,B,C,'w'>();\
    }

#define SWIZZLE3_GEN2(name, A, B)\
    SWIZZLE3(name ## 0, A, B, '0')\
    SWIZZLE3(name ## 1, A, B, '1')\
    SWIZZLE3(name ## x, A, B, 'x')\
    SWIZZLE3(name ## y, A, B, 'y')\
    SWIZZLE3(name ## z, A, B, 'z')\
    SWIZZLE3(name ## w, A, B, 'w')

#define SWIZZLE3_GEN1(name, A)\
    SWIZZLE3_GEN2(name ## 0, A, '0')\
    SWIZZLE3_GEN2(name ## 1, A, '1')\
    SWIZZLE3_GEN2(name ## x, A, 'x')\
    SWIZZLE3_GEN2(name ## y, A, 'y')\
    SWIZZLE3_GEN2(name ## z, A, 'z')\
    SWIZZLE3_GEN2(name ## w, A, 'w')

    SWIZZLE3_GEN1(_0, '0')
    SWIZZLE3_GEN1(_1, '1')
    SWIZZLE3_GEN1(x, 'x')
    SWIZZLE3_GEN1(y, 'y')
    SWIZZLE3_GEN1(z, 'z')
    SWIZZLE3_GEN1(w, 'w')

#define SWIZZLE2(name, A, B)\
    [[nodiscard]] i32x4 name() const noexcept {\
        return swizzle<A,B,'0','w'>();\
    }

#define SWIZZLE2_GEN1(name, A)\
    SWIZZLE2(name ## 0, A, '0')\
    SWIZZLE2(name ## 1, A, '1')\
    SWIZZLE2(name ## x, A, 'x')\
    SWIZZLE2(name ## y, A, 'y')\
    SWIZZLE2(name ## z, A, 'z')\
    SWIZZLE2(name ## w, A, 'w')

    SWIZZLE2_GEN1(_0, '0')
    SWIZZLE2_GEN1(_1, '1')
    SWIZZLE2_GEN1(x, 'x')
    SWIZZLE2_GEN1(y, 'y')
    SWIZZLE2_GEN1(z, 'z')
    SWIZZLE2_GEN1(w, 'w')
};

}

#undef SWIZZLE4
#undef SWIZZLE4_GEN1
#undef SWIZZLE4_GEN2
#undef SWIZZLE4_GEN3
#undef SWIZZLE3
#undef SWIZZLE3_GEN1
#undef SWIZZLE3_GEN2
#undef SWIZZLE2
#undef SWIZZLE2_GEN1
