#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

#include "isa.h"

class InstructionList
{
public:
    struct ParsedInstruction
    {
        std::string     label;
        std::string     jump_label;
        Instruction     mnemonics{ 0 };
        InstructionType type{ 0 };
        Register        r1{ 0 };
        Register        r2{ 0 };
        Register        r3{ 0 };
        uint16_t        imm16{ 0 };
    };

    void Append(const std::string& label, const std::string& mnemonics, const std::string& op1, const std::string& op2, const std::string& op3)
    {
        _instructions.emplace_back(ParseInstruction(label, mnemonics, op1, op2, op3));
    }

private:
    std::vector<ParsedInstruction> _instructions;

    ParsedInstruction ParseInstruction(const std::string& label, const std::string& mnemonics, const std::string& op1, const std::string& op2, const std::string& op3)
    {
        ParsedInstruction res;

        res.label = label;
        res.mnemonics = ParseMnemonics(mnemonics);

        switch (GetInstructionType(res.mnemonics))
        {
        case InstructionType::OP_R3:
            res.r1 = ParseRegister(op1);
            res.r2 = ParseRegister(op2);
            res.r3 = ParseRegister(op3);
            break;
        case InstructionType::OP_R2_IMM16:
            res.r1 = ParseRegister(op1);
            res.r2 = ParseRegister(op2);
            res.imm16 = ParseImmediate16(op3);
        case InstructionType::OP_R2:
            res.r1 = ParseRegister(op1);
            res.r2 = ParseRegister(op2);
        case InstructionType::OP_R1_IMM16:
            res.r1 = ParseRegister(op1);
            res.imm16 = ParseImmediate16(op2);
        case InstructionType::OP_J:
            res.r1 = ParseRegister(op1);
            res.jump_label = op2;
        case InstructionType::OP:
            break;
        default:
            break;
        }

        return res;
    }

    Instruction ParseMnemonics(const std::string& mnemonics)
    {
        static const std::unordered_map<std::string, Instruction> instruction_map
        {
            { "ADD",    Instruction::ADD    },
            { "ADDI",   Instruction::ADDI   },
            { "SUB",    Instruction::SUB    },
            { "SUBI",   Instruction::SUBI   },
            { "LUI",    Instruction::LUI    },
            { "SHL",    Instruction::SHL    },
            { "SHLI",   Instruction::SHLI   },
            { "SHR",    Instruction::SHR    },
            { "SHRI",   Instruction::SHRI   },
            { "OR",     Instruction::OR     },
            { "ORI",    Instruction::ORI    },
            { "AND",    Instruction::AND    },
            { "ANDI",   Instruction::ANDI   },
            { "XOR",    Instruction::XOR    },
            { "XORI",   Instruction::XORI   },
            { "NOT",    Instruction::NOT    },
            { "LB",     Instruction::LB     },
            { "LBU",    Instruction::LBU    },
            { "LH",     Instruction::LH     },
            { "LHU",    Instruction::LHU    },
            { "LW",     Instruction::LW     },
            { "LWU",    Instruction::LWU    },
            { "SB",     Instruction::SB     },
            { "SH",     Instruction::SH     },
            { "SW",     Instruction::SW     },
            { "CMP",    Instruction::CMP    },
            { "CMPI",   Instruction::CMPI   },
            { "B",      Instruction::B      },
            { "BEQ",    Instruction::BEQ    },
            { "BNE",    Instruction::BNE    },
            { "BGT",    Instruction::BGT    },
            { "BGE",    Instruction::BGE    },
            { "BLT",    Instruction::BLT    },
            { "BLE",    Instruction::BLE    },
            { "J",      Instruction::J      },
            { "JR",     Instruction::JR     },
            { "CALL",   Instruction::CALL   },
            { "CALLR",  Instruction::CALLR  },
            { "RET",    Instruction::RET    },
            { "PUSH",   Instruction::PUSH   },
            { "POP",    Instruction::POP    }
        };

        auto it = instruction_map.find(mnemonics);
        if (it == instruction_map.end())
            throw std::runtime_error(std::string("Invalid mnemonics: ") + mnemonics);
        
        return it->second;
    }

    Register ParseRegister(const std::string& reg)
    {
        static const std::unordered_map<std::string, Register> instruction_map
        {
            { "RZ",     Register::RZ    },
            { "R1",     Register::R1    },
            { "R2",     Register::R2    },
            { "R3",     Register::R3    },
            { "R4",     Register::R4    },
            { "R5",     Register::R5    },
            { "R6",     Register::R6    },
            { "R7",     Register::R7    },
            { "R8",     Register::R8    }
        };

        auto it = instruction_map.find(reg);
        if (it == instruction_map.end())
            throw std::runtime_error(std::string("Invalid register: ") + reg);
        
        return it->second;
    }

    InstructionType GetInstructionType(Instruction instruction)
    {
        static const std::unordered_map<Instruction, InstructionType> instruction_type_map
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
            { Instruction::B,         InstructionType::OP_J             },
            { Instruction::BEQ,       InstructionType::OP_J             },
            { Instruction::BNE,       InstructionType::OP_J             },
            { Instruction::BGT,       InstructionType::OP_J             },
            { Instruction::BGE,       InstructionType::OP_J             },
            { Instruction::BLT,       InstructionType::OP_J             },
            { Instruction::BLE,       InstructionType::OP_J             },
            { Instruction::J,         InstructionType::OP_J             },
            { Instruction::JR,        InstructionType::OP_J             },
            { Instruction::CALL,      InstructionType::OP_J             },
            { Instruction::CALLR,     InstructionType::OP_R1            },
            { Instruction::RET,       InstructionType::OP               },
            { Instruction::PUSH,      InstructionType::OP_R1            },
            { Instruction::POP,       InstructionType::OP_R1            }
        };

        auto it = instruction_type_map.find(instruction);
        if (it == instruction_type_map.end())
            throw std::runtime_error(std::string("Undefined instruction type: ") + std::to_string(static_cast<uint8_t>(instruction)));
        
        return it->second;
    }

    uint16_t ParseImmediate16(const std::string& imm)
    {
        int tmp = std::stoi(imm);
        if (tmp > std::numeric_limits<uint16_t>::max())
            throw std::runtime_error(std::string("Invalid imm16: ") + std::to_string(tmp));
        
        return static_cast<uint16_t>(tmp);
    }


};

class Assembler
{
public:


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
