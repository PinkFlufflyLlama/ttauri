// Copyright Take Vos 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "interval_vec2.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <limits>

using namespace std;
using namespace tt;

TEST(IntervalVec2, Add)
{
    ASSERT_EQ(interval_vec2(1.0f, 2.0f) + interval_vec2(3.0f, 4.0f), interval_vec2(4.0f, 6.0f));

    ASSERT_EQ(
        interval_vec2(f32x4(1.0f, 2.0f), f32x4(2.0f, 3.0f)) + interval_vec2(f32x4(3.0f, 4.0f), f32x4(4.0f, 5.0f)),
        interval_vec2(f32x4(4.0f, 6.0f), f32x4(6.0f, 8.0f)));
}

TEST(IntervalVec2, Max)
{
    ASSERT_EQ(
        max(interval_vec2(f32x4(136.0f, 56.0f), f32x4(136.0f, 59.0f)), interval_vec2(f32x4(150.0f, 0.0f), f32x4(150.0f, 0.0f))),
        interval_vec2(f32x4(150.0f, 56.0f), f32x4(150.0f, 59.0f)));

    ASSERT_EQ(
        max(interval_vec2(f32x4(1.0f, 2.0f), f32x4(2.0f, 3.0f)), interval_vec2(f32x4(3.0f, 4.0f), f32x4(4.0f, 5.0f))),
        interval_vec2(f32x4(3.0f, 4.0f), f32x4(4.0f, 5.0f)));
}
