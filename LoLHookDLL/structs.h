#pragma once
#include "stdafx.h"

/// Struct used by riot to handle strings
// stringPart1-4 will be a string UNLESS
// the string is too large to hold in 16 bytes.
// In this case, stringPart1 holds a pointer to
// the string. Thus the max string length holdable
// is 15, plus the null byte.
struct STRING_HOLDER {
	char string[16]; // 4 DWORDS of string
	DWORD stringLength;
	DWORD pointerFlag;
};

extern char* StringHolder_GetCString(STRING_HOLDER* holder);
extern DWORD StringHolder_GetStringLen(STRING_HOLDER* holder);

// Struct used by the riot to handle coordinates
struct RIOT_COORD {
	float x;
	float y;
	float z;
};