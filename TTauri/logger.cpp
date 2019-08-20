// Copyright 2019 Pokitec
// All rights reserved.

#include "logger.hpp"
#include "strings.hpp"
#include "os_detect.hpp"
#include <exception>
#include <memory>
#include <iostream>
#include <chrono>

#if OPERATING_SYSTEM == OS_WINDOWS
#include <Windows.h>
#endif

namespace TTauri {

using namespace std::literals::chrono_literals;

void logger::writeToFile(std::string_view str) noexcept {
}

void logger::writeToConsole(std::string_view str) noexcept {
#if OPERATING_SYSTEM == OS_WINDOWS
#else
    cerr << str << endl;
#endif
}

/*! Write to a log file and console.
 * This will write to the console if one is open.
 * It will also create a log file in the application-data directory.
 */
void logger::write(std::string_view str) noexcept {
    writeToFile(str);
    writeToConsole(str);
}

void logger::loop() noexcept {
    while (!logger_thread_stop) {
        while (message_queue.size() > 0) {
            let &message = message_queue.peek();
            let str = message->string();
            message_queue.pop();
            write(str);
        }

        std::this_thread::sleep_for(100ms);
    }
}

gsl_suppress(i.11)
std::string getLastErrorMessage()
{
    DWORD const errorCode = GetLastError();
    size_t const messageSize = 32768;
    wchar_t* const c16_message = new wchar_t[messageSize];;

    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, // source
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        c16_message,
        messageSize,
        NULL
    );

    let message = translateString<std::string>(std::wstring(c16_message));
    delete [] c16_message;

    return message;
}

}