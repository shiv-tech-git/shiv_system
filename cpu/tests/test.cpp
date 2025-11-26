#include <gtest/gtest.h>
#include "cpu.h"

TEST(CPUTest, AddTest)
{
    RAM ram{ 1024 };
    ram.Write(0, 21);
    ram.Write(4, 21);

    CPU cpu{ram};
    cpu.Load(0, CPU::Register::R1);
    cpu.Load(4, CPU::Register::R2);
    
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 21);
    EXPECT_EQ(cpu.Reg(CPU::Register::R2), 21);

    cpu.Add(CPU::Register::R1, CPU::Register::R2);
    
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 42);
    EXPECT_EQ(cpu.Reg(CPU::Register::R2), 21);

    cpu.Store(CPU::Register::R1, 8);
    EXPECT_EQ(ram.Read(8), 42);
}

TEST(CPUTest, ShiftTest)
{
    RAM ram{ 1024 };
    ram.Write(0, 0x00ff0000);
    ram.Write(4, 8);

    CPU cpu{ram};
    cpu.Load(0, CPU::Register::R1);
    cpu.Load(4, CPU::Register::R2);

    cpu.ShiftLeft(CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 0xff000000);

    cpu.Load(0, CPU::Register::R1);
    cpu.ShiftRight(CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 0x0000ff00);
}

TEST(CPUTest, BitwiseTest)
{
    RAM ram{ 1024 };
    ram.Write(0, 0xffff0000);
    ram.Write(4, 0x00ffff00);

    CPU cpu{ram};
    cpu.Load(0, CPU::Register::R1);
    cpu.Load(4, CPU::Register::R2);

    cpu.OR(CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 0xffffff00);

    cpu.Load(0, CPU::Register::R1);
    cpu.AND(CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 0x00ff0000);

    cpu.Load(0, CPU::Register::R1);
    cpu.XOR(CPU::Register::R1, CPU::Register::R2);
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 0xff00ff00);

    cpu.Load(0, CPU::Register::R1);
    cpu.NOT(CPU::Register::R1);
    EXPECT_EQ(cpu.Reg(CPU::Register::R1), 0x0000ffff);
}
