%option noyywrap

%{
#include "parser.hpp"
#include <cstdlib>
#include <iostream>
%}

REG         R[0-9]+
IMM         [0-9]+
LABEL       [A-Za-z][A-Za-z0-9_]*
LOCAL_LABEL _[A-Za-z][A-Za-z0-9_]*
WS          [ \t\r\n]+

%%
"ADD"       { return ADD; }
"ADDI"      { return ADDI; }
"SUB"       { return SUB; }
"SUBI"      { return SUBI; }
"LUI"       { return LUI; }
"SHL"       { return SHL; }
"SHLI"      { return SHLI; }
"SHR"       { return SHR; }
"SHRI"      { return SHRI; }
"OR"        { return OR; }
"ORI"       { return ORI; }
"AND"       { return AND; }
"ANDI"      { return ANDI; }
"XOR"       { return XOR; }
"XORI"      { return XORI; }
"NOT"       { return NOT; }
"LB"        { return LB; }
"LBU"       { return LBU; }
"LH"        { return LH; }
"LHU"       { return LHU; }
"LW"        { return LW; }
"SB"        { return SB; }
"SH"        { return SH; }
"SW"        { return SW; }
"CMP"       { return CMP; }
"CMPI"      { return CMPI; }
"B"         { return B; }
"BEQ"       { return BEQ; }
"BNE"       { return BNE; }
"BGT"       { return BGT; }
"BGE"       { return BGE; }
"BLT"       { return BLT; }
"BLE"       { return BLE; }
"J"         { return J; }
"JR"        { return JR; }
"CALL"      { return CALL; }
"CALLR"     { return CALLR; }
"RET"       { return RET; }
"PUSH"      { return PUSH; }
"POP"       { return POP; }

".text"     { return TEXT; }
".data"     { return DATA; }
".bss"      { return BSS; }
".rodata"   { return RODATA; }
".globl"    { return GLOBL; }
".extern"   { return EXTERN; }
".byte"     { return D_BYTE; }
".hword"    { return D_HWORD; }
".word"     { return D_WORD; }
".space"    { return SPACE; }
".string"   { return STRING; }

{REG}       { yylval = yytext; return REGISTER; }
{IMM}       { yylval = yytext; return IMM; }
{LABEL}     { yylval = yytext; return LABEL; }
{LOCAL_LABEL}  { yylval = yytext; return LOCAL_LABEL; }

\"([^\\\"]|\\.)*\"    {
    // Strip quotes and store the string
    std::string striped(yytext);
    // remove leading and trailing quotes
    striped = striped.substr(1, striped.size()-2);
    yylval = striped;
    return STR_VALUE;
}

":"         { return COLON; }
","         { return COMMA; }
"["         { return LBRACK; }
"]"         { return RBRACK; }

{WS}        { /* skip */ }
.           { std::cerr << "LEX: unknown '" << yytext << "'" << std::endl; return INVALID; }

%%
