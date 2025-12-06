#pragma once

#include <vector>
#include <sstream>
#include <string.h>

#include "isa.h"

class RAM
{
public:

RAM(size_t size) :
_mem(size, 0)
{
}

void WriteByte(WORD addr, WORD word)
{
    BYTE tmp = static_cast<BYTE>(word);
    Write(addr, &tmp);
}

void WriteHWord(WORD addr, WORD word)
{
    HWORD tmp = static_cast<HWORD>(word);
    Write(addr, &tmp);
}

void WriteWord(WORD addr, WORD word)
{
    Write(addr, &word);
}

BYTE ReadByte(WORD addr)
{
    BYTE tmp;
    Read(addr, &tmp);
    return tmp;
}

HWORD ReadHWord(WORD addr)
{
    HWORD tmp;
    Read(addr, &tmp);
    return tmp;
}

WORD ReadWord(WORD addr)
{
    WORD tmp;
    Read(addr, &tmp);
    return tmp;
}

private:
    std::vector<uint8_t> _mem;

    template<typename T>
    void Read(WORD addr, T* out)
    {
        CheckMemoryBounds(addr);

        if (addr % sizeof(T) != 0)
            ThrowMemoryException("Unaligned read", addr);

        ::memcpy(out, _mem.data() + addr, sizeof(T));
    }

    template<typename T>
    void Write(WORD addr, T* data)
    {
        CheckMemoryBounds(addr);

        if (addr % sizeof(T) != 0)
            ThrowMemoryException("Unaligned write", addr);

        ::memcpy(_mem.data() + addr, data, sizeof(T));
    }

    void CheckMemoryBounds(WORD addr)
    {
        if (addr >= _mem.size())
            ThrowMemoryException("Invalid memory address", addr);
    }

    void ThrowMemoryException(std::string prefix, WORD addr)
    {
        std::stringstream ss;
        ss << prefix << ". Address=0x" << std::hex << addr;
        throw std::runtime_error(ss.str());
    }
};
