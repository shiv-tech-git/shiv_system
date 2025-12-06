#pragma once

#include <array>
#include <assert.h>
#include <limits>

#include <isa.h>
#include <ram.h>

class Core
{
public:
    
    Core(RAM& ram):
    _ram(ram)
    {
    }

//  ======================= ARITHMETIC =======================
    void Add(Register dst, Register reg1, Register reg2)
    {
        AddImmediate(dst, reg1, Reg(reg2));
    }

    void AddImmediate(Register dst, Register reg1, WORD imm)
    {
        Reg(dst) = DoAdd(Reg(reg1), imm);
    }

    void Sub(Register dst, Register reg1, Register reg2)
    {
        Reg(dst) = DoSub(Reg(reg1), Reg(reg2));
    }

    void SubImmediate(Register dst, Register reg1, WORD imm)
    {
        Reg(dst) = DoSub(Reg(reg1), imm);
    }

    void LoadUpperImmediate(Register reg1, HWORD imm)
    {
        Reg(reg1) = static_cast<WORD>(imm) << (sizeof(HWORD) * 8);
    }

//  ======================== SHIFTS ===========================

    void ShiftLeft(Register dst, Register reg1, Register reg2)
    {
        ShiftLeftImmediate(dst, reg1, static_cast<BYTE>(Reg(reg2)));
    }

    void ShiftLeftImmediate(Register dst, Register reg1, WORD imm)
    {
        WORD tmp = Reg(reg1) << imm;
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void ShiftRight(Register dst, Register reg1, Register reg2)
    {
        ShiftRightImmediate(dst, reg1, static_cast<BYTE>(Reg(reg2)));
    }

    void ShiftRightImmediate(Register dst, Register reg1, WORD imm)
    {
        WORD tmp = Reg(reg1) >> imm;
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

//  ======================= LOGICAL ===========================

    void Or(Register dst, Register reg1, Register reg2)
    {
        OrImmediate(dst, reg1, Reg(reg2));
    }

    void OrImmediate(Register dst, Register reg1, WORD imm)
    {
        WORD tmp = Reg(reg1) | imm;
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void And(Register dst, Register reg1, Register reg2)
    {
        AndImmediate(dst, reg1, Reg(reg2));
    }

    void AndImmediate(Register dst, Register reg1, WORD imm)
    {
        WORD tmp = Reg(reg1) & imm;
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void Xor(Register dst, Register reg1, Register reg2)
    {
        XorImmediate(dst, reg1, Reg(reg2));
    }

    void XorImmediate(Register dst, Register reg1, WORD imm)
    {
        WORD tmp = Reg(reg1) ^ imm;
        UpdateZeroFlag(tmp);
        UpdateNegativeFlag(tmp);
        Reg(dst) = tmp;
    }

    void Not(Register dst, Register reg1)
    {
        Reg(dst) = ~Reg(reg1);
    }

//  ====================== MEMORY =============================

    void LoadByte(Register reg1, Register addr)
    {
        Reg(reg1) = ExtendSign(_ram.ReadByte(Reg(addr)));
    }

    void LoadByteUnsigned(Register reg1, Register addr)
    {
        Reg(reg1) = static_cast<WORD>(_ram.ReadByte(Reg(addr)));
    }

    void LoadHWord(Register reg1, Register addr)
    {
        Reg(reg1) = ExtendSign(_ram.ReadHWord(Reg(addr)));
    }

    void LoadHWordUnsigned(Register reg1, Register addr)
    {
        Reg(reg1) = static_cast<WORD>(_ram.ReadHWord(Reg(addr)));
    }

    void LoadWord(Register reg1, Register addr)
    {
        Reg(reg1) = _ram.ReadWord(Reg(addr));
    }

    void StoreByte(Register reg1, Register addr)
    {
        _ram.WriteByte(Reg(addr), Reg(reg1));
    }

    void StoreHWord(Register reg1, Register addr)
    {
        _ram.WriteHWord(Reg(addr), Reg(reg1));
    }

    void StoreWord(Register reg1, Register addr)
    {
        _ram.WriteWord(Reg(addr), Reg(reg1));
    }

//  ====================== COMPARE ============================

    void Cmp(Register reg1, Register reg2)
    {
        (void)DoSub(Reg(reg1), Reg(reg2));
    }

    void CmpImmediate(Register reg1, WORD op2)
    {
        (void)DoSub(Reg(reg1), op2);
    }

//  ====================== BRANCHES ===========================

    void Branch(WORD offset)
    {
        Reg(Register::IP) = Reg(Register::IP) + offset;
    }

    void BranchEqual(WORD offset)
    {
        if (Equal())
            Branch(offset);
    }

    void BranchNotEqual(WORD offset)
    {
        if (!Equal())
            Branch(offset);
    }

    void BranchGreaterThan(WORD offset)
    {
        if (GreaterThan())
            Branch(offset);
    }

    void BranchGreaterOrEqual(WORD offset)
    {
        if (GreaterOrEqual())
            Branch(offset);
    }

    void BranchLessThan(WORD offset)
    {
        if (LessThan())
            Branch(offset);
    }

    void BranchLessOrEqual(WORD offset)
    {
        if (LessOrEqual())
            Branch(offset);
    }

// ==================== CONTROL FLOW =========================
    void Jump(WORD addr)
    {
        Reg(Register::IP) = addr;
    }

    void JumpRegister(Register reg1)
    {
        Jump(Reg(reg1));
    }
    
    void Call(WORD addr)
    {
        Reg(Register::RA) = Reg(Register::IP);
        Reg(Register::IP) = addr;
    }

    void CallRegister(Register reg1)
    {
        Call(Reg(reg1));
    }
    
    void Ret()
    {
        Reg(Register::IP) = Reg(Register::RA);
    }

// ====================== STACK ==============================

    void Push(Register src)
    {
        Reg(Register::SP) -= sizeof(WORD);
        _ram.WriteWord(Reg(Register::SP), Reg(src));
    }

    void Pop(Register dst)
    {
        Reg(dst) = _ram.ReadWord(Reg(Register::SP));
        Reg(Register::SP) += sizeof(WORD);
    }

// ====================== PSEUDO ==============================

    void LoadImmediate(Register reg1, WORD op2)
    {
        Reg(reg1) = op2;
    }

// ===========================================================

    WORD& Reg(Register reg)
    {
        return _reg_file[static_cast<size_t>(reg)];
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

private:    
    using RegisterFile = std::array<WORD, static_cast<size_t>(Register::__NUM)>;

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

    //helpers
    template<typename T>
    bool IsNegative(T val)
    {
        WORD msb = sizeof(T) * 8 - 1;

        if (val & (1 << msb))
            return true;
        
        return false;
    }

    template<typename T>
    WORD ExtendSign(T val)
    {
        if (!IsNegative(val))
            return static_cast<WORD>(val);
        
        WORD tmp = std::numeric_limits<WORD>::max();
        tmp -= std::numeric_limits<T>::max();
        return tmp & val;
    }
};
