#pragma once
/// <summary>Contains generic memory access, scanning, and
/// patching functions.</summary>
class MemoryManager
{
private:
	static BYTE ASCIIHexNumToNum(const char* ascii, BYTE &isWildcard);
	static BYTE TryPatternMatch(const BYTE* memory, const BYTE* pattern,
		const BYTE* wildcardMask, int patternSize);
public:
	MemoryManager();
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

