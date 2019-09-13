// Copyright 2019 Pokitec
// All rights reserved.

#pragma once

#include "TTauri/Required/globals.hpp"
#include "TTauri/Required/required.hpp"
#include <thread>
#include <string_view>
#include <functional>

namespace TTauri {

void set_thread_name(std::string_view name);

inline bool is_main_thread()
{
    return std::this_thread::get_id() == Required_globals->main_thread_id;
}

void run_on_main_thread(std::function<void()> f);


}

