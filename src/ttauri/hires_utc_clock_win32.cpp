// Copyright Take Vos 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "hires_utc_clock.hpp"
#include <Windows.h>

namespace tt {

hires_utc_clock::time_point hires_utc_clock::now() noexcept {
    FILETIME ts; 

    GetSystemTimePreciseAsFileTime(&ts);

    auto utc_ts = static_cast<int64_t>(ts.dwHighDateTime) << 32;
    utc_ts |= static_cast<int64_t>(ts.dwLowDateTime);

    // Convert to UNIX Epoch. Currently utc_ts is still in 100ns format.
    // Convert  1601-01-1 00:00:00 -> 1970-01-01 00:00:00
    utc_ts -= 116444736000000000;

    // Convert to 1ns format.
    utc_ts *= 100;

    return time_point(duration(utc_ts));
}

}
