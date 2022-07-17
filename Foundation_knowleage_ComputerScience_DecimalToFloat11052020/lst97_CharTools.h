#pragma once
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "lst97_FloatTools.h"
#include "lst97_LogicalOperate.h"

#define CHAR_OFFSET 0x30

float charToFloat(char* _in_pString);
char* floatToChar(float scrFloat);

// Covert String to base 10 integer.
int charToInt(char* _in_pString);
int charToIntEx(char* _in_pString, int* _out_pInteger);

char* intToChar(long long int scrInt);
int intToCharEx(char* _in_pString, long long int scrInt, int zeroFill);

// (Useful for float calculation)
// Get absolute value to a char*
bool isValidFloatFormat(char* _in_pString);
int isFractStringValid(char* _in_pString);
bool isAbsStringValid(char* _in_pString);
char* getAbsString(char* _in_pString);
char* getFractString(char* _in_pString);
int expAndManToBinary(char* _in_pString);