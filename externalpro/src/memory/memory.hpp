#pragma once

#include <Windows.h>
#include <string>
#include "../globals.hpp"

namespace memory
{
    template<typename T>
    T read(uintptr_t address)
    {
        T value{};
        ReadProcessMemory(globals::handle, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
        return value;
    }

    template<typename T>
    bool write(uintptr_t address, const T& buffer)
    {
        SIZE_T bytes_written = 0;
        return WriteProcessMemory(globals::handle, reinterpret_cast<LPVOID>(address), &buffer, sizeof(T), &bytes_written) && bytes_written == sizeof(T);
    }

    template<typename T>
    T read_module(uintptr_t address)
    {
        T value{};
        ReadProcessMemory(globals::handle, reinterpret_cast<LPCVOID>(address + globals::base), &value, sizeof(T), nullptr);
        return value;
    }

    std::string read_string(uintptr_t address);
    std::string get_string(uintptr_t string_ptr);
}