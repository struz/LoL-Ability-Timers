#include "stdafx.h"
#include "structs.h"

/// <summary>Helper function to get the char* representation
/// of the string from a given STRING_HOLDER struct.</summary>
/// <returns>The direct pointer to the string held in the given
/// STRING_HOLDER.</returns>
char* StringHolder_GetCString(STRING_HOLDER* holder) {
	if (holder->pointerFlag > 15)
		return *reinterpret_cast<char**>(holder->string);
	return holder->string;
}

/// <summary>Helper function to get the string length
/// from a given STRING_HOLDER struct.</summary>
/// <returns>The length of the string in the given STRING_HOLDER.</returns>
DWORD StringHolder_GetStringLen(STRING_HOLDER* holder) {
	return holder->stringLength;
}