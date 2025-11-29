#pragma once

#include <inttypes.h>

using BYTE  = uint8_t;
using HWORD = uint16_t;
using WORD  = uint32_t;
using DWORD = uint64_t;

const static WORD MSB_I = ((sizeof(WORD) * 8) - 1);
const static WORD CB_I = ((sizeof(WORD) * 8));
