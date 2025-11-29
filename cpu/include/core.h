#pragma once

#include <array>
#include <assert.h>

#include <cpu_common.h>
#include <ram.h>

class Core
{
public:
    enum class Register: uint8_t
    {
        RZ= 0,
        R1,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7,
        R8,
        RA,
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
        return (Reg(Register::FLAGS) >> static_cast<uint32_t>(flag)) & 1;
    }

    void Push(Register src)
    {
        Reg(Register::SP) -= sizeof(WORD);
        _ram.Write(Reg(Register::SP), Reg(src));
    }

    void Pop(Register dst)
    {
        Reg(dst) = _ram.Read(Reg(Register::SP));
        Reg(Register::SP) += sizeof(WORD);
    }

    void AddImmediate(Register dst, Register reg1, WORD op2)
    {
        Reg(dst) = DoAdd(Reg(reg1), op2);
    }

    void Add(Register dst, Register reg1, Register reg2)
    {
        AddImmediate(dst, reg1, Reg(reg2));
    }

    void SubImmediate(Register dst, Register reg1, WORD op2)
    {
        Reg(dst) = DoSub(Reg(reg1), op2);
    }

    void Sub(Register dst, Register reg1, Register reg2)
    {
        SubImmediate(dst, reg1, Reg(reg2));
    }

    void Cmp(Register reg1, Register reg2)
    {
        (void)DoSub(Reg(reg1), Reg(reg2));
    }

    void Load(Register reg1, Register addr)
    {
        Reg(reg1) = _ram.Read(Reg(addr));
    }

    void LoadImmediate(Register reg1, WORD op2)
    {
        Reg(reg1) = op2;
    }

    void LoadUpperImmediate(Register reg1, WORD op2)
    {
        Reg(reg1) = op2 << (sizeof(HWORD) * 8);
    }

    void Store(Register reg1, Register addr)
    {
        _ram.Write(Reg(addr), Reg(reg1));
    }

    void Jump(WORD addr)
    {
        Reg(Register::IP) = addr;
    }
    
    void Call(WORD addr)
    {
        Reg(Register::RA) = Reg(Register::IP) + sizeof(WORD);
        Reg(Register::IP) = addr;
    }
    
    void Ret()
    {
        Reg(Register::IP) = Reg(Register::RA);
    }

    void BranchEqual(WORD addr)
    {
        if (Equal())
            Reg(Register::IP) = addr;
    }

    void BranchNotEqual(WORD addr)
    {
        if (!Equal())
            Reg(Register::IP) = addr;
    }

    void BranchLessThan(WORD addr)
    {
        if (LessThan())
            Reg(Register::IP) = addr;
    }

    void BranchLessOrEqual(WORD addr)
    {
        if (LessOrEqual())
            Reg(Register::IP) = addr;
    }

    void BranchGreaterThan(WORD addr)
    {
        if (GreaterThan())
            Reg(Register::IP) = addr;
    }

    void BranchGreaterOrEqual(WORD addr)
    {
        if (GreaterOrEqual())
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

    bool Equal()
    {
        return GetFlag(Flag::Zero) == 1;
    }
    
    bool LessThan()
    {
        return GetFlag(Flag::Negative) != GetFlag(Flag::Overflow);
    }

    bool LessOrEqual()
    {
        return GetFlag(Flag::Zero) == 1 || (GetFlag(Flag::Negative) != GetFlag(Flag::Overflow));
    }

    bool GreaterThan()
    {
        return !LessOrEqual();
    }

    bool GreaterOrEqual()
    {
        return !LessThan();
    }

    WORD DoAdd(WORD op1, WORD op2)
    {
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

        return res;
    }

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
