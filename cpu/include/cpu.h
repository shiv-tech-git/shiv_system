#pragma once

#include <array>
#include <vector>
#include <string.h>

using WORD = uint32_t;

class RAM
{
public:
    using Memory = std::vector<uint8_t>;

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
    Memory _mem;
};

class CPU
{
public:
    enum class Register: uint8_t
    {
        R1 = 0,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7,
        R8,
        IP,
        SP,
        R_NUM
    };

    CPU(RAM& ram):
        _ram(ram)
    {
    }

    void Add(Register reg1, Register reg2)
    {
        Reg(reg1) = Reg(reg1) + Reg(reg2);
    }

    void Move(Register reg1, Register reg2)
    {
        Reg(reg2) = Reg(reg1);
    }

    void Load(WORD addr, Register reg)
    {
        Reg(reg) = _ram.Read(addr);
    }

    void Store(Register reg, WORD addr)
    {
        _ram.Write(addr, Reg(reg));
    }

    void JumpZero(Register reg, WORD addr)
    {
        if (!Reg(reg))
            Reg(Register::IP) = addr;
    }

    void ShiftRight(Register reg1, Register reg2)
    {
        Reg(reg1) = Reg(reg1) >> Reg(reg2);
    }

    void ShiftLeft(Register reg1, Register reg2)
    {
        Reg(reg1) = Reg(reg1) << Reg(reg2);
    }

    void OR(Register reg1, Register reg2)
    {
        Reg(reg1) = Reg(reg1) | Reg(reg2);
    }

    void AND(Register reg1, Register reg2)
    {
        Reg(reg1) = Reg(reg1) & Reg(reg2);
    }

    void NOT(Register reg1)
    {
        Reg(reg1) = ~Reg(reg1);
    }

    void XOR(Register reg1, Register reg2)
    {
        Reg(reg1) = Reg(reg1) ^ Reg(reg2);
    }

    WORD& Reg(Register reg)
    {
        return _reg_file[static_cast<size_t>(reg)];
    }

private:
    using RegisterFile = std::array<WORD, static_cast<size_t>(Register::R_NUM)>;

    RegisterFile    _reg_file{ 0 };
    RAM&            _ram;
};
