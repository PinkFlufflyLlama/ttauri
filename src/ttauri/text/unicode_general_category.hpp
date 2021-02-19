// Copyright Take Vos 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

// TODO: When using enum is added to stable gcc use using enum over directly referencing them

#pragma once

#include <cstdint>

namespace tt {

enum class unicode_general_category : uint8_t {
    unknown,
    Lu,
    Ll,
    Lt,
    Lm,
    Lo,
    Mn,
    Mc,
    Me,
    Nd,
    Nl,
    No,
    Pc,
    Pd,
    Ps,
    Pe,
    Pi,
    Pf,
    Po,
    Sm,
    Sc,
    Sk,
    So,
    Zs,
    Zl,
    Zp,
    Cc,
    Cf,
    Cs,
    Co,
    Cn
};

[[nodiscard]] constexpr bool is_LC(unicode_general_category const &rhs) noexcept
{
    return rhs == unicode_general_category::Lu
        || rhs == unicode_general_category::Ll
        || rhs == unicode_general_category::Lt;
}

[[nodiscard]] constexpr bool is_L(unicode_general_category const &rhs) noexcept
{
    return is_LC(rhs)
        || rhs == unicode_general_category::Lm
        || rhs == unicode_general_category::Lo;
}

[[nodiscard]] constexpr bool is_M(unicode_general_category const &rhs) noexcept
{
    return rhs == unicode_general_category::Mn
        || rhs == unicode_general_category::Mc
        || rhs == unicode_general_category::Me;
}

[[nodiscard]] constexpr bool is_N(unicode_general_category const &rhs) noexcept
{
    return rhs == unicode_general_category::Nd
        || rhs == unicode_general_category::Nl
        || rhs == unicode_general_category::No;
}

[[nodiscard]] constexpr bool is_P(unicode_general_category const &rhs) noexcept
{
    return rhs == unicode_general_category::Pc
        || rhs == unicode_general_category::Pd
        || rhs == unicode_general_category::Ps
        || rhs == unicode_general_category::Pe
        || rhs == unicode_general_category::Pi
        || rhs == unicode_general_category::Pf
        || rhs == unicode_general_category::Po;
}

[[nodiscard]] constexpr bool is_S(unicode_general_category const &rhs) noexcept
{
    return rhs == unicode_general_category::Sm
        || rhs == unicode_general_category::Sc
        || rhs == unicode_general_category::Sk
        || rhs == unicode_general_category::So;
}

[[nodiscard]] constexpr bool is_Z(unicode_general_category const &rhs) noexcept
{
    return rhs == unicode_general_category::Zs
        || rhs == unicode_general_category::Zl
        || rhs == unicode_general_category::Zp;
}

[[nodiscard]] constexpr bool is_C(unicode_general_category const &rhs) noexcept
{
    return rhs == unicode_general_category::Cc
        || rhs == unicode_general_category::Cf
        || rhs == unicode_general_category::Cs
        || rhs == unicode_general_category::Co
        || rhs == unicode_general_category::Cn;
}

[[nodiscard]] constexpr bool is_visible(unicode_general_category const &rhs) noexcept
{
    return is_L(rhs) | is_M(rhs) | is_N(rhs) | is_P(rhs) | is_S(rhs);
}

} // namespace tt
