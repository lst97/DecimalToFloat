#include "lst97_FloatTools.h"

void getSEM(int* _in_signedBit, int* _in_exponent, int* _in_mantissa, float scrFloat) {
	int scrFloatInt;

	if (scrFloat == 0.0) {
		*_in_signedBit = 0;
		*_in_exponent = 0;
		*_in_mantissa = 0;
		return;
	}

	memcpy(&scrFloatInt, &scrFloat, sizeof(int));

	if (scrFloatInt & LOGICALOPE_POS_31)
		*_in_signedBit = 1;
	else 
		*_in_signedBit ^= *_in_signedBit;
	
	*_in_exponent = (scrFloatInt & EXPONENTBYTE) >> EXPONENT_STARTPOS;

	*_in_mantissa = scrFloatInt & MANTISSABYTE;

	return;
}