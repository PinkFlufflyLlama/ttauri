// Copyright Caleb Owens 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
// This is the bit_ceil implementation based of the one in cppreference as the std::bit_ceil that is use in gnu gcc is not a constexpr
// TODO: write tests for this function

#include <concepts>
#include <bit>
#include <limits>

namespace tt {

template <std::unsigned_integral T>
    requires (!std::same_as<T, bool> && !std::same_as<T, char> &&
             !std::same_as<T, char8_t> && !std::same_as<T, char16_t> &&
             !std::same_as<T, char32_t> && !std::same_as<T, wchar_t>)
constexpr T bit_ceil(T x) noexcept
{
    if (x <= 1u)
        return T(1);
    if constexpr (std::same_as<T, decltype(+x)>)
        return T(1) << std::bit_width(T(x - 1));
    else { // for types subject to integral promotion
        constexpr int offset_for_ub =
            std::numeric_limits<unsigned>::digits - std::numeric_limits<T>::digits;
        return T(1u << (std::bit_width(T(x - 1)) + offset_for_ub) >> offset_for_ub);
    }
}

} // namespace tt