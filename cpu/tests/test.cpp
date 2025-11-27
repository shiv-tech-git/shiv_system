#include <gtest/gtest.h>
#include "cpu.h"

TEST(CPUTest, AddTest)
{
    RAM ram{ 0 };
    CPU cpu{ram};

    cpu.LoadImmediate(CPU::Register::R1, 21);
    cpu.LoadImmediate(CPU::Register::R2, 21);
    
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 21);
    EXPECT_EQ(cpu.Reg(CPU::Register::R2), 21);

    cpu.Add(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 42);
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 21);
    EXPECT_EQ(cpu.Reg(CPU::Register::R2), 21);
}

TEST(CPUTest, AddOverflowTests)
{
    RAM ram{0};
    CPU cpu{ram};

    // 1. No overflow: 10 + 20 = 30
    cpu.LoadImmediate(CPU::Register::R1, 10);
    cpu.LoadImmediate(CPU::Register::R2, 20);
    cpu.Add(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 30);
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Carry)); // no carry
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Overflow)); // no signed overflow

    // 2. Unsigned overflow: 0xFFFFFFFF + 1 = 0x00000000
    cpu.LoadImmediate(CPU::Register::R1, 0xFFFFFFFFu);
    cpu.LoadImmediate(CPU::Register::R2, 1);
    cpu.Add(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0x00000000u);
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Carry));  // carry out
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Overflow)); // no signed overflow

    // 3. Signed overflow: 0x7FFFFFFF + 1 = 0x80000000
    cpu.LoadImmediate(CPU::Register::R1, 0x7FFFFFFF);
    cpu.LoadImmediate(CPU::Register::R2, 1);
    cpu.Add(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0x80000000u);
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Carry)); // no unsigned carry
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Overflow));  // signed overflow

    // 4. Signed overflow: 0x80000000 + 0x80000000 = 0x00000000
    cpu.LoadImmediate(CPU::Register::R1, 0x80000000);
    cpu.LoadImmediate(CPU::Register::R2, 0x80000000);
    cpu.Add(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0x00000000u);
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Carry));  // carry out
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Overflow));  // signed overflow
}

TEST(CPUTest, SubOverflowTests)
{
    RAM ram{0};
    CPU cpu{ram};

    // 1. No overflow: 20 - 10 = 10
    cpu.LoadImmediate(CPU::Register::R1, 20);
    cpu.LoadImmediate(CPU::Register::R2, 10);
    cpu.Sub(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 10u);
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Carry));   // no borrow
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Overflow)); // no signed overflow

    // 2. Unsigned borrow: 10 - 20 = 0xFFFFFFF6
    cpu.LoadImmediate(CPU::Register::R1, 10);
    cpu.LoadImmediate(CPU::Register::R2, 20);
    cpu.Sub(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0xFFFFFFF6u);
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Carry));  // borrow occurred
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Overflow)); // no signed overflow

    // 3. Signed overflow: (-2147483648) - 1 = +2147483647
    cpu.LoadImmediate(CPU::Register::R1, 0x80000000); // -2147483648
    cpu.LoadImmediate(CPU::Register::R2, 1);
    cpu.Sub(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0x7FFFFFFFu);
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Carry));   // no borrow
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Overflow)); // signed overflow

    // 4. Signed overflow: (+2147483647) - (-1) = -2147483648
    cpu.LoadImmediate(CPU::Register::R1, 0x7FFFFFFF); // +2147483647
    cpu.LoadImmediate(CPU::Register::R2, 0xFFFFFFFF); // -1
    cpu.Sub(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0x80000000u);
    EXPECT_FALSE(cpu.GetFlag(CPU::Flag::Carry));   // no borrow
    EXPECT_TRUE(cpu.GetFlag(CPU::Flag::Overflow)); // signed overflow
}


TEST(CPUTest, ShiftTest)
{
    RAM ram{ 0 };
    CPU cpu{ram};

    cpu.LoadImmediate(CPU::Register::R1, 0x00ff0000);
    cpu.LoadImmediate(CPU::Register::R2, 8);

    cpu.ShiftLeft(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0xff000000);

    cpu.ShiftRight(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0x0000ff00);
}

TEST(CPUTest, BitwiseTest)
{
    RAM ram{ 1024 };
    ram.Write(0, 0xffff0000);
    ram.Write(4, 0x00ffff00);

    CPU cpu{ram};
    cpu.LoadImmediate(CPU::Register::R1, 0xffff0000);
    cpu.LoadImmediate(CPU::Register::R2, 0x00ffff00);

    cpu.Or(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0xffffff00);

    cpu.And(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0x00ff0000);

    cpu.Xor(CPU::Register::R3, CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R3), 0xff00ff00);
}
