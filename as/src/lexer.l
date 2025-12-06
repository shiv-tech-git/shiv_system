%option noyywrap

%{
#include "parser.hpp"
#include <cstdlib>
#include <iostream>
%}

REG     R[0-9]+
IMM     [0-9]+
LABEL   [A-Za-z_][A-Za-z0-9_]*
WS      [ \t\r\n]+

%%
"ADD"    { return ADD; }
"ADDI"   { return ADDI; }
"SUB"    { return SUB; }
"SUBI"   { return SUBI; }
"LUI"    { return LUI; }
"SHL"    { return SHL; }
"SHLI"   { return SHLI; }
"SHR"    { return SHR; }
"SHRI"   { return SHRI; }
"OR"     { return OR; }
"ORI"    { return ORI; }
"AND"    { return AND; }
"ANDI"   { return ANDI; }
"XOR"    { return XOR; }
"XORI"   { return XORI; }
"NOT"    { return NOT; }
"LB"     { return LB; }
"LBU"    { return LBU; }
"LH"     { return LH; }
"LHU"    { return LHU; }
"LW"     { return LW; }
"SB"     { return SB; }
"SH"     { return SH; }
"SW"     { return SW; }
"CMP"    { return CMP; }
"CMPI"   { return CMPI; }
"B"      { return B; }
"BEQ"    { return BEQ; }
"BNE"    { return BNE; }
"BGT"    { return BGT; }
"BGE"    { return BGE; }
"BLT"    { return BLT; }
"BLE"    { return BLE; }
"J"      { return J; }
"JR"     { return JR; }
"CALL"   { return CALL; }
"CALLR"  { return CALLR; }
"RET"    { return RET; }
"PUSH"   { return PUSH; }
"POP"    { return POP; }

{REG}    { yylval = yytext; return REGISTER; }
{IMM}    { yylval = yytext; return IMM; }
{LABEL}  { yylval = yytext; return LABEL; }
":"      { return COLON; }

","      { return COMMA; }
"["      { return LBRACK; }
"]"      { return RBRACK; }

{WS}     { /* skip */ }
.        { std::cerr << "LEX: unknown '" << yytext << "'" << std::endl; return INVALID; }

%%
