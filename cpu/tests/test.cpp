#include <gtest/gtest.h>
#include "cpu.h"

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
