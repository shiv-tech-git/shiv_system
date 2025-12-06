
%code requires {
  #include <string>
}
%define api.value.type {std::string}
%define parse.error verbose
%define parse.trace

%{
#include <iostream>
#include <string>
#include <cstring>
#include "parser.hpp"
#include "isa.h"

int yylex();
extern char* yytext;
void yyerror(const char* s) {
    std::cerr << "Parse error: " << s << " at token '" << yytext << "'" << std::endl;
}
%}

// COMMON
%token REGISTER IMM LABEL LOCAL_LABEL COLON
%token COMMA LBRACK RBRACK
%token INVALID
// DATA
%token D_BYTE D_WORD D_HWORD STRING SPACE STR_VALUE
// SECTIONS
%token TEXT RODATA DATA BSS
// VISIBILITY
%token GLOBL EXTERN
// ARITHMETIC
%token ADD ADDI SUB SUBI LUI
// SHIFTS
%token SHL SHLI SHR SHRI
// LOGICAL
%token OR ORI AND ANDI XOR XORI NOT
// MEMORY
%token LB LBU LH LHU LW SB SH SW
// COMPARE
%token CMP CMPI
// BRANCHES
%token B BEQ BNE BGT BGE BLT BLE 
// CONTROL FLOW
%token J JR CALL CALLR RET 
// STACK
%token PUSH POP

%%
program:
    | program line
    ;

line:
    directive
    | instruction
    | label
    ;

directive:
    TEXT
    | DATA
        { std::cout << "section data" << std::endl; }
    | BSS
        { std::cout << "section bss" << std::endl; }
    | RODATA
        { std::cout << "section rodata" << std::endl; }
    | GLOBL LABEL
        { std::cout << "global" << std::endl; }
    | EXTERN LABEL
        { std::cout << "exterm" << std::endl; }
    | D_BYTE IMM
        { std::cout << "byte " << $2 << std::endl; }
    | D_HWORD IMM
        { std::cout << "hword " << $2 << std::endl; }
    | D_WORD IMM
        { std::cout << "word " << $2 << std::endl; }
    | SPACE IMM
        { std::cout << "space " << $2 << std::endl; }
    | STRING STR_VALUE
        { std::cout << "string: " << $2 << std::endl; }
    ;

label:
    LABEL COLON
        { std::cout << "label: " << $1 << std::endl; }
    | LOCAL_LABEL COLON
        { std::cout << "local label: " << $1 << std::endl; }
    ;

instruction:
    ADD REGISTER COMMA REGISTER COMMA REGISTER
        { std::cout << "ADD " << $2 << "," << $4 << "," << $6 << std::endl; }
    | ADDI REGISTER COMMA REGISTER COMMA IMM
        { std::cout << "ADDI " << $2 << "," << $4 << "," << $6 << std::endl; }
    | SUB REGISTER COMMA REGISTER COMMA REGISTER
        { std::cout << "SUB " << $2 << "," << $4 << "," << $6 << std::endl; }
    | SUBI REGISTER COMMA REGISTER COMMA IMM
        { std::cout << "SUBI " << $2 << "," << $4 << "," << $6 << std::endl; }
    | LUI REGISTER COMMA IMM
        { std::cout << "LUI " << $2 << "," << $4 << std::endl; }
    | SHL REGISTER COMMA REGISTER COMMA REGISTER
        { std::cout << "SHL " << $2 << "," << $4 << "," << $6 << std::endl; }
    | SHLI REGISTER COMMA REGISTER COMMA IMM
        { std::cout << "SHLI " << $2 << "," << $4 << "," << $6 << std::endl; }
    | SHR REGISTER COMMA REGISTER COMMA REGISTER
        { std::cout << "SHR " << $2 << "," << $4 << "," << $6 << std::endl; }
    | SHRI REGISTER COMMA REGISTER COMMA IMM
        { std::cout << "SHRI " << $2 << "," << $4 << "," << $6 << std::endl; }
    | OR REGISTER COMMA REGISTER COMMA REGISTER
        { std::cout << "OR " << $2 << "," << $4 << "," << $6 << std::endl; }
    | ORI REGISTER COMMA REGISTER COMMA IMM
        { std::cout << "ORI " << $2 << "," << $4 << "," << $6 << std::endl; }
    | AND REGISTER COMMA REGISTER COMMA REGISTER
        { std::cout << "AND " << $2 << "," << $4 << "," << $6 << std::endl; }
    | ANDI REGISTER COMMA REGISTER COMMA IMM
        { std::cout << "ANDI " << $2 << "," << $4 << "," << $6 << std::endl; }
    | XOR REGISTER COMMA REGISTER COMMA REGISTER
        { std::cout << "XOR " << $2 << "," << $4 << "," << $6 << std::endl; }
    | XORI REGISTER COMMA REGISTER COMMA IMM
        { std::cout << "XORI " << $2 << "," << $4 << "," << $6 << std::endl; }
    | NOT REGISTER COMMA REGISTER
        { std::cout << "NOT " << $2 << "," << $4 << std::endl; }
    | LB REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "LB " << $2 << ",[" << $5 << "]" << std::endl; }
    | LBU REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "LBU " << $2 << ",[" << $5 << "]" << std::endl; }
    | LH REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "LH " << $2 << ",[" << $5 << "]" << std::endl; }
    | LHU REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "LHU " << $2 << ",[" << $5 << "]" << std::endl; }
    | LW REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "LW " << $2 << ",[" << $5 << "]" << std::endl; }
    | SB REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "SB " << $2 << ",[" << $5 << "]" << std::endl; }
    | SH REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "SH " << $2 << ",[" << $5 << "]" << std::endl; }
    | SW REGISTER COMMA LBRACK REGISTER RBRACK
        { std::cout << "SW " << $2 << ",[" << $5 << "]" << std::endl; }
    | CMP REGISTER COMMA REGISTER
        { std::cout << "CMP " << $2 << "," << $4 << std::endl; }
    | CMPI REGISTER COMMA IMM
        { std::cout << "CMPI " << $2 << "," << $4 << std::endl; }
    | B LABEL
        { std::cout << "B " << $2 << std::endl; }
    | BEQ LABEL
        { std::cout << "BEQ " << $2 << std::endl; }
    | BNE LABEL
        { std::cout << "BNE " << $2 << std::endl; }
    | BGT LABEL
        { std::cout << "BGT " << $2 << std::endl; }
    | BGE LABEL
        { std::cout << "BGE " << $2 << std::endl; }
    | BLT LABEL
        { std::cout << "BLT " << $2 << std::endl; }
    | BLE LABEL
        { std::cout << "BLE " << $2 << std::endl; }
    | J LABEL
        { std::cout << "J " << $2 << std::endl; }
    | JR REGISTER
        { std::cout << "JR " << $2 << std::endl; }
    | CALL LABEL
        { std::cout << "CALL " << $2 << std::endl; }
    | CALLR REGISTER
        { std::cout << "CALL " << $2 << std::endl; }
    | CALL RET
        { std::cout << "RET" << std::endl; }
    | PUSH REGISTER
        { std::cout << "PUSH " << $2 << std::endl; }
    | POP REGISTER
        { std::cout << "POP " << $2 << std::endl; }
    ;
%%
