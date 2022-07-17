#pragma once
#include "string.h"
#include "lst97_CharTools.h"
#include "lst97_LogicalOperate.h"

#define FLOAT_SIZE 32

#define EXPONENTBYTE 0x7F800000
#define MANTISSABYTE 0x007FFFFF
#define EXPONENT_STARTPOS 23
#define EXPONENT_ENDPOS 30
#define MANTISSA_STARTPOS 0
#define MANTISSA_ENDPOS 22

#define LONGLONG_MAXDIGIT 18

void getSEM(int* signedBit, int* exponent, int* mantissa, float scrFloat);