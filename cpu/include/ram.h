#pragma once

#include <vector>
#include <string.h>

#include "cpu_common.h"

class RAM
{
public:

RAM(size_t size) :
_mem(size, 0)
{
}

void Write(WORD addr, WORD word)
{
    ::memcpy(_mem.data() + addr, &word, sizeof(WORD));
}

WORD Read(WORD addr)
{
    WORD tmp;
    ::memcpy(&tmp, _mem.data() + addr, sizeof(WORD));
    return tmp;
}

private:
    std::vector<uint8_t> _mem;
};