# Assembler specification

Below is a compact, practical definition of an assembler syntax for your CPU, aligned with the Core API and flag semantics you implemented. It includes the token rules, instruction forms, addressing, labels, and a minimal directive set, with examples you can assemble into WORD-addressed RAM.

---

## Lexical conventions

- **Whitespace:** Spaces, tabs, and newlines separate tokens.
- **Case:** Mnemonics and registers are case-insensitive; labels and symbols are case-sensitive by default.
- **Numbers:** Decimal (`42`), hexadecimal (`0x2A`), binary (`0b101010`). All immediates are unsigned unless context specifies signed behavior (e.g., subtraction overflow).
- **Identifiers:** `[A-Za-z_][A-Za-z0-9_]*`
- **Comments:** Start with `;` and go to end of line.
- **Strings:** Double-quoted for data directives.
- **Registers:** `RZ, R1..R8, RA, IP, SP, FLAGS`

---

## Syntax overview

```ebnf
program      := { line }
line         := [ label ":" ] [ statement ] [ ";" comment ]
statement    := instruction | directive
label        := ident

; Directives
directive    := ".text"
             | ".data"
             | ".global" ident
             | ".extern" ident
             | ".align" number
             | ".org" number
             | ".word" number { "," number }
             | ".byte" number { "," number }
             | ".ascii" string
             | ".asciz" string

; Operands
reg          := "RZ" | "R1" | "R2" | "R3" | "R4" | "R5" | "R6" | "R7" | "R8" | "RA" | "IP" | "SP" | "FLAGS"
imm          := number
addr         := "[" reg "]"                   ; register-indirect addressing

; Instructions (core forms)
instruction  := "ADD" reg "," reg "," reg
             | "ADDI" reg "," reg "," imm
             | "SUB" reg "," reg "," reg
             | "SUBI" reg "," reg "," imm
             | "CMP" reg "," reg
             | "CMPI" reg "," imm
             | "LDR" reg "," addr
             | "STR" reg "," addr
             | "LI"  reg "," imm
             | "LUI" reg "," imm
             | "JMP" target
             | "CALL" target
             | "RET"
             | "BEQ" target
             | "BNE" target
             | "BLT" target
             | "BLE" target
             | "BGT" target
             | "BGE" target
             | "SHL" reg "," reg "," reg
             | "SHR" reg "," reg "," reg
             | "OR"  reg "," reg "," reg
             | "AND" reg "," reg "," reg
             | "XOR" reg "," reg "," reg
             | "PUSH" reg
             | "POP"  reg

target       := number | ident
```

---

## Instruction set and semantics

### Arithmetic and compare

- **ADD dst, src1, src2**
  - **Effect:** dst ← src1 + src2 (mod WORD)
  - **Flags:** **Z, N, C, V** updated per DoAdd
- **ADDI dst, src, imm**
  - **Effect:** dst ← src + imm
  - **Flags:** **Z, N, C, V** updated

- **SUB dst, src1, src2**
  - **Effect:** dst ← src1 − src2 (implemented as add with two’s complement)
  - **Flags:** **Z, N, C, V** updated per DoSub (ARM-style carry: C=1 no borrow)
- **SUBI dst, src, imm**
  - **Effect:** dst ← src − imm
  - **Flags:** **Z, N, C, V** updated

- **CMP src1, src2**
  - **Effect:** flags from (src1 − src2); registers unchanged
  - **Flags:** **Z, N, C, V** updated
- **CMPI src, imm**
  - **Effect:** flags from (src − imm); registers unchanged
  - **Flags:** **Z, N, C, V** updated

### Logical and shifts

- **OR dst, src1, src2** → dst ← src1 | src2; flags **Z, N** updated
- **AND dst, src1, src2** → dst ← src1 & src2; flags **Z, N** updated
- **XOR dst, src1, src2** → dst ← src1 ^ src2; flags **Z, N** updated
- **SHL dst, src, sh** → dst ← src << sh; flags **Z, N** updated
- **SHR dst, src, sh** → dst ← src >> sh (logical); flags **Z, N** updated

### Immediates and upper-load

- **LI dst, imm** → dst ← imm; flags unchanged
- **LUI dst, imm** → dst ← imm << (sizeof(HWORD) · 8); flags unchanged

### Memory

- **LDR dst, [addrReg]** → dst ← RAM[ Reg(addrReg) ]; flags unchanged
- **STR src, [addrReg]** → RAM[ Reg(addrReg) ] ← Reg(src); flags unchanged

### Stack

- **PUSH src**
  - **SP ← SP − WORD; RAM[SP] ← Reg(src)**
- **POP dst**
  - **dst ← RAM[SP]; SP ← SP + WORD**

### Control flow

- **JMP target** → **IP ← addr(target)**
- **CALL target** → **RA ← IP + WORD; IP ← addr(target)**
- **RET** → **IP ← RA**

### Conditional branches

- Predicates derive from flags: **Z, N, V**
  - **BEQ target** → if Z==1 then IP ← addr
  - **BNE target** → if Z==0 then IP ← addr
  - **BLT target** → if N != V then IP ← addr
  - **BLE target** → if Z==1 or (N != V) then IP ← addr
  - **BGT target** → if not (Z==1 or N != V) then IP ← addr
  - **BGE target** → if N == V then IP ← addr

---

## Directives and labels

- **.text / .data:** Switch current section (emission address space). If you keep a flat address space, `.text` starts code emission; `.data` starts data emission.
- **.global name:** Export symbol.
- **.extern name:** Declare external symbol (linked elsewhere).
- **.align n:** Align location counter to multiple of n bytes.
- **.org addr:** Set location counter to absolute address.
- **.word values...:** Emit WORD(s) (endian per RAM implementation).
- **.byte values...:** Emit raw bytes.
- **.ascii "str":** Emit string without terminating NUL.
- **.asciz "str":** Emit string with terminating NUL.

- **Labels:** Bind current location counter to a symbol. Used as **target** in `JMP/CALL/BEQ/...` and as addresses in data.

---

## Examples

### 1) Hello of arithmetic/branch (compare two numbers)

```asm
.text
.global _start
_start:
    LI   R1, 50            ; R1 = 50
    LI   R2, 8             ; R2 = 8
    SUB  R3, R1, R2        ; R3 = 42, flags updated
    CMPI R3, 42            ; set Z if equal
    BNE  not_equal         ; branch if R3 != 42

equal:
    ; fallthrough to done
    JMP  done

not_equal:
    ; handle error
    JMP  done

done:
    RET
```

### 2) Load/store and stack

```asm
.data
msg:    .asciz "Hi\0"
.text
.global main
main:
    ; Suppose R4 points to msg address (loader/linker resolves labels)
    LI   R4, msg           ; pseudo: assembler/linker resolves label to address
    LDR  R1, [R4]          ; R1 = RAM[msg]
    PUSH R1                ; push first WORD of msg
    POP  R2                ; pop into R2 (restores SP)
    RET
```

### 3) Function call/return pattern

```asm
.text
.global _start
_start:
    CALL  add42
    JMP   exit

add42:
    LI    R1, 13
    LI    R2, 29
    ADD   R3, R1, R2       ; 13 + 29 = 42
    RET

exit:
    RET
```

### 4) Signed compare branches

```asm
.text
cmp_example:
    LI   R1, 0x80000000    ; INT_MIN
    LI   R2, 1
    SUB  R3, R1, R2        ; R3 = 0x7FFFFFFF, V=1, C=1, N=0, Z=0
    BLT  neg_result        ; N != V? Here N(0) != V(1) => true -> signed less-than
    BGE  ge_result         ; otherwise
neg_result:
    RET
ge_result:
    RET
```

---

## Assembler behavior notes

- **Immediate range:** Must fit in WORD; assembler validates numeric literals.
- **Label resolution:** Targets in `JMP/CALL/BEQ/...` accept either numeric addresses or labels. Assembler/linker resolve labels to absolute addresses placed into instruction operands (the Core API uses absolute addresses).
- **No PC-relative addressing:** Branches and calls use absolute targets (consistent with `Jump(addr)` / `Call(addr)`).
- **Memory addressing:** Only register-indirect `[Rn]` is defined at ISA surface; add indexed forms later if needed.
- **Flags update policy:** Exactly as in Core:
  - **ADD/SUB/CMP:** Update Z, N, C, V
  - **Logic/Shift:** Update Z, N
  - **Memory/Stack/Control:** Do not modify flags

---

If you want, I can extend this with a compact binary encoding (opcode map + operand fields) and an example linker script that places `.text` at a chosen base and `.data` after it, so your RAM image aligns with IP expectations.