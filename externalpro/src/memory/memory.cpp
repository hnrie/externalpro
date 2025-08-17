#include "memory.hpp"
#include "../globals.hpp"

std::string memory::read_string(uintptr_t address)
{
    std::string result;
    result.reserve(204);
    for (int offset = 0; offset < 200; ++offset)
    {
        char ch = read<char>(address + offset);
        if (ch == 0) break;
        result.push_back(ch);
    }
    return result;
}

std::string memory::decode_string(uintptr_t string_ptr)
{
    uintptr_t data_ptr = read<uintptr_t>(string_ptr);
    int length = read<int>(string_ptr + 0x8);
    (void)length;
    return read_string(data_ptr);
}