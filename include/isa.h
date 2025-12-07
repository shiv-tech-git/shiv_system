#pragma once

#include <inttypes.h>

//  ======================= ARITHMETIC =======================
//  01  000001  ADD      R1, R2, R3          # R1 = R2 + R3
//  02  000010  ADDI     R1, R2, imm16       # R1 = R2 + imm16
//  03  000011  SUB      R1, R2, R3          # R1 = R2 - R3
//  04  000100  SUBI     R1, R2, imm16       # R1 = R2 - imm16
//  05  000101  LUI      R1, imm16           # R1 = R2 - imm16
//
//  ======================== SHIFTS ===========================
//  06  000110  SHL      R1, R2, R3          # R1 = R2 << R3{4:0}
//  07  000111  SHLI     R1, R2, imm5        # R1 = R2 << imm5
//  08  001000  SHR      R1, R2, R3          # R1 = R2 >> R3{4:0}
//  09  001001  SHRI     R1, R2, imm5        # R1 = R2 >> imm5
//
//  ======================= LOGICAL ===========================
//  10  001010  OR       R1, R2, R3          # R1 = R2 OR R3
//  11  001011  ORI      R1, R2, imm16       # R1 = R2 OR imm16
//  12  001100  AND      R1, R2, R3          # R1 = R2 AND R3
//  13  001101  ANDI     R1, R2, imm16       # R1 = R2 AND imm16
//  14  001110  XOR      R1, R2, R3          # R1 = R2 XOR R3
//  15  001111  XORI     R1, R2, imm16       # R1 = R2 XOR imm16
//  16  010000  NOT      R1, R2              # R1 = NOT R2
//
//  ====================== MEMORY =============================
//  17  010001  LB       R1, [R2]           # R1 = RAM[R3]  # Load byte unsigned 
//  18  010010  LBU      R1, [R2]           # R1 = RAM[R3]  # Load byte signed
//  19  010011  LH       R1, [R2]           # R1 = RAM[R3]  # Load half signed
//  20  010100  LHU      R1, [R2]           # R1 = RAM[R3]  # Load half unsigned
//  21  010101  LW       R1, [R2]           # R1 = RAM[R3]  # Load word signed
//  22  010110  -----------------------------------------------
//  23  011111  SB       R1, [R2]           # RAM[R4] = R2  # Store byte
//  24  011000  SH       R1, [R2]           # RAM[R4] = R2  # Store half
//  25  011001  SW       R1, [R2]           # RAM[R4] = R2  # Store word
//
//  ====================== COMPARE ============================
//  26  011010  CMP      R1, R2              # R1 - R2, update flags
//  27  011011  CMPI     R1, imm16           # R1 - imm16, update flags
//
//  ====================== BRANCHES ===========================
//  28  011100  B        label               # IP = IP + offset    
//  29  011101  BEQ      label               # IP = IP + offset  # Branch if equal
//  30  011110  BNE      label               # IP = IP + offset  # Branch if not equal
//  31  011111  BGT      label               # IP = IP + offset  # Branch if greater that
//  32  100000  BGE      label               # IP = IP + offset  # Branch if greater or equal
//  33  100001  BLT      label               # IP = IP + offset  # Branch if less than
//  34  100010  BLE      label               # IP = IP + offset  # Branch if less or equal
//
// ==================== CONTROL FLOW =========================
//  35  100011  J        label               # IP = IP + offset
//  36  100100  JR       R1                  # IP = R1
//  37  100101  CALL     label               # RA = IP; IP = IP + offset
//  38  100110  CALLR    R1                  # RA = IP; IP = R1
//  39  100111  RET                          # IP = RA
//
// ====================== STACK ==============================
//  40  101000  PUSH     R1                  # SP = SP + 4; RAM[SP] = R1
//  41  101001  POP      R1                  # R1 = RAM[SP]; SP = SP - 4
//====================== MISC ==============================
//  41  101010  HALT                         # Stops execution
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
