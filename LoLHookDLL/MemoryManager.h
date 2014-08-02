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
/// <summary>Contains generic memory access, scanning, and
/// patching functions. Is essentially not a class but a list
/// of functions.</summary>
class MemoryManager
{
private:
	static BYTE ASCIIHexNumToNum(const char* ascii, BYTE &isWildcard);
	static BYTE TryPatternMatch(const BYTE* memory, const BYTE* pattern,
		const BYTE* wildcardMask, int patternSize);
	MemoryManager();
public:
	~MemoryManager();
	static DWORD ScanMemoryForPattern(MODULEENTRY32 moduleToScan, std::string pattern);
	static BYTE PatchByte(DWORD addressToPatch, BYTE newByteValue);
	static DWORD PatchBytesWithJump(DWORD addressToPatch, DWORD jumpTarget, DWORD insertionAddress);
	static DWORD PatchJump(DWORD addressToPatch, int numBytesToPatch, DWORD jumpTarget, BYTE* &pBytesOverwritten);
	static DWORD Patch(DWORD addressToPatch, int numBytesToPatch, BYTE* bytesToUse, BYTE* &pBytesOverwritten);
	static DWORD Patch(DWORD addressToPatch, int numBytesToPatch, BYTE* bytesToUse);
	static MODULEENTRY32 GetModuleInfo(std::wstring moduleName);
	static DWORD* FindD3DDeviceVftablePointer(MODULEENTRY32& d3dModule);
};

