#pragma once

#include <inttypes.h>

//  ======================= ARITHMETIC =======================
//  ADD     R1, R2, R3          # R1 = R2 + R3
//  ADDI    R1, R2, imm16       # R1 = R2 + imm16
//  SUB     R1, R2, R3          # R1 = R2 - R3
//  SUBI    R1, R2, imm16       # R1 = R2 - imm16
//  LUI     R1, imm16           # R1 = R2 - imm16
//          R1, %hi(label)      # R1 = (address_of(label) << 16)
//
//  ======================== SHIFTS ===========================
//  SHL     R1, R2, R3          # R1 = R2 << R3{4:0}
//  SHLI    R1, R2, imm5        # R1 = R2 << imm5
//  SHR     R1, R2, R3          # R1 = R2 >> R3{4:0}
//  SHRI    R1, R2, imm5        # R1 = R2 >> imm5
//
//  ======================= LOGICAL ===========================
//  OR      R1, R2, R3          # R1 = R2 OR R3
//  ORI     R1, R2, imm16       # R1 = R2 OR imm16
//          R1, %lo(label)      # R1 = (address_of(label) & 0xFFFF)
//  AND     R1, R2, R3          # R1 = R2 AND R3
//  ANDI    R1, R2, imm16       # R1 = R2 AND imm16
//  XOR     R1, R2, R3          # R1 = R2 XOR R3
//  XORI    R1, R2, imm16       # R1 = R2 XOR imm16
//  NOT     R1, R2              # R1 = NOT R2
//
//  ====================== MEMORY =============================
//  LB      R1, [R2]            # R1 = RAM[R3]  # Load byte unsigned 
//  LBU     R1, [R2]            # R1 = RAM[R3]  # Load byte signed
//  LH      R1, [R2]            # R1 = RAM[R3]  # Load half signed
//  LHU     R1, [R2]            # R1 = RAM[R3]  # Load half unsigned
//  LW      R1, [R2]            # R1 = RAM[R3]  # Load word signed
//  SB      R1, [R2]            # RAM[R4] = R2  # Store byte
//  SH      R1, [R2]            # RAM[R4] = R2  # Store half
//  SW      R1, [R2]            # RAM[R4] = R2  # Store word
//
//  ====================== COMPARE ============================
//  CMP     R1, R2              # R1 - R2, update flags
//  CMPI    R1, imm16           # R1 - imm16, update flags
//
//  ====================== BRANCHES ===========================
//  B       label               # IP = IP + offset    
//  BEQ     label               # IP = IP + offset  # Branch if equal
//  BNE     label               # IP = IP + offset  # Branch if not equal
//  BGT     label               # IP = IP + offset  # Branch if greater that
//  BGE     label               # IP = IP + offset  # Branch if greater or equal
//  BLT     label               # IP = IP + offset  # Branch if less than
//  BLE     label               # IP = IP + offset  # Branch if less or equal
//
// ==================== CONTROL FLOW =========================
//  J       label               # IP = IP + offset
//  JR      R1                  # IP = R1
//  CALL    label               # RA = IP; IP = IP + offset
//  CALLR   R1                  # RA = IP; IP = R1
//  RET                         # IP = RA
//
// ====================== STACK ==============================
//  PUSH    R1                  # SP = SP + 4; RAM[SP] = R1
//  POP     R1                  # R1 = RAM[SP]; SP = SP - 4
//======================= MISC ===============================
//  HALT                        # Stops execution
//
//
//               32 | 31| 30| 29| 28| 27| 26| 25| 24| 23| 22| 21| 20| 19| 18| 17| 16| 15| 14| 13| 12| 11| 10| 09| 08| 07| 06| 05| 04| 03| 02| 01| 00|
//                  +-----------------------+-------------------+-------------------+---------------------------------------------------------------+
//    OP_R3         |                Opcode |                R1 |                R2 |                R3 |-----------------------|             shamt |
//                  +-----------------------+-------------------+-------------------+---------------------------------------------------------------+
//    OP_R2_IMM16   |                Opcode |                R1 |                R2 |                                                         imm16 |
//                  +-----------------------+-------------------+-----------------------------------------------------------------------------------+
//    OP_R2         |                Opcode |                R1 |                R2 |---------------------------------------------------------------|
//                  +-----------------------+-------------------+-----------------------------------------------------------------------------------+
//    OP_R1_IMM16   |                Opcode |                R1 |-------------------|                                                         imm16 |
//                  +-----------------------+-------------------+-----------------------------------------------------------------------------------+
//    OP_R1         |                Opcode |                R1 |                                                                                   |
//                  +-----------------------+-------------------------------------------------------------------------------------------------------+
//    OP_J          |                Opcode |                                                                                              offset26 |
//                  +-----------------------+-------------------------------------------------------------------------------------------------------+
//    OP            |                Opcode |-------------------------------------------------------------------------------------------------------|
//                  +-----------------------+-------------------------------------------------------------------------------------------------------+
//

enum class Instruction : uint8_t
{
    ADD = 1,
    ADDI,
    SUB,
    SUBI,
    LUI,
    SHL,
    SHLI,
    SHR,
    SHRI,
    OR,
    ORI,
    AND,
    ANDI,
    XOR,
    XORI,
    NOT,
    LB,
    LBU,
    LH,
    LHU,
    LW,
    LWU,
    SB,
    SH,
    SW,
    CMP,
    CMPI,
    B,
    BEQ,
    BNE,
    BGT,
    BGE,
    BLT,
    BLE,
    J,
    JR,
    CALL,
    CALLR,
    RET,
    PUSH,
    POP,
    HALT,
    __NUM
};

enum class InstructionType : uint8_t
{
    OP_R3 = 0,
    OP_R2_IMM16,
    OP_R2,
    OP_R1_IMM16,
    OP_R1,
    OP_J,
    OP
};

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
    __NUM
};

enum class Flag: uint8_t
{
    Zero = 0,
    Carry,
    Overflow,
    Negative
};

enum class Section: uint8_t
{
    TEXT = 0,
    RODATA,
    DATA,
    BSS
};

using BYTE  = uint8_t;
using HWORD = uint16_t;
using WORD  = uint32_t;
using DWORD = uint64_t;

const static WORD MSB_I = ((sizeof(WORD) * 8) - 1);
const static WORD CB_I = ((sizeof(WORD) * 8));
