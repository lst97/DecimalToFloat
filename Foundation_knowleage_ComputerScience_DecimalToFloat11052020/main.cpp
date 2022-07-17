/*
;|**********************************************************************;
;* Project           : CPP-Language_practise
;*
;* Program name      : FloatConversion.cpp
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 11/05/2020
;*
;* Version           : internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Demonstrate how to convert char > float || float > char
;*
;* Revision History  :
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 11052020    lst97       1      First release
;*
;* Known Issue       :
;* 1. It only for demonstrate how to convert float to char, therefore it may not handle all formating problem, for example --0.2 || 5..11
;* 2. Critical BUG in floatToChar(), calculation will be wrong when process 3 digits long float number
;* 3. May not be accurate with some value in charToFloat() function. For example 728.21 > 728.209961
;*
;* TODO              :
;* 
;|**********************************************************************;
*/

#include "stdio.h"
#include "lst97_CharTools.h"

int main(int argc, char* argv[]) {

	printf("%f\n", charToFloat((char*)"0.21"));
	printf("%s", floatToChar((float)555.2));

	getchar();
	return 0;
}