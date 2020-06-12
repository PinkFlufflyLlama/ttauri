// Copyright 2019 Pokitec
// All rights reserved.

#pragma once

#include "TTauri/Foundation/os_detect.hpp"

#if OPERATING_SYSTEM == OS_WINDOWS
#include "TTauri/GUI/Window_vulkan_win32.hpp"
namespace tt {
using Window = Window_vulkan_win32;
}

#elif OPERATING_SYSTEM == OS_MACOS
#include "TTauri/GUI/Window_vulkan_macos.hpp"
namespace tt {
using Window = Window_vulkan_macos;
}

#else
#error "Window not implemented for this os."
#endif
