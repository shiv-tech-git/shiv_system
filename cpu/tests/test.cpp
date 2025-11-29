#include <gtest/gtest.h>
#include "ram.h"
#include "core.h"

TEST(CPUTest, AddTest)
{
    RAM ram{ 0 };
    Core core{ram};

    core.LoadImmediate(Core::Register::R1, 21);
    core.LoadImmediate(Core::Register::R2, 21);
    
    EXPECT_EQ(core.Reg(Core::Register::R1), 21);
    EXPECT_EQ(core.Reg(Core::Register::R2), 21);

    core.Add(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    
    EXPECT_EQ(core.Reg(Core::Register::R3), 42);
    EXPECT_EQ(core.Reg(Core::Register::R1), 21);
    EXPECT_EQ(core.Reg(Core::Register::R2), 21);
}

TEST(CPUTest, AddOverflowTests)
{
    RAM ram{0};
    Core core{ram};

    // 1. No overflow: 10 + 20 = 30
    core.LoadImmediate(Core::Register::R1, 10);
    core.LoadImmediate(Core::Register::R2, 20);
    core.Add(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 30);
    EXPECT_FALSE(core.GetFlag(Core::Flag::Carry)); // no carry
    EXPECT_FALSE(core.GetFlag(Core::Flag::Overflow)); // no signed overflow

    // 2. Unsigned overflow: 0xFFFFFFFF + 1 = 0x00000000
    core.LoadImmediate(Core::Register::R1, 0xFFFFFFFFu);
    core.LoadImmediate(Core::Register::R2, 1);
    core.Add(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0x00000000u);
    EXPECT_TRUE(core.GetFlag(Core::Flag::Carry));  // carry out
    EXPECT_FALSE(core.GetFlag(Core::Flag::Overflow)); // no signed overflow

    // 3. Signed overflow: 0x7FFFFFFF + 1 = 0x80000000
    core.LoadImmediate(Core::Register::R1, 0x7FFFFFFF);
    core.LoadImmediate(Core::Register::R2, 1);
    core.Add(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0x80000000u);
    EXPECT_FALSE(core.GetFlag(Core::Flag::Carry)); // no unsigned carry
    EXPECT_TRUE(core.GetFlag(Core::Flag::Overflow));  // signed overflow

    // 4. Signed overflow: 0x80000000 + 0x80000000 = 0x00000000
    core.LoadImmediate(Core::Register::R1, 0x80000000);
    core.LoadImmediate(Core::Register::R2, 0x80000000);
    core.Add(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0x00000000u);
    EXPECT_TRUE(core.GetFlag(Core::Flag::Carry));  // carry out
    EXPECT_TRUE(core.GetFlag(Core::Flag::Overflow));  // signed overflow
}

TEST(CPUTest, SubOverflowTests)
{
    RAM ram{0};
    Core core{ram};

    // 1. No overflow: 20 - 10 = 10
    core.LoadImmediate(Core::Register::R1, 20);
    core.LoadImmediate(Core::Register::R2, 10);
    core.Sub(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 10u);
    EXPECT_TRUE(core.GetFlag(Core::Flag::Carry));   // no borrow
    EXPECT_FALSE(core.GetFlag(Core::Flag::Overflow)); // no signed overflow

    // 2. Unsigned borrow: 10 - 20 = 0xFFFFFFF6
    core.LoadImmediate(Core::Register::R1, 10);
    core.LoadImmediate(Core::Register::R2, 20);
    core.Sub(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0xFFFFFFF6u);
    EXPECT_FALSE(core.GetFlag(Core::Flag::Carry));  // borrow occurred
    EXPECT_FALSE(core.GetFlag(Core::Flag::Overflow)); // no signed overflow

    // 3. Signed overflow: (-2147483648) - 1 = +2147483647
    core.LoadImmediate(Core::Register::R1, 0x80000000); // -2147483648
    core.LoadImmediate(Core::Register::R2, 1);
    core.Sub(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0x7FFFFFFFu);
    EXPECT_TRUE(core.GetFlag(Core::Flag::Carry));   // no borrow
    EXPECT_TRUE(core.GetFlag(Core::Flag::Overflow)); // signed overflow

    // 4. Signed overflow: (+2147483647) - (-1) = -2147483648
    core.LoadImmediate(Core::Register::R1, 0x7FFFFFFF); // +2147483647
    core.LoadImmediate(Core::Register::R2, 0xFFFFFFFF); // -1
    core.Sub(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0x80000000u);
    EXPECT_FALSE(core.GetFlag(Core::Flag::Carry));   // no borrow
    EXPECT_TRUE(core.GetFlag(Core::Flag::Overflow)); // signed overflow
}

TEST(CPUTest, ShiftTest)
{
    RAM ram{ 0 };
    Core core{ram};

    core.LoadImmediate(Core::Register::R1, 0x00ff0000);
    core.LoadImmediate(Core::Register::R2, 8);

    core.ShiftLeft(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0xff000000);

    core.ShiftRight(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0x0000ff00);
}

TEST(CPUTest, BitwiseTest)
{
    RAM ram{ 1024 };
    ram.Write(0, 0xffff0000);
    ram.Write(4, 0x00ffff00);

    Core core{ram};
    core.LoadImmediate(Core::Register::R1, 0xffff0000);
    core.LoadImmediate(Core::Register::R2, 0x00ffff00);

    core.Or(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0xffffff00);

    core.And(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0x00ff0000);

    core.Xor(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(core.Reg(Core::Register::R3), 0xff00ff00);
}


// everything below is written by AI
struct CpuTest : ::testing::Test {
    static constexpr size_t kMemSize = 1024;
    RAM ram{ kMemSize };
    Core cpu{ ram };

    // Convenience
    WORD& R(Core::Register r) { return cpu.Reg(r); }

    void SetIP(WORD ip) { R(Core::Register::IP) = ip; }
    void SetSP(WORD sp) { R(Core::Register::SP) = sp; }
    void SetRA(WORD ra) { R(Core::Register::RA) = ra; }

    uint8_t Z() { return cpu.GetFlag(Core::Flag::Zero); }
    uint8_t N() { return cpu.GetFlag(Core::Flag::Negative); }
    uint8_t C() { return cpu.GetFlag(Core::Flag::Carry); }
    uint8_t V() { return cpu.GetFlag(Core::Flag::Overflow); }

    void ExpectFlags(uint8_t z, uint8_t n, uint8_t c, uint8_t v) {
        EXPECT_EQ(Z(), z);
        EXPECT_EQ(N(), n);
        EXPECT_EQ(C(), c);
        EXPECT_EQ(V(), v);
    }
};

TEST_F(CpuTest, AddImmediate_basic_no_flags) {
    R(Core::Register::R1) = 10;
    cpu.AddImmediate(Core::Register::R2, Core::Register::R1, 20);
    EXPECT_EQ(R(Core::Register::R2), 30);
    // 30 -> Z=0, N=0, no carry, no overflow
    ExpectFlags(/*Z*/0, /*N*/0, /*C*/0, /*V*/0);
}

TEST_F(CpuTest, AddImmediate_sets_zero_flag) {
    R(Core::Register::R1) = 0;
    cpu.AddImmediate(Core::Register::R2, Core::Register::R1, 0);
    EXPECT_EQ(R(Core::Register::R2), 0u);
    ExpectFlags(/*Z*/1, /*N*/0, /*C*/0, /*V*/0);
}

TEST_F(CpuTest, AddImmediate_unsigned_carry) {
    // Cause carry out of 32 bits
    R(Core::Register::R1) = 0xFFFF'FFFFu;
    cpu.AddImmediate(Core::Register::R2, Core::Register::R1, 1u);
    EXPECT_EQ(R(Core::Register::R2), 0u);
    // Carry out -> C=1; zero set; N=0; overflow (signed) should be 0 for +1 across boundary
    ExpectFlags(/*Z*/1, /*N*/0, /*C*/1, /*V*/0);
}

TEST_F(CpuTest, AddImmediate_signed_overflow_positive_to_negative) {
    // 0x7FFF'FFFF + 1 = 0x8000'0000 (signed overflow)
    R(Core::Register::R1) = 0x7FFF'FFFFu;
    cpu.AddImmediate(Core::Register::R2, Core::Register::R1, 1u);
    EXPECT_EQ(R(Core::Register::R2), 0x8000'0000u);
    ExpectFlags(/*Z*/0, /*N*/1, /*C*/0, /*V*/1);
}

TEST_F(CpuTest, SubImmediate_basic) {
    R(Core::Register::R1) = 50;
    cpu.SubImmediate(Core::Register::R2, Core::Register::R1, 8);
    EXPECT_EQ(R(Core::Register::R2), 42u);
    // ARM semantics:
    // Result = 42 -> Z=0, N=0
    // No borrow occurred -> Carry=1
    // No signed overflow -> V=0
    ExpectFlags(/*Z*/0, /*N*/0, /*C*/1, /*V*/0);
}

TEST_F(CpuTest, SubImmediate_zero_result_and_borrow) {
    R(Core::Register::R1) = 1;
    cpu.SubImmediate(Core::Register::R2, Core::Register::R1, 1);
    EXPECT_EQ(R(Core::Register::R2), 0u);
    // ARM semantics:
    // Result = 0 -> Z=1, N=0
    // No borrow occurred -> Carry=1
    // No signed overflow -> V=0
    ExpectFlags(/*Z*/1, /*N*/0, /*C*/1, /*V*/0);
}

TEST_F(CpuTest, SubImmediate_signed_overflow) {
    // INT32_MIN - 1 => wraps to 0x7FFFFFFF, signed overflow
    R(Core::Register::R1) = 0x8000'0000u;
    cpu.SubImmediate(Core::Register::R2, Core::Register::R1, 1);
    EXPECT_EQ(R(Core::Register::R2), 0x7FFF'FFFFu);
    // ARM semantics:
    // Result positive -> N=0, Z=0
    // No borrow occurred (0x80000000 >= 1) -> Carry=1
    // Signed overflow -> V=1
    ExpectFlags(/*Z*/0, /*N*/0, /*C*/1, /*V*/1);
}

TEST_F(CpuTest, Cmp_sets_flags_without_writing_registers) {
    R(Core::Register::R1) = 10;
    R(Core::Register::R2) = 20;
    WORD oldR1 = R(Core::Register::R1);
    WORD oldR2 = R(Core::Register::R2);

    cpu.Cmp(Core::Register::R1, Core::Register::R2); // 10 - 20
    EXPECT_EQ(R(Core::Register::R1), oldR1);
    EXPECT_EQ(R(Core::Register::R2), oldR2);
    // ARM semantics:
    // Result negative -> N=1, Z=0
    // Borrow occurred (10 < 20) -> Carry=0
    // No signed overflow -> V=0
    ExpectFlags(/*Z*/0, /*N*/1, /*C*/0, /*V*/0);
}

TEST_F(CpuTest, BranchEqual_and_BranchNotEqual) {
    SetIP(100);
    R(Core::Register::R1) = 5;
    R(Core::Register::R2) = 5;
    cpu.Cmp(Core::Register::R1, Core::Register::R2); // Z=1
    cpu.BranchEqual(500);
    EXPECT_EQ(R(Core::Register::IP), 500u);

    SetIP(100);
    cpu.BranchNotEqual(700);
    // Z=1 -> NE should not branch
    EXPECT_EQ(R(Core::Register::IP), 100u);
}

TEST_F(CpuTest, BranchLessThan_and_GreaterOrEqual) {
    SetIP(100);
    R(Core::Register::R1) = 10;
    R(Core::Register::R2) = 20;
    cpu.Cmp(Core::Register::R1, Core::Register::R2); // 10 - 20 => LT: N != V (N=1, V=0)
    cpu.BranchLessThan(400);
    EXPECT_EQ(R(Core::Register::IP), 400u);

    SetIP(100);
    cpu.BranchGreaterOrEqual(600);
    // Should not branch since LT was true
    EXPECT_EQ(R(Core::Register::IP), 100u);
}

TEST_F(CpuTest, BranchLessOrEqual_and_GreaterThan) {
    // Equal case -> LE true
    SetIP(100);
    R(Core::Register::R1) = 33;
    R(Core::Register::R2) = 33;
    cpu.Cmp(Core::Register::R1, Core::Register::R2); // Z=1
    cpu.BranchLessOrEqual(900);
    EXPECT_EQ(R(Core::Register::IP), 900u);

    SetIP(100);
    cpu.BranchGreaterThan(800);
    // LE was true -> GT is !LE -> should not branch
    EXPECT_EQ(R(Core::Register::IP), 100u);
}

TEST_F(CpuTest, Signed_compare_with_overflow_edge) {
    // Test LT/GE around overflow: 0x80000000 (-2147483648) < 0x7FFFFFFF (2147483647)
    SetIP(100);
    R(Core::Register::R1) = 0x8000'0000u;
    R(Core::Register::R2) = 0x7FFF'FFFFu;
    cpu.Cmp(Core::Register::R1, Core::Register::R2); // expect LT true (N != V)
    cpu.BranchLessThan(1000);
    EXPECT_EQ(R(Core::Register::IP), 1000u);

    SetIP(200);
    cpu.BranchGreaterOrEqual(1200);
    EXPECT_EQ(R(Core::Register::IP), 200u);
}

TEST_F(CpuTest, Load_store_roundtrip) {
    R(Core::Register::R1) = 0xDEAD'BEEFu;
    R(Core::Register::R2) = 64; // address
    cpu.Store(Core::Register::R1, Core::Register::R2);
    R(Core::Register::R3) = 0;
    cpu.Load(Core::Register::R3, Core::Register::R2);
    EXPECT_EQ(R(Core::Register::R3), 0xDEAD'BEEFu);
}

TEST_F(CpuTest, Push_pop_stack_grows_down) {
    SetSP(256);
    R(Core::Register::R1) = 0xC0FF'EE00u;
    cpu.Push(Core::Register::R1);
    // After push: memory[SP_before] contains R1; SP decreased by sizeof(WORD)
    EXPECT_EQ(R(Core::Register::SP), 256u - sizeof(WORD));

    R(Core::Register::R2) = 0;
    cpu.Pop(Core::Register::R2);
    EXPECT_EQ(R(Core::Register::R2), 0xC0FF'EE00u);
    EXPECT_EQ(R(Core::Register::SP), 256u);
}

TEST_F(CpuTest, ShiftLeft_updates_Z_and_N) {
    R(Core::Register::R1) = 1;
    R(Core::Register::R2) = 3;
    cpu.ShiftLeft(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(R(Core::Register::R3), 8u);
    ExpectFlags(/*Z*/0, /*N*/0, /*C*/0, /*V*/0);
}

TEST_F(CpuTest, ShiftRight_updates_Z_and_N) {
    R(Core::Register::R1) = 0x8000'0000u;
    R(Core::Register::R2) = 31;
    cpu.ShiftRight(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(R(Core::Register::R3), 1u);
    ExpectFlags(/*Z*/0, /*N*/0, /*C*/0, /*V*/0);
}

TEST_F(CpuTest, Or_and_And_and_Xor_set_flags) {
    R(Core::Register::R1) = 0x0000'0000u;
    R(Core::Register::R2) = 0xFFFF'FFFFu;

    cpu.Or(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(R(Core::Register::R3), 0xFFFF'FFFFu);
    ExpectFlags(/*Z*/0, /*N*/1, /*C*/0, /*V*/0);

    cpu.And(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(R(Core::Register::R3), 0x0000'0000u);
    ExpectFlags(/*Z*/1, /*N*/0, /*C*/0, /*V*/0);

    R(Core::Register::R1) = 0xF0F0'0000u;
    R(Core::Register::R2) = 0x0F0F'0000u;
    cpu.Xor(Core::Register::R3, Core::Register::R1, Core::Register::R2);
    EXPECT_EQ(R(Core::Register::R3), 0xFFFF'0000u);
    ExpectFlags(/*Z*/0, /*N*/1, /*C*/0, /*V*/0);
}

TEST_F(CpuTest, Jump_sets_ip) {
    SetIP(0);
    cpu.Jump(400);
    EXPECT_EQ(R(Core::Register::IP), 400u);
}

TEST_F(CpuTest, Call_sets_ra_and_ip) {
    SetIP(100);
    SetRA(0);
    cpu.Call(800);
    EXPECT_EQ(R(Core::Register::RA), 100u + sizeof(WORD));
    EXPECT_EQ(R(Core::Register::IP), 800u);
}

TEST_F(CpuTest, Ret_restores_ip_from_ra) {
    SetRA(555);
    cpu.Ret();
    EXPECT_EQ(R(Core::Register::IP), 555u);
}

TEST_F(CpuTest, LoadImmediate_and_LoadUpperImmediate) {
    cpu.LoadImmediate(Core::Register::R1, 0x1234'5678u);
    EXPECT_EQ(R(Core::Register::R1), 0x1234'5678u);

    cpu.LoadUpperImmediate(Core::Register::R2, 0xABCDu);
    // Upper immediate uses HWORD shifted by 16 bits for WORD
    EXPECT_EQ(R(Core::Register::R2), 0xABCD'0000u);
}
