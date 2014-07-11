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