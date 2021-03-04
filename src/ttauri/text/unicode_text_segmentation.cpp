// Copyright Take Vos 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "unicode_text_segmentation.hpp"
#include "unicode_description.hpp"
#include "../required.hpp"

namespace tt {

[[nodiscard]] static bool
breaks_grapheme(unicode_grapheme_cluster_break cluster_break, grapheme_break_state &state) noexcept
{
    // TODO: Use using enum when added to GCC
    ttlet lhs = state.previous;
    ttlet rhs = cluster_break;

    enum class break_state {
        unknown,
        do_break,
        dont_break,
    };

    break_state break_state = break_state::unknown;

    bool GB1 = state.first_character;
    if ((break_state == break_state::unknown) & GB1) {
        break_state = break_state::do_break;
    }

    state.first_character = false;

    ttlet GB3 = (lhs == unicode_grapheme_cluster_break::CR) && (rhs == unicode_grapheme_cluster_break::LF);
    ttlet GB4 = (lhs == unicode_grapheme_cluster_break::Control) || (lhs == unicode_grapheme_cluster_break::CR) || (lhs == unicode_grapheme_cluster_break::LF);
    ttlet GB5 = (rhs == unicode_grapheme_cluster_break::Control) || (rhs == unicode_grapheme_cluster_break::CR) || (rhs == unicode_grapheme_cluster_break::LF);
    if (break_state == break_state::unknown) {
        if (GB3) {
            break_state = break_state::dont_break;
        } else if (GB4 || GB5) {
            break_state = break_state::do_break;
        }
    }

    ttlet GB6 = (lhs == unicode_grapheme_cluster_break::L) &&
        ((rhs == unicode_grapheme_cluster_break::L) || (rhs == unicode_grapheme_cluster_break::V) ||
         (rhs == unicode_grapheme_cluster_break::LV) | (rhs == unicode_grapheme_cluster_break::LVT));
    ttlet GB7 = ((lhs == unicode_grapheme_cluster_break::LV) || (lhs == unicode_grapheme_cluster_break::V)) &&
        ((rhs == unicode_grapheme_cluster_break::V) || (rhs == unicode_grapheme_cluster_break::T));
    ttlet GB8 = ((lhs == unicode_grapheme_cluster_break::LVT) || (lhs == unicode_grapheme_cluster_break::T)) && (rhs == unicode_grapheme_cluster_break::T);
    if ((break_state == break_state::unknown) && (GB6 || GB7 || GB8)) {
        break_state = break_state::dont_break;
    }

    ttlet GB9 = ((rhs == unicode_grapheme_cluster_break::Extend) || (rhs == unicode_grapheme_cluster_break::ZWJ));
    ttlet GB9a = (rhs == unicode_grapheme_cluster_break::SpacingMark);
    ttlet GB9b = (lhs == unicode_grapheme_cluster_break::Prepend);
    if ((break_state == break_state::unknown) & (GB9 || GB9a || GB9b)) {
        break_state = break_state::dont_break;
    }

    ttlet GB11 =
        state.in_extended_pictograph && (lhs == unicode_grapheme_cluster_break::ZWJ) && (rhs == unicode_grapheme_cluster_break::Extended_Pictographic);
    if ((break_state == break_state::unknown) && GB11) {
        break_state = break_state::dont_break;
    }

    if (rhs == unicode_grapheme_cluster_break::Extended_Pictographic) {
        state.in_extended_pictograph = true;
    } else if (!((rhs == unicode_grapheme_cluster_break::Extend) || (rhs == unicode_grapheme_cluster_break::ZWJ))) {
        state.in_extended_pictograph = false;
    }

    ttlet GB12_13 = (lhs == unicode_grapheme_cluster_break::Regional_Indicator) && (rhs == unicode_grapheme_cluster_break::Regional_Indicator) &&
        ((state.RI_count % 2) == 1);
    if ((break_state == break_state::unknown) && (GB12_13)) {
        break_state = break_state::dont_break;
    }

    if (rhs == unicode_grapheme_cluster_break::Regional_Indicator) {
        state.RI_count++;
    } else {
        state.RI_count = 0;
    }

    // GB999
    if (break_state == break_state::unknown) {
        break_state = break_state::do_break;
    }

    state.previous = rhs;
    return break_state == break_state::do_break;
}


[[nodiscard]] bool breaks_grapheme(char32_t code_point, grapheme_break_state &state) noexcept
{
    return breaks_grapheme(unicode_description_find(code_point).grapheme_cluster_break(), state);
}

}
