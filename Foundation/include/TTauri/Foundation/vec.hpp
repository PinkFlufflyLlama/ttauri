// Copyright 2020 Pokitec
// All rights reserved.

#pragma once

#include "TTauri/Foundation/required.hpp"
#include "TTauri/Foundation/numeric_cast.hpp"
#include <xmmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <cstdint>
#include <stdexcept>
#include <array>

namespace TTauri {

/** A 4D vector.
 *
 * If you need a 3D vector or point, you can use this vector class
 * as a homogenious coordinate.
 *
 * Since the __m64 data type is not supported by MSVC on x64 it does
 * not yield a performance improvement to create a seperate 2D vector
 * class. For simplicity use the 4D class with the z-value set to 0.0
 * when working with 2D vectors.
 *
 * If you use this vector as a color, then x=Red, y=Green, z=Blue, w=Alpha.
 *  - Alpha is linear: 0.0 is transparent, 1.0 is opaque.
 *    The Red/Green/Blue are not pre-multiplied with the alpha.
 *  - Red/Green/Blue are based on the linear-scRGB floating point format:
 *    values between 0.0 and 1.0 is equivilant to linear-sRGB (no gamma curve).
 *    values are allowed to be outside of this range for high-dynamic-range
 *    and high-color-gamut. 1.0,1.0,1.0 equals 80 cd/m2 and should be the maximum
 *    value for user interfaces.
 *
 * This class supports swizzeling. Swizzeling is done using member functions which
 * will return a `vec`. The name of the member function consists of 2 to 4 of the
 * following characters: 'x', 'y', 'z', 'w', 'r', 'g', 'b', 'a', '0' & '1'.
 * If the swizzle member function name would start with a '0' or '1' character it
 * will be prefixed with an underscore '_'.
 *
 * Since swizzle member functions always return a 4D vec, the third and forth
 * element will default to '0' and 'w'. This allows a 2D vector to maintain its
 * homogeniousness, or a color to maintain its alpha value.
 */ 
class vec {
    /* Intrinsic value of the vec.
     * The elements in __m128 are assigned as follows.
     *  - [127:96] w, alpha
     *  - [95:64] z, blue
     *  - [63:32] y, green
     *  - [31:0] x, red
     */
    __m128 v;

public:
    /* Create a zeroed out vec.
     */
    force_inline vec() noexcept : vec(_mm_setzero_ps()) {}
    force_inline vec(vec const &rhs) = default;
    force_inline vec &operator=(vec const &rhs) = default;
    force_inline vec(vec &&rhs) = default;
    force_inline vec &operator=(vec &&rhs) = default;

    /** Create a vec out of a __m128
     */
    force_inline vec(__m128 rhs) noexcept :
        v(rhs) {}

    /** Create a vec out of a __m128
     */
    force_inline vec &operator=(__m128 rhs) noexcept {
        v = rhs;
        return *this;
    }

    /** Convert a vec to a __m128.
     */
    force_inline operator __m128 () const noexcept {
        return v;
    }

    explicit force_inline operator std::array<float,4> () const noexcept {
        std::array<float,4> r;
        _mm_storeu_ps(r.data(), *this);
        return r;
    }
        
    /** Initialize a vec with all elements set to a value.
     * Useful as a scalar converter, when combined with an
     * arithmatic operator.
     */
    force_inline vec(float rhs) noexcept:
        vec(_mm_set_ps1(rhs)) {}

    /** Initialize a vec with all elements set to a value.
     * Useful as a scalar converter, when combined with an
     * arithmatic operator.
     */
    force_inline vec &operator=(float rhs) noexcept {
        return *this = _mm_set_ps1(rhs);
    }

    /** Create a vec out of 2 to 4 values.
    * This vector is used as a homogenious coordinate, meaning:
    *  - vectors have w=0.0 (A direction and distance)
    *  - points have w=1.0 (A position in space)
    *
    * When this vector is used for color then:
    *  - x=Red, y=Green, z=Blue, w=Alpha
    *
    */
    template<typename T, typename U, typename V=float, typename W=float,
        std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V> && std::is_arithmetic_v<W>,int> = 0>
    force_inline vec(T x, U y, V z=0.0f, W w=0.0f) noexcept :
        vec(_mm_set_ps(
            numeric_cast<float>(w),
            numeric_cast<float>(z),
            numeric_cast<float>(y),
            numeric_cast<float>(x)
        )) {}

    /** Create a point out of 2 to 4 values.
    * This vector is used as a homogenious coordinate, meaning:
    *  - vectors have w=0.0 (A direction and distance)
    *  - points have w=1.0 (A position in space)
    *
    * When this vector is used for color then:
    *  - x=Red, y=Green, z=Blue, w=Alpha
    *
    */
    template<typename T, typename U, typename V=float, typename W=float,
    std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V>,int> = 0>
    [[nodiscard]] force_inline static vec point(T x, U y, V z=0.0f) noexcept {
        return vec{x, y, z, 1.0};
    }

    [[nodiscard]] force_inline static vec origin() noexcept {
        return vec{_mm_permute_ps(_mm_set_ss(1.0f), 0b00'01'10'11)};
    }

    /** Create a color out of 2 to 4 values.
    * This vector is used as a homogenious coordinate, meaning:
    *  - vectors have w=0.0 (A direction and distance)
    *  - points have w=1.0 (A position in space)
    *
    * When this vector is used for color then:
    *  - x=Red, y=Green, z=Blue, w=Alpha
    *
    */
    template<typename T, typename U, typename V=float, typename W=float,
        std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U> && std::is_arithmetic_v<V> && std::is_arithmetic_v<W>,int> = 0>
        [[nodiscard]] force_inline static vec color(T r, U g, V b=0.0f, W a=1.0f) noexcept {
        return vec{r, g, b, a};
    }

    template<size_t I>
    force_inline vec &set(float rhs) noexcept {
        static_assert(I <= 3);
        let tmp = _mm_set_ss(rhs);
        return *this = _mm_insert_ps(*this, tmp, I << 4);
    }

    template<size_t I>
    force_inline float get() const noexcept {
        static_assert(I <= 3);
        let tmp = _mm_permute_ps(*this, I);
        return _mm_cvtss_f32(tmp);
    }

    force_inline bool is_point() const noexcept {
        return w() == 1.0f;
    }

    force_inline bool is_vector() const noexcept {
        return w() == 0.0f;
    }

    force_inline bool is_opaque() const noexcept {
        return a() == 1.0f;
    }

    force_inline bool is_transparent() const noexcept {
        return a() == 0.0f;
    }

    constexpr size_t size() const noexcept {
        return 4;
    }

    force_inline float operator[](size_t i) const noexcept {
        ttauri_assume(i <= 3);
        let i_ = _mm_set1_epi32(static_cast<uint32_t>(i));
        let tmp = _mm_permutevar_ps(*this, i_);
        return _mm_cvtss_f32(tmp);
    }

    force_inline vec &x(float rhs) noexcept { return set<0>(rhs); }
    force_inline vec &y(float rhs) noexcept { return set<1>(rhs); }
    force_inline vec &z(float rhs) noexcept { return set<2>(rhs); }
    force_inline vec &w(float rhs) noexcept { return set<3>(rhs); }
    force_inline vec &r(float rhs) noexcept { return set<0>(rhs); }
    force_inline vec &g(float rhs) noexcept { return set<1>(rhs); }
    force_inline vec &b(float rhs) noexcept { return set<2>(rhs); }
    force_inline vec &a(float rhs) noexcept { return set<3>(rhs); }
    force_inline vec &width(float rhs) noexcept { return set<0>(rhs); }
    force_inline vec &height(float rhs) noexcept { return set<1>(rhs); }
    force_inline vec &depth(float rhs) noexcept { return set<2>(rhs); }
    force_inline float x() const noexcept { return get<0>(); }
    force_inline float y() const noexcept { return get<1>(); }
    force_inline float z() const noexcept { return get<2>(); }
    force_inline float w() const noexcept { return get<3>(); }
    force_inline float r() const noexcept { return get<0>(); }
    force_inline float g() const noexcept { return get<1>(); }
    force_inline float b() const noexcept { return get<2>(); }
    force_inline float a() const noexcept { return get<3>(); }
    force_inline float width() const noexcept { return get<0>(); }
    force_inline float height() const noexcept { return get<1>(); }
    force_inline float depth() const noexcept { return get<2>(); }


    force_inline vec &operator+=(vec const &rhs) noexcept {
        return *this = _mm_add_ps(*this, rhs);
    }

    force_inline vec &operator-=(vec const &rhs) noexcept {
        return *this = _mm_sub_ps(*this, rhs);
    }

    force_inline vec &operator*=(vec const &rhs) noexcept {
        return *this = _mm_mul_ps(*this, rhs);
    }

    force_inline vec &operator/=(vec const &rhs) noexcept {
        return *this = _mm_div_ps(*this, rhs);
    }

    [[nodiscard]] force_inline friend vec operator-(vec const &rhs) noexcept {
        return _mm_sub_ps(_mm_setzero_ps(), rhs);
    }

    [[nodiscard]] force_inline friend vec operator+(vec const &lhs, vec const &rhs) noexcept {
        return _mm_add_ps(lhs, rhs);
    }

    [[nodiscard]] force_inline friend vec operator-(vec const &lhs, vec const &rhs) noexcept {
        return _mm_sub_ps(lhs, rhs);
    }

    [[nodiscard]] force_inline friend vec operator*(vec const &lhs, vec const &rhs) noexcept {
        return _mm_mul_ps(lhs, rhs);
    }

    [[nodiscard]] force_inline friend vec operator/(vec const &lhs, vec const &rhs) noexcept {
        return _mm_div_ps(lhs, rhs);
    }

    [[nodiscard]] force_inline friend vec max(vec const &lhs, vec const &rhs) noexcept {
        return _mm_max_ps(lhs, rhs);
    }

    [[nodiscard]] force_inline friend vec min(vec const &lhs, vec const &rhs) noexcept {
        return _mm_min_ps(lhs, rhs);
    }

    [[nodiscard]] force_inline friend vec abs(vec const &rhs) noexcept {
        return max(rhs, -rhs);
    }

    [[nodiscard]] force_inline friend bool operator==(vec const &lhs, vec const &rhs) noexcept {
        let tmp2 = _mm_movemask_ps(_mm_cmpeq_ps(lhs, rhs));
        return tmp2 == 0b1111;
    }

    [[nodiscard]] force_inline friend bool operator!=(vec const &lhs, vec const &rhs) noexcept {
        return !(lhs == rhs);
    }

    /** Equal to.
    * @return boolean bit field, bit 0=x, 1=y, 2=z, 3=w.
    */
    [[nodiscard]] force_inline friend int eq(vec const &lhs, vec const &rhs) noexcept {
        return _mm_movemask_ps(_mm_cmpeq_ps(lhs, rhs));
    }

    /** Not equal to.
    * @return boolean bit field, bit 0=x, 1=y, 2=z, 3=w.
    */
    [[nodiscard]] force_inline friend int ne(vec const &lhs, vec const &rhs) noexcept {
        return _mm_movemask_ps(_mm_cmpneq_ps(lhs, rhs));
    }

    /** Less than.
     * @return boolean bit field, bit 0=x, 1=y, 2=z, 3=w.
     */
    [[nodiscard]] force_inline friend int operator<(vec const &lhs, vec const &rhs) noexcept {
        return _mm_movemask_ps(_mm_cmplt_ps(lhs, rhs));
    }

    /** Less than or equal.
    * @return boolean bit field, bit 0=x, 1=y, 2=z, 3=w.
    */
    [[nodiscard]] force_inline friend int operator<=(vec const &lhs, vec const &rhs) noexcept {
        return _mm_movemask_ps(_mm_cmple_ps(lhs, rhs));
    }

    /** Greater than.
    * @return boolean bit field, bit 0=x, 1=y, 2=z, 3=w.
    */
    [[nodiscard]] force_inline friend int operator>(vec const &lhs, vec const &rhs) noexcept {
        return _mm_movemask_ps(_mm_cmpgt_ps(lhs, rhs));
    }

    /** Greater than or equal.
    * @return boolean bit field, bit 0=x, 1=y, 2=z, 3=w.
    */
    [[nodiscard]] force_inline friend int operator>=(vec const &lhs, vec const &rhs) noexcept {
        return _mm_movemask_ps(_mm_cmpge_ps(lhs, rhs));
    }

    [[nodiscard]] force_inline friend __m128 _length_squared(vec const &rhs) noexcept {
        let tmp1 = _mm_mul_ps(rhs, rhs);
        let tmp2 = _mm_hadd_ps(tmp1, tmp1);
        return _mm_hadd_ps(tmp2, tmp2);
    }

    [[nodiscard]] force_inline friend float length_squared(vec const &rhs) noexcept {
        return _mm_cvtss_f32(_length_squared(rhs));
    }

    [[nodiscard]] force_inline friend float length(vec const &rhs) noexcept {
        let tmp = _mm_sqrt_ps(_length_squared(rhs));
        return _mm_cvtss_f32(tmp);
    }

    [[nodiscard]] force_inline friend vec normalize(vec const &rhs) noexcept {
        auto ___l = _length_squared(rhs);
        auto llll = _mm_permute_ps(___l, _MM_SHUFFLE(0,0,0,0));
        auto iiii = _mm_rsqrt_ps(llll);
        return _mm_mul_ps(rhs, iiii);
    }

    [[nodiscard]] force_inline friend vec homogeneous_divide(vec const &rhs) noexcept {
        auto wwww = _mm_permute_ps(rhs, _MM_SHUFFLE(3,3,3,3));
        auto rcp_wwww = _mm_rcp_ps(wwww);
        return _mm_mul_ps(rhs, rcp_wwww);
    }

    [[nodiscard]] force_inline friend float dot(vec const &lhs, vec const &rhs) noexcept {
        let tmp1 = _mm_mul_ps(lhs, rhs);
        let tmp2 = _mm_hadd_ps(tmp1, tmp1);
        let tmp3 = _mm_hadd_ps(tmp2, tmp2);
        return _mm_cvtss_f32(tmp3);
    }

    [[nodiscard]] force_inline friend float viktor_cross(vec const &lhs, vec const &rhs) noexcept {
        // a.x * b.y - a.y * b.x
        let tmp1 = _mm_permute_ps(rhs, _MM_SHUFFLE(2,3,0,1));
        let tmp2 = _mm_mul_ps(lhs, tmp1);
        let tmp3 = _mm_hsub_ps(tmp2, tmp2);
        return _mm_cvtss_f32(tmp3);
    }

    // x=a.y*b.z - a.z*b.y
    // y=a.z*b.x - a.x*b.z
    // z=a.x*b.y - a.y*b.x
    // w=a.w*b.w - a.w*b.w
    [[nodiscard]] friend vec cross(vec const &lhs, vec const &rhs) noexcept {
        let a_left = _mm_permute_ps(lhs, _MM_SHUFFLE(3,0,2,1));
        let b_left = _mm_permute_ps(rhs, _MM_SHUFFLE(3,1,0,2));
        let left = _mm_mul_ps(a_left, b_left);

        let a_right = _mm_permute_ps(lhs, _MM_SHUFFLE(3,1,0,2));
        let b_right = _mm_permute_ps(rhs, _MM_SHUFFLE(3,0,2,1));
        let right = _mm_mul_ps(a_right, b_right);
        return _mm_sub_ps(left, right);
    }

    /** Calculate the 2D normal on a 2D vector.
     */
    [[nodiscard]] force_inline friend vec normal(vec const &rhs) noexcept {
        ttauri_assume(rhs.z() == 0.0f && rhs.w() == 0.0f);
        return normalize(vec{-rhs.y(), rhs.x()});
    }

    /** Find a point at the midpoint between two points.
     */
    [[nodiscard]] friend vec midpoint(vec const &p1, vec const &p2) noexcept {
        return (p1 + p2) * 0.5;
    }

    [[nodiscard]] friend vec desaturate(vec const &color, float brightness) noexcept {
        // Use luminance ratios and change the brightness.
        // luminance ratios according to BT.709.
        let _0LLL = color * vec{0.2126, 0.7152, 0.0722} * brighness;
        let __LL = _mm_hadd_ps(_0LLL _0LLL);
        let ___L = _mm_hadd_ps(__LL, __LL);

        // grayscale, with original alpha. 
        return _mm_blend_ps(_mm_set1_ps(L), color, 0b1000);
    }

    [[nodiscard]] friend vec composit(vec &under, vec &over) noexcept {
        if (over.is_transparent()) {
            return under;
        }
        if (over.is_opaque()) {
            return over;
        }

        let over_alpha = over.aaaa();
        let under_alpha = under.aaaa();

        let over_color = over.rgb1();
        let under_color = over.rgb1();

        output_color =
            over_color * over_alpha +
            under_color * under_alpha * (vec{1.0} - over_alpha);

        output_color /= output_color.aaa1();

        return output_color;
    }

    /** Find the point on the other side and at the same distance of an anchor-point.
     */
    [[nodiscard]] friend vec reflect_point(vec const &p, vec const anchor) noexcept {
        return anchor - (p - anchor);
    }

    [[nodiscard]] friend std::string to_string(vec const &rhs) noexcept {
        return fmt::format("({}, {}, {}, {})", rhs.x(), rhs.y(), rhs.z(), rhs.w());
    }

    std::ostream friend &operator<<(std::ostream &lhs, vec const &rhs) noexcept {
        return lhs << to_string(rhs);
    }

    template<std::size_t I>
    [[nodiscard]] force_inline friend float get(vec const &rhs) noexcept {
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
    [[nodiscard]] constexpr static int swizzle_zero_mask() noexcept {
        int r = 0;
        r |= (a == '1') ? 0 : 0b0001;
        r |= (b == '1') ? 0 : 0b0010;
        r |= (c == '1') ? 0 : 0b0100;
        r |= (d == '1') ? 0 : 0b1000;
        return r;
    }

    template<char a, char b, char c, char d>
    [[nodiscard]] constexpr static int swizzle_number_mask() noexcept {
        int r = 0;
        r |= (a == '0' || a == '1') ? 0b0001 : 0;
        r |= (b == '0' || b == '1') ? 0b0010 : 0;
        r |= (c == '0' || c == '1') ? 0b0100 : 0;
        r |= (d == '0' || d == '1') ? 0b1000 : 0;
        return r;
    }

    template<char a, char b, char c, char d>
    [[nodiscard]] force_inline vec swizzle() const noexcept {
        constexpr int permute_mask = vec::swizzle_permute_mask<a,b,c,d>();
        constexpr int zero_mask = vec::swizzle_zero_mask<a,b,c,d>();
        constexpr int number_mask = vec::swizzle_number_mask<a,b,c,d>();

        __m128 swizzled;
        // Clang is able to optimize these intrinsics, MSVC is not.
        if constexpr (permute_mask != 0b11'10'01'00) {
            swizzled = _mm_permute_ps(*this, permute_mask);
        } else {
            swizzled = *this;
        }

        __m128 numbers;
        if constexpr (zero_mask == 0b0000) {
            numbers = _mm_set_ps1(1.0f);
        } else if constexpr (zero_mask == 0b1111) {
            numbers = _mm_setzero_ps();
        } else if constexpr (zero_mask == 0b1110) {
            numbers = _mm_set_ss(1.0f);
        } else {
            let _1111 = _mm_set_ps1(1.0f);
            numbers = _mm_insert_ps(_1111, _1111, zero_mask);
        }

        __m128 result;
        if constexpr (number_mask == 0b0000) {
            result = swizzled;
        } else if constexpr (number_mask == 0b1111) {
            result = numbers;
        } else if constexpr (((zero_mask | ~number_mask) & 0b1111) == 0b1111) {
            result = _mm_insert_ps(swizzled, swizzled, number_mask);
        } else {
            result = _mm_blend_ps(swizzled, numbers, number_mask);
        }
        return result;
    }

#define SWIZZLE4(name, A, B, C, D)\
    [[nodiscard]] vec name() const noexcept {\
        return swizzle<A, B, C, D>();\
    }

#define SWIZZLE4_GEN3(name, A, B, C)\
    SWIZZLE4(name ## 0, A, B, C, '0')\
    SWIZZLE4(name ## 1, A, B, C, '1')\
    SWIZZLE4(name ## x, A, B, C, 'x')\
    SWIZZLE4(name ## y, A, B, C, 'y')\
    SWIZZLE4(name ## z, A, B, C, 'z')\
    SWIZZLE4(name ## w, A, B, C, 'w')\
    SWIZZLE4(name ## r, A, B, C, 'x')\
    SWIZZLE4(name ## g, A, B, C, 'y')\
    SWIZZLE4(name ## b, A, B, C, 'z')\
    SWIZZLE4(name ## a, A, B, C, 'w')

#define SWIZZLE4_GEN2(name, A, B)\
    SWIZZLE4_GEN3(name ## 0, A, B, '0')\
    SWIZZLE4_GEN3(name ## 1, A, B, '1')\
    SWIZZLE4_GEN3(name ## x, A, B, 'x')\
    SWIZZLE4_GEN3(name ## y, A, B, 'y')\
    SWIZZLE4_GEN3(name ## z, A, B, 'z')\
    SWIZZLE4_GEN3(name ## w, A, B, 'w')\
    SWIZZLE4_GEN3(name ## r, A, B, 'x')\
    SWIZZLE4_GEN3(name ## g, A, B, 'y')\
    SWIZZLE4_GEN3(name ## b, A, B, 'z')\
    SWIZZLE4_GEN3(name ## a, A, B, 'w')

#define SWIZZLE4_GEN1(name, A)\
    SWIZZLE4_GEN2(name ## 0, A, '0')\
    SWIZZLE4_GEN2(name ## 1, A, '1')\
    SWIZZLE4_GEN2(name ## x, A, 'x')\
    SWIZZLE4_GEN2(name ## y, A, 'y')\
    SWIZZLE4_GEN2(name ## z, A, 'z')\
    SWIZZLE4_GEN2(name ## w, A, 'w')\
    SWIZZLE4_GEN2(name ## r, A, 'x')\
    SWIZZLE4_GEN2(name ## g, A, 'y')\
    SWIZZLE4_GEN2(name ## b, A, 'z')\
    SWIZZLE4_GEN2(name ## a, A, 'w')

    SWIZZLE4_GEN1(_0, '0')
    SWIZZLE4_GEN1(_1, '1')
    SWIZZLE4_GEN1(x, 'x')
    SWIZZLE4_GEN1(y, 'y')
    SWIZZLE4_GEN1(z, 'z')
    SWIZZLE4_GEN1(w, 'w')
    SWIZZLE4_GEN1(r, 'x')
    SWIZZLE4_GEN1(g, 'y')
    SWIZZLE4_GEN1(b, 'z')
    SWIZZLE4_GEN1(a, 'w')

#define SWIZZLE3(name, A, B, C)\
    [[nodiscard]] vec name() const noexcept {\
        return swizzle<A,B,C,'w'>();\
    }

#define SWIZZLE3_GEN2(name, A, B)\
    SWIZZLE3(name ## 0, A, B, '0')\
    SWIZZLE3(name ## 1, A, B, '1')\
    SWIZZLE3(name ## x, A, B, 'x')\
    SWIZZLE3(name ## y, A, B, 'y')\
    SWIZZLE3(name ## z, A, B, 'z')\
    SWIZZLE3(name ## w, A, B, 'w')\
    SWIZZLE3(name ## r, A, B, 'x')\
    SWIZZLE3(name ## g, A, B, 'y')\
    SWIZZLE3(name ## b, A, B, 'z')\
    SWIZZLE3(name ## a, A, B, 'w')

#define SWIZZLE3_GEN1(name, A)\
    SWIZZLE3_GEN2(name ## 0, A, '0')\
    SWIZZLE3_GEN2(name ## 1, A, '1')\
    SWIZZLE3_GEN2(name ## x, A, 'x')\
    SWIZZLE3_GEN2(name ## y, A, 'y')\
    SWIZZLE3_GEN2(name ## z, A, 'z')\
    SWIZZLE3_GEN2(name ## w, A, 'w')\
    SWIZZLE3_GEN2(name ## r, A, 'x')\
    SWIZZLE3_GEN2(name ## g, A, 'y')\
    SWIZZLE3_GEN2(name ## b, A, 'z')\
    SWIZZLE3_GEN2(name ## a, A, 'w')

    SWIZZLE3_GEN1(_0, '0')
    SWIZZLE3_GEN1(_1, '1')
    SWIZZLE3_GEN1(x, 'x')
    SWIZZLE3_GEN1(y, 'y')
    SWIZZLE3_GEN1(z, 'z')
    SWIZZLE3_GEN1(w, 'w')
    SWIZZLE3_GEN1(r, 'x')
    SWIZZLE3_GEN1(g, 'y')
    SWIZZLE3_GEN1(b, 'z')
    SWIZZLE3_GEN1(a, 'w')

#define SWIZZLE2(name, A, B)\
    [[nodiscard]] vec name() const noexcept {\
        return swizzle<A,B,'0','w'>();\
    }

#define SWIZZLE2_GEN1(name, A)\
    SWIZZLE2(name ## 0, A, '0')\
    SWIZZLE2(name ## 1, A, '1')\
    SWIZZLE2(name ## x, A, 'x')\
    SWIZZLE2(name ## y, A, 'y')\
    SWIZZLE2(name ## z, A, 'z')\
    SWIZZLE2(name ## w, A, 'w')\
    SWIZZLE2(name ## r, A, 'x')\
    SWIZZLE2(name ## g, A, 'y')\
    SWIZZLE2(name ## b, A, 'z')\
    SWIZZLE2(name ## a, A, 'w')

    SWIZZLE2_GEN1(_0, '0')
    SWIZZLE2_GEN1(_1, '1')
    SWIZZLE2_GEN1(x, 'x')
    SWIZZLE2_GEN1(y, 'y')
    SWIZZLE2_GEN1(z, 'z')
    SWIZZLE2_GEN1(w, 'w')
    SWIZZLE2_GEN1(r, 'x')
    SWIZZLE2_GEN1(g, 'y')
    SWIZZLE2_GEN1(b, 'z')
    SWIZZLE2_GEN1(a, 'w')
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