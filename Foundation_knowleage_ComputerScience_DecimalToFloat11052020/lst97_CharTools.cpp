#include "lst97_CharTools.h"

// For understanding how int > float || float > int wokrs:
// http://sandbox.mc.edu/~bennet/cs110/flt/ftod.html conversion step by step
// https://www.h-schmidt.net/FloatConverter/IEEE754.html float converter
// https://www.rapidtables.com/convert/number/binary-to-hex.html base converter

float charToFloat(char* pString) {

	isValidFloatFormat(pString);

	int expAndManBin = expAndManToBinary(pString);	// There have a strange rounding problem in the code, 12.05 will become 12.049999

	// Adject signed bit
	if (*pString == '-' && expAndManBin != -1)
		expAndManBin |= LOGICALOPE_POS_31;

	float result;
	memcpy(&result, &expAndManBin, sizeof(float));

	return result;
}

bool isValidFloatFormat(char* pString) {

	bool isValid = false;
	int digitCount = 0;

	if (*pString == '-')
		pString++;

	while (*pString != '.' && digitCount < 9) {
		pString++;
		digitCount++;
	}
	if (digitCount == 8) {
		return isValid;
	}

	return !isValid;
}

int expAndManToBinary(char* pString) {
	int absolute, digitLength, quotient, remainder;
	int fractAbs = 0, mantissa = 0, digitPos = 0, result = 0;
	char* pStringBuffer_EBP = (char*)calloc(32, sizeof(char));
	char* pStringBuffer_ESP = pStringBuffer_EBP;

	if (*pString == '-') {
		*pStringBuffer_ESP = *pString;
		pString++;
		pStringBuffer_ESP++;
	}

	if (*pString == '.') {
		*pStringBuffer_ESP = CHAR_OFFSET;
		pStringBuffer_ESP++;
		strcat(pStringBuffer_ESP, pString);
	}
	else memcpy(pStringBuffer_ESP, pString, sizeof(char) * strlen(pString) + 1);

	// Step 1: Absolute value of the number to binary,
	char* pAbsString = getAbsString(pStringBuffer_EBP);
	if (!isAbsStringValid(pAbsString)) {
		result = -1;
		return result;
	}
	absolute = charToInt(pAbsString);
	free(pAbsString);

	char* pFractString = getFractString(pStringBuffer_EBP);
	int validStage = isFractStringValid(pFractString);
	if (!validStage) {
		result = -1;
		return result;
	}
	digitLength = charToIntEx(pFractString, &fractAbs);
	free(pFractString);

	free(pStringBuffer_EBP);

	if (!(absolute | fractAbs))	// float is 0
		return result;
	
	// Step 2: Convert the fractional part by multiplication, and harvest each one bit as it appears left of the decimal.
	if (validStage == 1) {
		remainder = fractAbs;
		do {
			remainder = (remainder * 2) % (int)pow(10, digitLength);
			quotient = (fractAbs * 2) / (int)pow(10, digitLength);
			mantissa = (mantissa << 1) | quotient;
			fractAbs = remainder;
			digitPos++;
		} while (remainder && digitPos < FLOAT_SIZE);
		if (digitPos != FLOAT_SIZE - 1) {
			digitPos = FLOAT_SIZE - digitPos;
			mantissa <<= digitPos;
		}
	}

	// Algo for calc Exponent & Mantissa
	int exponent = 0;
	if (absolute > 1) {		// Shift left
		while (absolute != 1) {
			mantissa = (mantissa >> 1) & (LOGICALOPE_POS_31 - 1);	// Drop bit && Set the 31st bit to zero

			if (absolute & 1)
				mantissa |= LOGICALOPE_POS_31;	// Set the 31st bit according to abs

			absolute >>= 1;
			exponent++;
		}

		// Adject exponent (make 8th bit true & copy 0-7 bit)
		--exponent = (exponent & (LOGICALOPE_POS_07 - 1)) | LOGICALOPE_POS_07;
	}

	// Shift right (0.x || 1.x)
	// Handle abs == 1 || 0
	if (!(exponent) && absolute)	// For 1.x
		exponent |= LOGICALOPE_POS_07 - 1;

	while (!(mantissa & LOGICALOPE_POS_31) && !absolute) {
		mantissa <<= 1;
		exponent++;
	}
	if (!absolute) {
		mantissa <<= 1;
		++exponent ^= LOGICALOPE_0_0_0_8;
		exponent &= LOGICALOPE_POS_07 - 1;
	}
	
	exponent <<= EXPONENT_STARTPOS;

	// Adject mantissa
	mantissa = ((mantissa & (LOGICALOPE_8_8_8_0 - 1)) >> (FLOAT_SIZE - EXPONENT_STARTPOS)) & MANTISSABYTE;
	//mantissa &= MANTISSABYTE; // Somewhat the compiler will fill '1' after right shift, I don't want it so I have to make sure the high 9th bit is zero.

	// Combine exponent && mantissa
	result |= exponent | mantissa;

	return result;
}

// Critical BUG, calculation will be wrong when process 3 digit long float, investigate when free [lst97 11052020]
char* floatToChar(float scrFloat) {
	int signedBit, exponent, mantissa;
	int absoluteInt = 1, shiftBit = 0, manZeroCount = 0, manLength = MANTISSA_ENDPOS;
	register int temp;	// Usually use as flag bit to do logical operation

	char* pString_ESP = (char*)calloc(64, sizeof(char));
	char* pString_EBP = pString_ESP;

	getSEM(&signedBit, &exponent, &mantissa, scrFloat);

	if (!(signedBit | exponent | mantissa)) {
		strcat(pString_EBP, "0.000000");
		return pString_EBP;
	}

	// Step 2: Exponent
	exponent -= 127;

	// Step 3: Denormalize
	// Calc AbsBin
	temp = LOGICALOPE_POS_22;
	for (int fecx = 1; fecx < exponent; fecx++) {
		shiftBit = mantissa & temp;
		temp >>= 1;
		mantissa ^= shiftBit;
		shiftBit >>= MANTISSA_ENDPOS - fecx;
		absoluteInt = (absoluteInt << 1) | shiftBit;
		manLength--;
	}

	// Count zero fill for mantissa
	temp = (int)pow(2, manLength);
	while (!(mantissa & temp) && mantissa) {
		temp >>= 1;
		manZeroCount++;
	}
	manZeroCount--;

	bool isExponentNeg = false;
	int fractExp = 1;
	unsigned long long int mantissaInt = 0;
	shiftBit ^= shiftBit;

	if (exponent < 0) {
		absoluteInt ^= absoluteInt;
		mantissaInt ^= mantissaInt;
		exponent *= -1;
		isExponentNeg = true;
		mantissa = (mantissa >> exponent) | (int)pow(2, EXPONENT_STARTPOS - exponent);
	}
	temp = (int)pow(2, manLength + manZeroCount - 1);
	while (manLength && fractExp <= LONGLONG_MAXDIGIT) {	// Limited overflow 2^64 = 1.8446744e+19

		if (isExponentNeg)
			temp = (int)pow(2, MANTISSA_ENDPOS - (fractExp - 1));

		shiftBit = (mantissa & temp) >> (manLength);

		mantissaInt += shiftBit * (unsigned long long int)pow(5, fractExp);
		mantissaInt *= 10;

		fractExp++;
		manLength--;
		mantissa &= (temp - 1);

		if (!isExponentNeg)
			temp >>= 1;
	}

	// Fill zero for 0.0x || 0.00x || 0.000x etc
	int zeroFill = 0;
	temp = LONGLONG_MAXDIGIT;	// Expoent that unsigned long long int can handle
	while (!(mantissaInt / (unsigned long long int)pow(10, temp)) && mantissaInt) {
		zeroFill++;
		temp--;
	}

	// Convert sign bit (Float not allow doing logical operation)
	memcpy(&temp, &scrFloat, sizeof(int));
	if (temp & LOGICALOPE_POS_31) {
		*pString_ESP = '-';
		pString_ESP++;
	}

	// Convert absBin to Char
	int digitCount = intToCharEx(pString_ESP, absoluteInt, 0);

	// write '.'
	if (!digitCount)
		pString_ESP++;
	else
		pString_ESP += digitCount;
	*pString_ESP = '.';
	pString_ESP++;

	// Convert manBin to Char
	char* pStringFractAbs = (char*)calloc(32, sizeof(char));
	intToCharEx(pStringFractAbs, mantissaInt, zeroFill);

	strcat(pString_EBP, pStringFractAbs);
	free(pStringFractAbs);

	return pString_EBP;
}

char* getAbsString(char* pString) {
	bool isNegative = false;
	int absCount = 0;
	char* pAbsString = (char*)calloc(16, sizeof(char));
	char* pString_EBP = pString;

	if (*pString == '-') {
		pString++;
		isNegative = true;
	}

	while (*pString != '.') {
		pString++;
		absCount++;
	}
	memcpy(pAbsString, pString_EBP + isNegative, absCount);
	*(pAbsString + absCount + 1) = 0;

	return pAbsString;
}

char* getFractString(char* pString) {
	int fractCount = 0;
	char* pFractString = (char*)calloc(16, sizeof(char));

	while (*pString != '.') {
		pString++;
	}
	pString++;

	char* pString_EBP = pString;
	while (*pString != 0) {
		pString++;
		fractCount++;
	}
	memcpy(pFractString, pString_EBP, fractCount);
	*(pFractString + fractCount + 1) = 0;

	return pFractString;
}

int isFractStringValid(char* pString) {
	int status = -1;

	while (*pString != 0) {
		if (*pString < 0x30 || *pString > 0x40) {
			status = 0;
			break;
		}
		if (*pString > 0x30 && *pString < 0x40) {
			status = 1;
			break;
		}
		pString++;
	}

	return status;
}

bool isAbsStringValid(char* pString) {
	bool isValid = true;

	while (*pString != 0 && (*pString < 0x30 || *pString > 0x40)) {
		isValid = !isValid;
		pString++;
	}
	return isValid;
}

int charToInt(char* pString) {
	char* absStack_EBP = (char*)calloc(16, sizeof(char));
	char* absStack_ESP = absStack_EBP;

	int decx = 0;
	do {
		*absStack_ESP = *pString;
		pString++;
		absStack_ESP++;
		decx++;
	} while (*pString != 0);

	int exponent = 0, result = 0;
	do {
		result += (*(absStack_ESP - 1) - 0x30) * (int)pow(10, exponent);
		absStack_ESP--;
		exponent++;
	} while (absStack_EBP != absStack_ESP);

	return result;
}

// Special use, return number of character read.
int charToIntEx(char* pString, int* _out_pInteger) {
	char* absStack_EBP = (char*)calloc(16, sizeof(char));
	char* absStack_ESP = absStack_EBP;

	int decx = 0;
	do {
		*absStack_ESP = *pString;
		pString++;
		absStack_ESP++;
		decx++;
	} while (*pString != 0);

	int exponent = 0, result = 0;
	do {
		result += (*(absStack_ESP - 1) - 0x30) * (int)pow(10, exponent);
		absStack_ESP--;
		exponent++;
	} while (absStack_EBP != absStack_ESP);
	*_out_pInteger = result;

	return decx;
}

char* intToChar(long long int scrInt) {
	char* pString = (char*)calloc(32, sizeof(char));
	int remainder = 0;
	int wecx = 0;

	char* pStack_EBP = (char*)calloc(32, sizeof(char));
	char* pStack_ESP = pStack_EBP;

	while (scrInt != 0) {
		remainder = scrInt % 10;
		scrInt /= 10;

		*pStack_ESP = remainder + CHAR_OFFSET;
		pStack_ESP++;
		wecx++;
	}
	wecx--;

	pStack_ESP--;
	for (int fecx = 0; fecx < wecx; fecx++) {
		*pString = *pStack_ESP;
		pStack_ESP--;
		pString++;
	}

	free(pStack_EBP);
	*pString = 0;
	pString -= wecx;

	return pString;
}

int intToCharEx(char* pString, long long int scrInt, int zeroFill) {
	int remainder = 0;
	int wecx = 0;

	char* pStack_EBP = (char*)calloc(32, sizeof(char));
	char* pStack_ESP = pStack_EBP;

	if (scrInt == 0) {
		*pStack_ESP = CHAR_OFFSET;
		*pString = *pStack_ESP;
		pString++;
	}
	else {
		while (scrInt != 0) {
			remainder = scrInt % 10;
			scrInt /= 10;

			*pStack_ESP = remainder + 0x30;
			pStack_ESP++;
			wecx++;
		}
	}

	while (zeroFill != 0) {
		*pStack_ESP = CHAR_OFFSET;
		pStack_ESP++;
		zeroFill--;
	}
	pStack_ESP--;

	for (int fecx = 0; fecx < wecx; fecx++) {
		*pString = *pStack_ESP;
		pStack_ESP--;
		pString++;
	}

	free(pStack_EBP);
	*pString = 0;

	return wecx;
}