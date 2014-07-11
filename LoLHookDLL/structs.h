/*  LoL Ability Timers. Injects into League of Legends to show ability
timers for all champions.
Copyright (C) 2014  Matthew Whittington

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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