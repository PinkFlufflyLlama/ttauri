// Copyright Take Vos 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>

namespace tt {

/** Bidirectional class
 * Unicode Standard Annex #9: https://unicode.org/reports/tr9/
 */
enum class unicode_bidi_class : uint8_t {
    unknown = 0,
    L = 1, ///< Left-to-Right
    R = 2, ///< Right-to-Left
    AL = 3, ///< Right-to-Left Arabic
    EN = 4, ///< European Number
    ES = 5, ///< European Number Separator
    ET = 6, ///< European Number Terminator
    AN = 7, ///< Arabic Number
    CS = 8, ///< Common Number Separator
    NSM = 9, ///< Nonspacing Mark
    BN = 10, ///< Boundary Neutral
    B = 11, ///< Paragraph Separator
    S = 12, ///< Segment Separator
    WS = 13, ///< Whitespace
    ON = 14, ///< Other Neutrals
    // Explicit values.
    LRE, ///< Left-to-Right Embedding
    LRO, ///< Left-to-Right Override
    RLE, ///< Right-to-Left Embedding
    RLO, ///< Right-to-left Override
    PDF, ///< Pop Directional Format
    LRI, ///< Left-to-Right Isolate
    RLI, ///< Right-to-Left Isolate
    FSI, ///< First Strong Isolate
    PDI ///< Pop Directional Isolate
};

[[nodiscard]] constexpr bool is_isolate_starter(unicode_bidi_class const &rhs) noexcept
{
    return rhs == tt::unicode_bidi_class::LRI
        || rhs == tt::unicode_bidi_class::RLI
        || rhs == tt::unicode_bidi_class::FSI;
}

[[nodiscard]] constexpr bool is_isolate_formatter(unicode_bidi_class const &rhs) noexcept
{
    return is_isolate_starter(rhs)
        || rhs == tt::unicode_bidi_class::PDI;
}

[[nodiscard]] constexpr bool is_NI(unicode_bidi_class const &rhs) noexcept
{
    return rhs == tt::unicode_bidi_class::B
        || rhs == tt::unicode_bidi_class::S
        || rhs == tt::unicode_bidi_class::WS
        || rhs == tt::unicode_bidi_class::ON
        || rhs == tt::unicode_bidi_class::FSI
        || rhs == tt::unicode_bidi_class::LRI
        || rhs == tt::unicode_bidi_class::RLI
        || rhs == tt::unicode_bidi_class::PDI;
}

[[nodiscard]] constexpr unicode_bidi_class unicode_bidi_class_from_string(std::string_view str) noexcept
{

    if (str == "L") {
        return tt::unicode_bidi_class::L;
    } else if (str == "R") {
        return tt::unicode_bidi_class::R;
    } else if (str == "AL") {
        return tt::unicode_bidi_class::AL;
    } else if (str == "EN") {
        return tt::unicode_bidi_class::EN;
    } else if (str == "ES") {
        return tt::unicode_bidi_class::ES;
    } else if (str == "ET") {
        return tt::unicode_bidi_class::ET;
    } else if (str == "AN") {
        return tt::unicode_bidi_class::AN;
    } else if (str == "CS") {
        return tt::unicode_bidi_class::CS;
    } else if (str == "NSM") {
        return tt::unicode_bidi_class::NSM;
    } else if (str == "BN") {
        return tt::unicode_bidi_class::BN;
    } else if (str == "B") {
        return tt::unicode_bidi_class::B;
    } else if (str == "S") {
        return tt::unicode_bidi_class::S;
    } else if (str == "WS") {
        return tt::unicode_bidi_class::WS;
    } else if (str == "ON") {
        return tt::unicode_bidi_class::ON;
    } else if (str == "LRE") {
        return tt::unicode_bidi_class::LRE;
    } else if (str == "LRO") {
        return tt::unicode_bidi_class::LRO;
    } else if (str == "RLE") {
        return tt::unicode_bidi_class::RLE;
    } else if (str == "RLO") {
        return tt::unicode_bidi_class::RLO;
    } else if (str == "PDF") {
        return tt::unicode_bidi_class::PDF;
    } else if (str == "LRI") {
        return tt::unicode_bidi_class::LRI;
    } else if (str == "RLI") {
        return tt::unicode_bidi_class::RLI;
    } else if (str == "FSI") {
        return tt::unicode_bidi_class::FSI;
    } else if (str == "PDI") {
        return tt::unicode_bidi_class::PDI;
    } else {
        tt_no_default();
    }
}

} // namespace tt
