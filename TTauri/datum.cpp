// Copyright 2019 Pokitec
// All rights reserved.

#include "datum.hpp"
#include "exceptions.hpp"
#include <fmt/ostream.h>
#include <fmt/format.h>

namespace TTauri {

void datum::delete_pointer() noexcept {
    switch (type_id()) {
    case phy_integer_ptr_id: delete get_pointer<int64_t>(); break;
    case phy_string_ptr_id: delete get_pointer<std::string>(); break;
    case phy_url_ptr_id: delete get_pointer<URL>(); break;
    case phy_vector_ptr_id: delete get_pointer<datum::vector>(); break;
    case phy_map_ptr_id: delete get_pointer<datum::map>(); break;
    case phy_wsrgba_ptr_id: delete get_pointer<wsRGBA>(); break;
    default: no_default;
    }
    u64 = undefined_mask;
}

void datum::copy_pointer(datum const &other) noexcept {
    switch (other.type_id()) {
    case phy_integer_ptr_id: {
        auto *p = new int64_t(*other.get_pointer<int64_t>());
        u64 = integer_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    } break;

    case phy_string_ptr_id: {
        auto *p = new std::string(*other.get_pointer<std::string>());
        u64 = string_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    } break;

    case phy_url_ptr_id: {
        auto *p = new URL(*other.get_pointer<URL>());
        u64 = url_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    } break;

    case phy_vector_ptr_id: {
        auto *p = new datum::vector(*other.get_pointer<datum::vector>());
        u64 = vector_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    } break;

    case phy_map_ptr_id: {
        auto *p = new datum::map(*other.get_pointer<datum::map>());
        u64 = map_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    } break;

    case phy_wsrgba_ptr_id: {
        auto *p = new wsRGBA(*other.get_pointer<wsRGBA>());
        u64 = wsrgba_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    } break;

    default:
        no_default;
    }
}

datum::datum(std::string_view value) noexcept : u64(make_string(value)) {
    if (value.size() > 6) {
        // Overflow.
        auto p = new std::string(value);
        u64 = string_ptr_mask | reinterpret_cast<uint64_t>(p);
    }
}

datum::datum(URL const &value) noexcept {
    auto p = new URL(value);
    u64 = url_ptr_mask | reinterpret_cast<uint64_t>(p);
}

datum::datum(datum::vector const &value) noexcept {
    auto p = new datum::vector(value);
    u64 = vector_ptr_mask | reinterpret_cast<uint64_t>(p);
}

datum::datum(datum::map const &value) noexcept {
    auto p = new datum::map(value);
    u64 = map_ptr_mask | reinterpret_cast<uint64_t>(p);
}

datum::datum(wsRGBA const &value) noexcept {
    auto p = new wsRGBA(value);
    u64 = wsrgba_ptr_mask | reinterpret_cast<uint64_t>(p);
}

datum::operator double() const {
    if (is_phy_float()) {
        return f64;
    } else if (is_phy_integer()) {
        return static_cast<double>(get_signed_integer());
    } else if (is_phy_integer_ptr()) {
        return static_cast<double>(*get_pointer<int64_t>());
    } else {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a double", this->repr(), this->type_name()));
    }
}

datum::operator float() const {
    return static_cast<float>(static_cast<double>(*this));
}

datum::operator int64_t() const {
    if (is_phy_integer()) {
        return get_signed_integer();
    } else if (is_phy_integer_ptr()) {
        return *get_pointer<int64_t>();
    } else if (is_phy_float()) {
        return static_cast<int64_t>(f64);
    } else if (is_phy_boolean()) {
        return get_unsigned_integer() > 0 ? 1 : 0;
    } else {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a int64_t", this->repr(), this->type_name()));
    }
}

datum::operator int32_t() const {
    let v = static_cast<int64_t>(*this);
    if (v < std::numeric_limits<int32_t>::min() || v > std::numeric_limits<int32_t>::max()) {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a int32_t", this->repr(), this->type_name()));
    }
    return static_cast<int32_t>(v);
}

datum::operator int16_t() const {
    let v = static_cast<int64_t>(*this);
    if (v < std::numeric_limits<int16_t>::min() || v > std::numeric_limits<int16_t>::max()) {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a int16_t", this->repr(), this->type_name()));
    }
    return static_cast<int16_t>(v);
}

datum::operator int8_t() const {
    let v = static_cast<int64_t>(*this);
    if (v < std::numeric_limits<int8_t>::min() || v > std::numeric_limits<int8_t>::max()) {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a int8_t", this->repr(), this->type_name()));
    }
    return static_cast<int8_t>(v);
}

datum::operator uint64_t() const {
    let v = static_cast<int64_t>(*this);
    return static_cast<uint64_t>(v);
}

datum::operator uint32_t() const {
    let v = static_cast<uint64_t>(*this);
    if ( v > std::numeric_limits<uint32_t>::max()) {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a uint32_t", this->repr(), this->type_name()));
    }
    return static_cast<uint32_t>(v);
}

datum::operator uint16_t() const {
    let v = static_cast<uint64_t>(*this);
    if (v > std::numeric_limits<uint16_t>::max()) {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a uint16_t", this->repr(), this->type_name()));
    }
    return static_cast<uint16_t>(v);
}

datum::operator uint8_t() const {
    let v = static_cast<uint64_t>(*this);
    if (v > std::numeric_limits<uint8_t>::max()) {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a uint8_t", this->repr(), this->type_name()));
    }
    return static_cast<uint8_t>(v);
}

datum::operator bool() const noexcept {
    switch (type_id()) {
    case phy_boolean_id: return get_unsigned_integer() > 0;
    case phy_null_id: return false;
    case phy_undefined_id: return false;
    case phy_integer_id0:
    case phy_integer_id1:
    case phy_integer_id2:
    case phy_integer_id3:
    case phy_integer_id4:
    case phy_integer_id5:
    case phy_integer_id6:
    case phy_integer_id7: return static_cast<int64_t>(*this) != 0;
    case phy_integer_ptr_id: return *get_pointer<int64_t>() != 0;
    case phy_string_id0:
    case phy_string_id1:
    case phy_string_id2:
    case phy_string_id3:
    case phy_string_id4:
    case phy_string_id5:
    case phy_string_id6:
    case phy_string_ptr_id: return this->size() > 0;
    case phy_url_ptr_id: return true;
    case phy_vector_ptr_id: return this->size() > 0;
    case phy_map_ptr_id: return this->size() > 0;
    case phy_wsrgba_ptr_id: return !(get_pointer<wsRGBA>()->isTransparent());
    default:
        if (ttauri_likely(is_phy_float())) {
            return static_cast<double>(*this) != 0.0;
        } else {
            no_default;
        };
    }
}

datum::operator char() const {
    if (is_phy_string() && size() == 1) {
        return u64 & 0xff;
    } else if (is_phy_string_ptr() && size() == 1) {
        return get_pointer<std::string>()->at(0);
    } else {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a char", this->repr(), this->type_name()));
    }
}

datum::operator std::string() const noexcept {
    switch (type_id()) {
    case phy_boolean_id: return static_cast<bool>(*this) ? "true" : "false";
    case phy_null_id: return "null";
    case phy_undefined_id: return "undefined";
    case phy_integer_id0:
    case phy_integer_id1:
    case phy_integer_id2:
    case phy_integer_id3:
    case phy_integer_id4:
    case phy_integer_id5:
    case phy_integer_id6:
    case phy_integer_id7: return fmt::format("{}", static_cast<int64_t>(*this));
    case phy_integer_ptr_id: return fmt::format("{}", static_cast<int64_t>(*this));
    case phy_string_id0:
    case phy_string_id1:
    case phy_string_id2:
    case phy_string_id3:
    case phy_string_id4:
    case phy_string_id5:
    case phy_string_id6: {
        let length = size();
        char buffer[6];
        for (int i = 0; i < length; i++) {
            buffer[i] = (u64 >> ((length - i - 1) * 8)) & 0xff;
        }
        return std::string(buffer, length);
    }
    case phy_string_ptr_id: return *get_pointer<std::string>();
    case phy_url_ptr_id: return get_pointer<URL>()->string();
    case phy_vector_ptr_id: {
        std::string r = "[";
        auto count = 0;
        for (let &v: *get_pointer<datum::vector>()) {
            if (count++ > 0) {
                r += ", ";
            }
            r += v.repr();
        }
        r += "]";
        return r;
    }
    case phy_map_ptr_id: {
        let *m = get_pointer<datum::map>();
        auto keys = transform<datum::vector>(*m, [](let &x) {
            return x.first;
        }); 
        std::sort(keys.begin(), keys.end());

        std::string r = "{";
        auto count = 0;
        for (let &key: keys) {
            let it = m->find(key);
            required_assert(it != m->end());
            let &value = it->second;

            if (count++ > 0) {
                r += ", ";
            }
            r += key.repr();
            r += ": ";
            r += value.repr();
        }
        r += "}";
        return r;
    }
    case phy_wsrgba_ptr_id: return to_string(*get_pointer<wsRGBA>());
    default:
        if (is_phy_float()) {
            auto str = fmt::format("{:g}", static_cast<double>(*this));
            if (str.find('.') == str.npos) {
                str += ".0";
            }
            return str;
        } else {
            no_default;
        }
    }
}

datum::operator URL() const {
    if (is_string()) {
        return URL{static_cast<std::string>(*this)};
    } else if (is_url()) {
        return *get_pointer<URL>();
    } else {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a URL", this->repr(), this->type_name()));
    }
}

datum::operator datum::vector() const {
    if (is_vector()) {
        return *get_pointer<datum::vector>();
    } else {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a Vector", this->repr(), this->type_name()));
    }
}

datum::operator datum::map() const {
    if (is_map()) {
        return *get_pointer<datum::map>();
    } else {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a Map", this->repr(), this->type_name()));
    }
}

datum::operator wsRGBA() const {
    if (is_wsrgba()) {
        return *get_pointer<wsRGBA>();
    } else {
        TTAURI_THROW(invalid_operation_error("Value {} of type {} can not be converted to a wsRGBA", this->repr(), this->type_name()));
    }
}

datum datum::operator~() const {
    if (is_integer()) {
        return datum{~static_cast<int64_t>(*this)};
    } else {
        TTAURI_THROW(invalid_operation_error("Can't bit-wise negate '~' value {} of type {}",
            repr(), type_name()
        ));
    }
}
datum datum::operator-() const {
    if (is_integer()) {
        return datum{-static_cast<int64_t>(*this)};
    } else if (is_float()) {
        return datum{-static_cast<double>(*this)};
    } else {
        TTAURI_THROW(invalid_operation_error("Can't arithmatic negate '-' value {} of type {}",
            repr(), type_name()
        ));
    }
}

datum &datum::operator[](datum const &rhs) {
    if (is_undefined()) {
        // When accessing a name on an undefined it means we need replace it with an empty map.
        auto *p = new datum::map();
        u64 = map_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    }

    if (is_map()) {
        auto *m = get_pointer<datum::map>();
        auto [i, did_insert] = m->try_emplace(rhs);
        return i->second;

    } else if (is_vector() && rhs.is_integer()) {
        let index = static_cast<int64_t>(rhs);
        auto *v = get_pointer<datum::vector>();

        if (index < 0 || index >= to_int64(v->size())) {
            TTAURI_THROW(invalid_operation_error("Index {} out of range to access value in vector of size {}", index, v->size()));
        } else {
            return (*v)[index];
        }
    } else {
        TTAURI_THROW(invalid_operation_error("Cannot index value of type {} with {} of type {}", type_name(), rhs.repr(), rhs.type_name()));
    }
}

datum datum::operator[](datum const &rhs) const {
    if (is_map()) {
        auto *m = get_pointer<datum::map>();
        let i = m->find(rhs);
        if (i == m->end()) {
            TTAURI_THROW(invalid_operation_error("Could not find key {} in map of size {}", rhs.repr(), m->size()));
        }
        return i->second;

    } else if (is_vector() && rhs.is_integer()) {
        let index = static_cast<int64_t>(rhs);
        auto *v = get_pointer<datum::vector>();

        if (index < 0 || index >= to_int64(v->size())) {
            TTAURI_THROW(invalid_operation_error("Index {} out of range to access value in vector of size {}", index, v->size()));
        } else {
            return (*v)[index];
        }
    } else {
        TTAURI_THROW(invalid_operation_error("Cannot index value of type {} with {} of type {}", type_name(), rhs.repr(), rhs.type_name()));
    }
}

datum &datum::append() {
    if (is_undefined()) {
        // When appending on undefined it means we need replace it with an empty vector.
        auto *p = new datum::vector();
        u64 = vector_ptr_mask | (reinterpret_cast<uint64_t>(p) & pointer_mask);
    }

    if (is_vector()) {
        auto *v = get_pointer<datum::vector>();
        v->emplace_back();
        return v->back();

    } else {
        TTAURI_THROW(invalid_operation_error("Cannot append new item onto type {}", type_name()));
    }
}

char const *datum::type_name() const noexcept
{
    switch (type_id()) {
    case phy_boolean_id: return "Boolean";
    case phy_null_id: return "Null";
    case phy_undefined_id: return "Undefined";
    case phy_integer_id0:
    case phy_integer_id1:
    case phy_integer_id2:
    case phy_integer_id3:
    case phy_integer_id4:
    case phy_integer_id5:
    case phy_integer_id6:
    case phy_integer_id7:
    case phy_integer_ptr_id: return "Integer";
    case phy_string_id0:
    case phy_string_id1:
    case phy_string_id2:
    case phy_string_id3:
    case phy_string_id4:
    case phy_string_id5:
    case phy_string_id6:
    case phy_string_ptr_id: return "String";
    case phy_url_ptr_id: return "URL";
    case phy_vector_ptr_id: return "Vector";
    case phy_map_ptr_id: return "Map";
    case phy_wsrgba_ptr_id: return "wsRGBA";
    default:
        if (ttauri_likely(is_phy_float())) {
            return "Float";
        } else {
            no_default;
        }
    }
}

std::string datum::repr() const noexcept
{
    switch (type_id()) {
    case phy_boolean_id: return static_cast<std::string>(*this);
    case phy_null_id: return static_cast<std::string>(*this);
    case phy_undefined_id: return static_cast<std::string>(*this);
    case phy_integer_id0:
    case phy_integer_id1:
    case phy_integer_id2:
    case phy_integer_id3:
    case phy_integer_id4:
    case phy_integer_id5:
    case phy_integer_id6:
    case phy_integer_id7:
    case phy_integer_ptr_id: return static_cast<std::string>(*this);
    case phy_string_id0:
    case phy_string_id1:
    case phy_string_id2:
    case phy_string_id3:
    case phy_string_id4:
    case phy_string_id5:
    case phy_string_id6:
    case phy_string_ptr_id: return fmt::format("\"{}\"", static_cast<std::string>(*this));
    case phy_url_ptr_id: return fmt::format("<URL {}>", static_cast<std::string>(*this));
    case phy_vector_ptr_id: return static_cast<std::string>(*this);
    case phy_map_ptr_id: return static_cast<std::string>(*this);
    case phy_wsrgba_ptr_id: return fmt::format("<wsRGBA {}>", static_cast<std::string>(*this));
    default:
        if (ttauri_likely(is_phy_float())) {
            return static_cast<std::string>(*this);
        } else {
            no_default;
        }
    }
}

size_t datum::size() const
{
    switch (type_id()) {
    case phy_string_id0:
    case phy_string_id1:
    case phy_string_id2:
    case phy_string_id3:
    case phy_string_id4:
    case phy_string_id5:
    case phy_string_id6: return to_int(((u64 & 0xffff'0000'0000'0000ULL) - string_mask) >> 48);
    case phy_string_ptr_id: return get_pointer<std::string>()->size();
    case phy_vector_ptr_id: return get_pointer<datum::vector>()->size();
    case phy_map_ptr_id: return get_pointer<datum::map>()->size();
    case phy_wsrgba_ptr_id: return 4;
    default: TTAURI_THROW(invalid_operation_error("Can't get size of value {} of type {}.", this->repr(), this->type_name()));
    }
}

size_t datum::hash() const noexcept
{
    if (is_phy_float()) {
        return std::hash<double>{}(f64);
    } else if (ttauri_unlikely(is_phy_pointer())) {
        switch (type_id()) {
        case phy_string_ptr_id: return std::hash<std::string>{}(*get_pointer<std::string>());
        case phy_url_ptr_id: return std::hash<URL>{}(*get_pointer<URL>());
        case phy_vector_ptr_id: return std::hash<datum::vector>{}(*get_pointer<datum::vector>());
        case phy_map_ptr_id: return std::hash<double>{}(f64);
        case phy_wsrgba_ptr_id: return std::hash<wsRGBA>{}(*get_pointer<wsRGBA>());
        default: no_default;
        }
    } else {
        return std::hash<uint64_t>{}(u64);
    }
}

datum &datum::get_by_path(std::vector<std::string> const &key) {
    if (key.size() > 0 && is_map()) {
        let index = key.at(0);
        auto &next = (*this)[index];
        let next_key = std::vector<std::string>{key.begin() + 1, key.end()};
        return next.get_by_path(next_key);

    } else if (key.size() > 0 && is_vector()) {
        size_t const index = std::stoll(key.at(0));
        auto &next = (*this)[index];
        let next_key = std::vector<std::string>{key.begin() + 1, key.end()};
        return next.get_by_path(next_key);

    } else if (key.size() > 0) {
        TTAURI_THROW(invalid_operation_error("type {0} does not support get() with '{1}'", type_name(), key.at(0)));
    } else {
        return *this;
    }
}

datum datum::get_by_path(std::vector<std::string> const &key) const {
    if (key.size() > 0 && is_map()) {
        let index = key.at(0);
        let next = (*this)[index];
        return next.get_by_path({key.begin() + 1, key.end()});

    } else if (key.size() > 0 && is_vector()) {
        size_t const index = std::stoll(key.at(0));
        let next = (*this)[index];
        return next.get_by_path({key.begin() + 1, key.end()});

    } else if (key.size() > 0) {
        TTAURI_THROW(invalid_operation_error("type {0} does not support get() with '{1}'", type_name(), key.at(0)));
    } else {
        return *this;
    }
}

std::string to_string(datum const &d)
{
    return static_cast<std::string>(d);
}

std::ostream &operator<<(std::ostream &os, datum const &d)
{
    return os << static_cast<std::string>(d);
}


bool operator==(datum const &lhs, datum const &rhs) noexcept
{
    switch (lhs.type_id()) {
    case datum::phy_boolean_id:
        return rhs.is_boolean() && static_cast<bool>(lhs) == static_cast<bool>(rhs);
    case datum::phy_null_id:
        return rhs.is_null();
    case datum::phy_undefined_id:
        return rhs.is_undefined();
    case datum::phy_integer_id0:
    case datum::phy_integer_id1:
    case datum::phy_integer_id2:
    case datum::phy_integer_id3:
    case datum::phy_integer_id4:
    case datum::phy_integer_id5:
    case datum::phy_integer_id6:
    case datum::phy_integer_id7:
    case datum::phy_integer_ptr_id:
        return (
            (rhs.is_float() && static_cast<double>(lhs) == static_cast<double>(rhs)) ||
            (rhs.is_integer() && static_cast<int64_t>(lhs) == static_cast<int64_t>(rhs))
        );
    case datum::phy_string_id0:
    case datum::phy_string_id1:
    case datum::phy_string_id2:
    case datum::phy_string_id3:
    case datum::phy_string_id4:
    case datum::phy_string_id5:
    case datum::phy_string_id6:
    case datum::phy_string_ptr_id:
        return (
            (rhs.is_string() && static_cast<std::string>(lhs) == static_cast<std::string>(rhs)) ||
            (rhs.is_url() && static_cast<URL>(lhs) == static_cast<URL>(rhs))
        );
    case datum::phy_url_ptr_id:
        return (rhs.is_url() || rhs.is_string()) && static_cast<URL>(lhs) == static_cast<URL>(rhs);
    case datum::phy_vector_ptr_id:
        return rhs.is_vector() && *lhs.get_pointer<datum::vector>() == *rhs.get_pointer<datum::vector>();
    case datum::phy_map_ptr_id:
        return rhs.is_map() && *lhs.get_pointer<datum::map>() == *rhs.get_pointer<datum::map>();
    case datum::phy_wsrgba_ptr_id:
        return rhs.is_wsrgba() && *lhs.get_pointer<wsRGBA>() == *rhs.get_pointer<wsRGBA>();
    default:
        if (lhs.is_phy_float()) {
            return rhs.is_numeric() && static_cast<double>(lhs) == static_cast<double>(rhs);
        } else {
            no_default;
        }
    }
}

bool operator<(datum::map const &lhs, datum::map const &rhs) noexcept
{
    auto lhs_keys = transform<datum::vector>(lhs, [](auto x) { return x.first; });
    auto rhs_keys = transform<datum::vector>(lhs, [](auto x) { return x.first; });

    if (lhs_keys == rhs_keys) {
        for (let &k: lhs_keys) {
            if (lhs.at(k) == rhs.at(k)) {
                continue;
            } else if (lhs.at(k) < rhs.at(k)) {
                return true;
            } else {
                return false;
            }
        }
    } else {
        return lhs_keys < rhs_keys;
    }
    return false;
}

bool operator<(datum const &lhs, datum const &rhs) noexcept
{
    switch (lhs.type_id()) {
    case datum::phy_boolean_id:
        if (rhs.is_boolean()) {
            return static_cast<bool>(lhs) < static_cast<bool>(rhs);
        } else {
            return lhs.type_order() < rhs.type_order();
        }
    case datum::phy_null_id:
        return lhs.type_order() < rhs.type_order();
    case datum::phy_undefined_id:
        return lhs.type_order() < rhs.type_order();
    case datum::phy_integer_id0:
    case datum::phy_integer_id1:
    case datum::phy_integer_id2:
    case datum::phy_integer_id3:
    case datum::phy_integer_id4:
    case datum::phy_integer_id5:
    case datum::phy_integer_id6:
    case datum::phy_integer_id7:
    case datum::phy_integer_ptr_id:
        if (rhs.is_float()) {
            return static_cast<double>(lhs) < static_cast<double>(rhs);
        } else if (rhs.is_integer()) {
            return static_cast<int64_t>(lhs) < static_cast<int64_t>(rhs);
        } else {
            return lhs.type_order() < rhs.type_order();
        }
    case datum::phy_string_id0:
    case datum::phy_string_id1:
    case datum::phy_string_id2:
    case datum::phy_string_id3:
    case datum::phy_string_id4:
    case datum::phy_string_id5:
    case datum::phy_string_id6:
    case datum::phy_string_ptr_id:
        if (rhs.is_string()) {
            return static_cast<std::string>(lhs) < static_cast<std::string>(rhs);
        } else if (rhs.is_url()) {
            return static_cast<URL>(lhs) < static_cast<URL>(rhs);
        } else {
            return lhs.type_order() < rhs.type_order();
        }
    case datum::phy_url_ptr_id:
        if (rhs.is_url() || rhs.is_string()) {
            return static_cast<URL>(lhs) < static_cast<URL>(rhs);
        } else {
            return lhs.type_order() < rhs.type_order();
        }
    case datum::phy_vector_ptr_id:
        if (rhs.is_vector()) {
            return *lhs.get_pointer<datum::vector>() < *rhs.get_pointer<datum::vector>();
        } else {
            return lhs.type_order() < rhs.type_order();
        }
    case datum::phy_map_ptr_id:
        if (rhs.is_map()) {
            return *lhs.get_pointer<datum::map>() < *rhs.get_pointer<datum::map>();
        } else {
            return lhs.type_order() < rhs.type_order();
        }
    case datum::phy_wsrgba_ptr_id:
        if (rhs.is_wsrgba()) {
            return *lhs.get_pointer<wsRGBA>() < *rhs.get_pointer<wsRGBA>();
        } else {
            return lhs.type_order() < rhs.type_order();
        }
    default:
        if (lhs.is_phy_float()) {
            if (rhs.is_numeric()) {
                return static_cast<double>(lhs) < static_cast<double>(rhs);
            } else {
                return lhs.type_order() < rhs.type_order();
            }
        } else {
            no_default;
        }
    }
}

datum operator+(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<int64_t>(lhs);
        let rhs_ = static_cast<int64_t>(rhs);
        return datum{lhs_ + rhs_};

    } else if (lhs.is_numeric() && rhs.is_numeric()) {
        let lhs_ = static_cast<double>(lhs);
        let rhs_ = static_cast<double>(rhs);
        return datum{lhs_ + rhs_};

    } else if (lhs.is_string() && rhs.is_string()) {
        let lhs_ = static_cast<std::string>(lhs);
        let rhs_ = static_cast<std::string>(rhs);
        return datum{std::move(lhs_ + rhs_)};

    } else if (lhs.is_vector() && rhs.is_vector()) {
        auto lhs_ = static_cast<datum::vector>(lhs);
        let &rhs_ = *(rhs.get_pointer<datum::vector>());
        std::copy(rhs_.begin(), rhs_.end(), std::back_inserter(lhs_));
        return datum{std::move(lhs_)};

    } else if (lhs.is_map() && rhs.is_map()) {
        let &lhs_ = *(lhs.get_pointer<datum::map>());
        auto rhs_ = static_cast<datum::map>(rhs);
        for (let &item: lhs_) {
            rhs_.try_emplace(item.first, item.second);
        }
        return datum{std::move(rhs_)};

    } else if (lhs.is_wsrgba() && rhs.is_wsrgba()) {
        auto lhs_ = static_cast<wsRGBA>(lhs);
        lhs_.composit(*(rhs.get_pointer<wsRGBA>()));
        return datum{lhs_};

    } else {
        TTAURI_THROW(invalid_operation_error("Can't add '+' value {} of type {} to value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}

datum operator-(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<int64_t>(lhs);
        let rhs_ = static_cast<int64_t>(rhs);
        return datum{lhs_ - rhs_};

    } else if (lhs.is_numeric() && rhs.is_numeric()) {
        let lhs_ = static_cast<double>(lhs);
        let rhs_ = static_cast<double>(rhs);
        return datum{lhs_ - rhs_};

    } else {
        TTAURI_THROW(invalid_operation_error("Can't subtract '-' value {} of type {} from value {} of type {}",
            rhs.repr(), rhs.type_name(), lhs.repr(), lhs.type_name()
        ));
    }
}

datum operator*(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<int64_t>(lhs);
        let rhs_ = static_cast<int64_t>(rhs);
        return datum{lhs_ * rhs_};

    } else if (lhs.is_numeric() && rhs.is_numeric()) {
        let lhs_ = static_cast<double>(lhs);
        let rhs_ = static_cast<double>(rhs);
        return datum{lhs_ * rhs_};

    } else {
        TTAURI_THROW(invalid_operation_error("Can't multiply '+' value {} of type {} with value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}

datum operator/(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<int64_t>(lhs);
        let rhs_ = static_cast<int64_t>(rhs);
        return datum{lhs_ / rhs_};

    } else if (lhs.is_numeric() && rhs.is_numeric()) {
        let lhs_ = static_cast<double>(lhs);
        let rhs_ = static_cast<double>(rhs);
        return datum{lhs_ / rhs_};

        // XXX implement path concatenation.
    } else {
        TTAURI_THROW(invalid_operation_error("Can't divide '/' value {} of type {} by value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}

datum operator%(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<int64_t>(lhs);
        let rhs_ = static_cast<int64_t>(rhs);
        return datum{lhs_ % rhs_};

    } else if (lhs.is_numeric() && rhs.is_numeric()) {
        let lhs_ = static_cast<double>(lhs);
        let rhs_ = static_cast<double>(rhs);
        return datum{std::fmod(lhs_,rhs_)};

    } else {
        TTAURI_THROW(invalid_operation_error("Can't take modulo '%' value {} of type {} by value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}

datum operator<<(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<uint64_t>(lhs);
        let rhs_ = static_cast<int64_t>(rhs);
        if (rhs_ < -63) {
            return datum{0};
        } else if (rhs_ < 0) {
            // Pretent this is a unsigned shift right.
            return datum{lhs_ >> -rhs_};
        } else if (rhs_ == 0) {
            return lhs;
        } else if (rhs_ > 63) {
            return datum{0};
        } else {
            return datum{lhs_ << rhs_};
        }

    } else {
        TTAURI_THROW(invalid_operation_error("Can't logical shift-left '<<' value {} of type {} with value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}

datum operator>>(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<uint64_t>(lhs);
        let rhs_ = static_cast<int64_t>(rhs);
        if (rhs_ < -63) {
            return datum{0};
        } else if (rhs_ < 0) {
            return datum{lhs_ << -rhs_};
        } else if (rhs_ == 0) {
            return lhs;
        } else if (rhs_ > 63) {
            return (lhs_ >= 0) ? datum{0} : datum{-1};
        } else {
            return datum{static_cast<int64_t>(lhs_) >> rhs_};
        }

    } else {
        TTAURI_THROW(invalid_operation_error("Can't arithmatic shift-right '>>' value {} of type {} with value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}
datum operator&(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<uint64_t>(lhs);
        let rhs_ = static_cast<uint64_t>(rhs);
        return datum{lhs_ & rhs_};

    } else {
        TTAURI_THROW(invalid_operation_error("Can't AND '&' value {} of type {} with value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}

datum operator|(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<uint64_t>(lhs);
        let rhs_ = static_cast<uint64_t>(rhs);
        return datum{lhs_ | rhs_};

    } else {
        TTAURI_THROW(invalid_operation_error("Can't OR '|' value {} of type {} with value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}

datum operator^(datum const &lhs, datum const &rhs)
{
    if (lhs.is_integer() && rhs.is_integer()) {
        let lhs_ = static_cast<uint64_t>(lhs);
        let rhs_ = static_cast<uint64_t>(rhs);
        return datum{lhs_ ^ rhs_};

    } else {
        TTAURI_THROW(invalid_operation_error("Can't XOR '^' value {} of type {} with value {} of type {}",
            lhs.repr(), lhs.type_name(), rhs.repr(), rhs.type_name()
        ));
    }
}



}