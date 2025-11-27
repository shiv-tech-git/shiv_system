#pragma once

#include <array>
#include <vector>
#include <string.h>
#include <assert.h>

using WORD = uint32_t;
using DWORD = uint64_t;
const static WORD MSB_I = ((sizeof(WORD) * 8) - 1);
const static WORD CB_I = ((sizeof(WORD) * 8));

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

class Core
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
        FLAGS,
        R_NUM
    };

    enum class Flag: uint8_t
    {
        Zero = 0,
        Carry,
        Overflow,
        Negative
    };
    
    Core(RAM& ram):
    _ram(ram)
    {
    }

    void SetFlag(Flag flag)
    {
        Reg(Register::FLAGS) |= (1 << static_cast<uint32_t>(flag));
    }

    void ClearFlag(Flag flag)
    {
        Reg(Register::FLAGS) &= ~(1 << static_cast<uint32_t>(flag));
    }

    uint8_t GetFlag(Flag flag)
    {
        return Reg(Register::FLAGS) & (1 << static_cast<uint32_t>(flag));
    }

    void Add(Register dst, Register reg1, Register reg2)
    {
        WORD op1 = Reg(reg1);
        WORD op2 = Reg(reg2);
        DWORD wide_op1 = static_cast<DWORD>(op1);
        DWORD wide_op2 = static_cast<DWORD>(op2);
        
        DWORD wide_res = wide_op1 + wide_op2;
        WORD res = static_cast<WORD>(wide_res);

        UpdateZeroFlag(res);
        UpdateNegativeFlag(res);
        UpdateCarryFlag(wide_res);

        uint8_t op1_sign = op1 >> MSB_I;
        uint8_t op2_sign = op2 >> MSB_I;
        uint8_t res_sign = res >> MSB_I;
        uint8_t overflow = ~(op1_sign ^ op2_sign) & (op1_sign ^ res_sign);
        UpdateFlag(Flag::Overflow, overflow);

        Reg(dst) = res;
    }

    void Sub(Register dst, Register reg1, Register reg2)
    {
        Reg(dst) = DoSub(Reg(reg1), Reg(reg2));
    }

    void Cmp(Register reg1, Register reg2)
    {
        (void)DoSub(Reg(reg1), Reg(reg2));
    }

    void Move(Register dst, Register src)
    {
        Reg(dst) = Reg(src);
    }

    void Load(Register reg1, Register addr)
    {
        Reg(reg1) = _ram.Read(Reg(addr));
    }

    void LoadImmediate(Register reg1, WORD data)
    {
        Reg(reg1) = data;
    }

    void Store(Register reg1, Register addr)
    {
        _ram.Write(Reg(addr), Reg(reg1));
    }

    void Jump(Register reg, WORD addr)
    {
        if (!Reg(reg))
            Reg(Register::IP) = addr;
    }

    void JumpEqualReg(Register reg)
    {
        if (GetFlag(Flag::Zero))
            Reg(Register::IP) = Reg(reg);
    }

    void JumpNotEqualReg(Register reg)
    {
        if (!GetFlag(Flag::Zero))
            Reg(Register::IP) = Reg(reg);
    }

    void JumpEqual(WORD addr)
    {
        if (GetFlag(Flag::Zero))
            Reg(Register::IP) = addr;
    }

    void JumpNotEqual(WORD addr)
    {
        if (!GetFlag(Flag::Zero))
            Reg(Register::IP) = addr;
    }

    void ShiftRight(Register dst, Register reg1, Register reg2)
    {
        WORD tmp = Reg(reg1) >> Reg(reg2);
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void ShiftLeft(Register dst, Register reg1, Register reg2)
    {
        WORD tmp = Reg(reg1) << Reg(reg2);
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void Or(Register dst, Register reg1, Register reg2)
    {
        WORD tmp = Reg(reg1) | Reg(reg2);
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void And(Register dst, Register reg1, Register reg2)
    {
        WORD tmp = Reg(reg1) & Reg(reg2);
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void Xor(Register dst, Register reg1, Register reg2)
    {
        WORD tmp = Reg(reg1) ^ Reg(reg2);
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    WORD& Reg(Register reg)
    {
        return _reg_file[static_cast<size_t>(reg)];
    }

private:    
    using RegisterFile = std::array<WORD, static_cast<size_t>(Register::R_NUM)>;

    RegisterFile _reg_file{ 0 };
    RAM& _ram;

    WORD DoSub(WORD op1, WORD op2)
    {
        DWORD wide_op1 = static_cast<DWORD>(op1);
        DWORD wide_op2 = static_cast<DWORD>(~op2 + 1);
        DWORD wide_res = wide_op1 + wide_op2;
        WORD res = static_cast<WORD>(wide_res);

        UpdateNegativeFlag(res);
        UpdateZeroFlag(res);
        UpdateCarryFlag(wide_res);
        
        uint8_t op1_sign = op1 >> MSB_I;
        uint8_t op2_sign = op2 >> MSB_I;
        uint8_t res_sign = res >> MSB_I;
        uint8_t overflow = (op1_sign ^ op2_sign) & (op1_sign ^ res_sign);
        UpdateFlag(Flag::Overflow, overflow);
        
        return res;
    }

    void UpdateFlag(Flag flag, uint8_t state)
    {
        if (state)
            SetFlag(flag);
        else
            ClearFlag(flag);
    }

    void UpdateZeroFlag(WORD value)
    {
        UpdateFlag(Flag::Zero, value == 0);
    }

    void UpdateNegativeFlag(WORD value)
    {
        uint8_t negative = (value >> MSB_I) & 1;
        UpdateFlag(Flag::Negative, negative);
    }

    void UpdateCarryFlag(DWORD wide_value)
    {
        uint8_t carry = (wide_value >> CB_I) & 1;
        UpdateFlag(Flag::Carry, carry);
    }
};
