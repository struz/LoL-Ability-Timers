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
#include "ObjectAccessor.h"

NullReferenceException NULL_REF_EX;

/// <summary>Given the base address of a class (which should point to the
/// classes vftable), calls the function at the given offset within the class
/// vftable and returns the value from that function.</summary>
/// <param name="classAddr">The base address of the class to perform the
/// function on.</param>
/// <param name="offset">The offset into the classes vftable at which the
/// function to call can be found.</param>
/// <returns>The value returned by the called function.</returns>
DWORD ObjectAccessor::CallVtableFunction(DWORD classAddr, DWORD offset) {
	DWORD retVal = 0;

	// to fix an issue with the name "offset" being unparsable by the compiler
	DWORD moreFullyNamedOffset = offset;

	__asm {
		push ecx
		push ebx
		push edx
		mov ecx, classAddr
		mov ebx, [ecx] // the vtable addr
		mov edx, moreFullyNamedOffset
		mov eax, [ebx + edx] // eax = whats in vtable addr + offset = the vtable func addr
		call eax
		mov retVal, eax
		pop edx
		pop ebx
		pop ecx
	}
	
	return retVal;
}