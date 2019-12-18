// Copyright 2019 Pokitec
// All rights reserved.

#pragma once

#include <cstdint>
#include <limits>

namespace TTauri {

enum class graphic_character_t {
    none = 0x00,
    exclamation_mark = 0x01,
    double_quote = 0x02,
    hash = 0x03,
    dollar = 0x04,
    percent = 0x05,
    ampersand = 0x06,
    single_quote = 0x07,
    open_paren = 0x08,
    close_paren = 0x09,
    star = 0x0a,
    plus = 0x0b,
    comma = 0x0c,
    minus = 0x0d,
    dot = 0x0e,
    slash = 0x0f,
    colon = 0x10,
    semi_colon = 0x11,
    less_than = 0x12,
    equal = 0x13,
    greater_than = 0x14,
    question_mark = 0x15,
    open_bracket = 0x16,
    back_slash = 0x17,
    close_bracket = 0x18,
    carret = 0x19,
    underscore = 0x1a,
    back_quote = 0x1b,
    open_brace = 0x1c,
    pipe = 0x1d,
    close_brace = 0x1e,
    tilde = 0x1f
};

[[nodiscard]] constexpr graphic_character_t char_to_graphic_character(char x) noexcept
{
    switch (x) {
    case '!': return graphic_character_t::exclamation_mark;
    case '"': return graphic_character_t::double_quote;
    case '#': return graphic_character_t::hash;
    case '$': return graphic_character_t::dollar;
    case '%': return graphic_character_t::percent;
    case '&': return graphic_character_t::ampersand;
    case '\'': return graphic_character_t::single_quote;
    case '(': return graphic_character_t::open_paren;
    case ')': return graphic_character_t::close_paren;
    case '*': return graphic_character_t::star;
    case '+': return graphic_character_t::plus;
    case ',': return graphic_character_t::comma;
    case '-': return graphic_character_t::minus;
    case '.': return graphic_character_t::dot;
    case '/': return graphic_character_t::slash;
    case ':': return graphic_character_t::colon;
    case ';': return graphic_character_t::semi_colon;
    case '<': return graphic_character_t::less_than;
    case '=': return graphic_character_t::equal;
    case '>': return graphic_character_t::greater_than;
    case '?': return graphic_character_t::question_mark;
    case '[': return graphic_character_t::open_bracket;
    case '\\': return graphic_character_t::back_slash;
    case ']': return graphic_character_t::close_bracket;
    case '^': return graphic_character_t::carret;
    case '_': return graphic_character_t::underscore;
    case '`': return graphic_character_t::back_quote;
    case '{': return graphic_character_t::open_brace;
    case '|': return graphic_character_t::pipe;
    case '}': return graphic_character_t::close_brace;
    case '~': return graphic_character_t::tilde;
    default: return graphic_character_t::none;
    }
}

[[nodiscard]] constexpr uint64_t operator_to_int(char const *s) noexcept
{
    uint64_t r = 0;
    for (; *s != '\0'; s++) {
        r <<= 5;
        r |= static_cast<uint64_t>(char_to_graphic_character(*s));
    }
    return r;
}


/** Binary Operator Precedence according to C++.
 */
[[nodiscard]] uint8_t binary_operator_precedence(char const *str) noexcept {
    switch (operator_to_int(str)) {
    case operator_to_int("::"): return 1;
    case operator_to_int("("): return 2;
    case operator_to_int("["): return 2;
    case operator_to_int("."): return 2;
    case operator_to_int("->"): return 2;
    case operator_to_int(".*"): return 4;
    case operator_to_int("->*"): return 4;
    case operator_to_int("**"): return 4;
    case operator_to_int("*"): return 5;
    case operator_to_int("/"): return 5;
    case operator_to_int("%"): return 5;
    case operator_to_int("+"): return 6;
    case operator_to_int("-"): return 6;
    case operator_to_int("<<"): return 7;
    case operator_to_int(">>"): return 7;
    case operator_to_int("<=>"): return 8;
    case operator_to_int("<"): return 9;
    case operator_to_int(">"): return 9;
    case operator_to_int("<="): return 9;
    case operator_to_int(">="): return 9;
    case operator_to_int("=="): return 10;
    case operator_to_int("!="): return 10;
    case operator_to_int("&"): return 11;
    case operator_to_int("^"): return 12;
    case operator_to_int("|"): return 13;
    case operator_to_int("&&"): return 14;
    case operator_to_int("||"): return 15;
    case operator_to_int("?"): return 16;
    case operator_to_int("="): return 16;
    case operator_to_int("+="): return 16;
    case operator_to_int("-="): return 16;
    case operator_to_int("*="): return 16;
    case operator_to_int("/="): return 16;
    case operator_to_int("%="): return 16;
    case operator_to_int("<<="): return 16;
    case operator_to_int(">>="): return 16;
    case operator_to_int("&="): return 16;
    case operator_to_int("^="): return 16;
    case operator_to_int("|="): return 16;
    case operator_to_int(","): return 17;
    case operator_to_int("]"): return 17;
    case operator_to_int(")"): return 17;
    case operator_to_int("!"): return 18;
    default: return std::numeric_limits<uint8_t>::max();
    }
}

/** Operator Precedence according to C++.
 */
[[nodiscard]] uint8_t operator_precedence(char const *str, bool binary) noexcept {
    return binary ? binary_operator_precedence(str) : 3;
}

}
