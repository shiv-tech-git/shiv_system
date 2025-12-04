#pragma once

#include <unordered_map>

#include "isa.h"

class Assembler
{
public:
    const std::unordered_map<Instruction, InstructionType> operation_to_type
    {
        { Instruction::ADD,       InstructionType::OP_R3            },
        { Instruction::ADDI,      InstructionType::OP_R2_IMM16      },
        { Instruction::SUB,       InstructionType::OP_R3            },
        { Instruction::SUBI,      InstructionType::OP_R2_IMM16      },
        { Instruction::LUI,       InstructionType::OP_R1_IMM16      },
        { Instruction::SHL,       InstructionType::OP_R3            },
        { Instruction::SHLI,      InstructionType::OP_R2_IMM16      },
        { Instruction::SHR,       InstructionType::OP_R3            },
        { Instruction::SHRI,      InstructionType::OP_R2_IMM16      },
        { Instruction::OR,        InstructionType::OP_R3            },
        { Instruction::ORI,       InstructionType::OP_R2_IMM16      },
        { Instruction::AND,       InstructionType::OP_R3            },
        { Instruction::ANDI,      InstructionType::OP_R2_IMM16      },
        { Instruction::XOR,       InstructionType::OP_R3            },
        { Instruction::XORI,      InstructionType::OP_R2_IMM16      },
        { Instruction::NOT,       InstructionType::OP_R3            },
        { Instruction::LB,        InstructionType::OP_R2            },
        { Instruction::LBU,       InstructionType::OP_R2            },
        { Instruction::LH,        InstructionType::OP_R2            },
        { Instruction::LHU,       InstructionType::OP_R2            },
        { Instruction::LW,        InstructionType::OP_R2            },
        { Instruction::LWU,       InstructionType::OP_R2            },
        { Instruction::SB,        InstructionType::OP_R2            },
        { Instruction::SH,        InstructionType::OP_R2            },
        { Instruction::SW,        InstructionType::OP_R2            },
        { Instruction::CMP,       InstructionType::OP_R2            },
        { Instruction::CMPI,      InstructionType::OP_R1_IMM16      },
        { Instruction::B,         InstructionType::OP_IMM26         },
        { Instruction::BEQ,       InstructionType::OP_IMM26         },
        { Instruction::BNE,       InstructionType::OP_IMM26         },
        { Instruction::BGT,       InstructionType::OP_IMM26         },
        { Instruction::BGE,       InstructionType::OP_IMM26         },
        { Instruction::BLT,       InstructionType::OP_IMM26         },
        { Instruction::BLE,       InstructionType::OP_IMM26         },
        { Instruction::J,         InstructionType::OP_IMM26         },
        { Instruction::JR,        InstructionType::OP_IMM26         },
        { Instruction::CALL,      InstructionType::OP_IMM26         },
        { Instruction::CALLR,     InstructionType::OP_R1            },
        { Instruction::RET,       InstructionType::OP               },
        { Instruction::PUSH,      InstructionType::OP_R1            },
        { Instruction::POP,       InstructionType::OP_R1            }
    };

    struct DecodedInstruction
    {
        uint8_t         opcode{ 0 };
        InstructionType type{ 0 };
        Register        r1{ 0 };
        Register        r2{ 0 };
        Register        r3{ 0 };
        uint16_t        imm16{ 0 };
        uint32_t        imm26{ 0 };
    };

    DecodedInstruction Decode(WORD instruction)
    {

    }

private:
    Instruction GetOpcode(WORD instruction)
    {
        return static_cast<Instruction>(instruction >> 26);
    }

    Register GetR1(WORD instruction)
    {
        return static_cast<Register>((instruction >> 21) && 0x1F);
    }

    Register GetR2(WORD instruction)
    {
        return static_cast<Register>((instruction >> 16) && 0x1F);
    }

    Register GetR3(WORD instruction)
    {
        return static_cast<Register>((instruction >> 11) && 0x1F);
    }

    uint16_t GetImm16(WORD instruction)
    {
        return static_cast<uint16_t>(instruction && 0xFFFF);
    }

    uint32_t GetImm26(WORD instruction)
    {
        return static_cast<uint16_t>(instruction && 0x3FFFFFF);
    }

};
