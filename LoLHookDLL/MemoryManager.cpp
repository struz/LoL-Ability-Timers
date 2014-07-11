#include "stdafx.h"
#include "MemoryManager.h"

MemoryManager::MemoryManager()
{
}


MemoryManager::~MemoryManager()
{
}

/// <summary>Helper function to turn a string of length 2 containing
/// a hex representation of a number into a BYTE containing the number.</summary>
/// <param name="ascii">String of length 2 containing a hex representation of
/// a number.</param>
/// <param name="isWildcard">Variable passed by reference which is filled
/// with 1 if the ascii character contained "??" and 0 otherwise.</param>
/// <returns>A byte containing the number in parameter "ascii".</returns>
BYTE MemoryManager::ASCIIHexNumToNum(const char* ascii, BYTE &isWildcard) {
	int wildcardCheck = strcmp(ascii, "??");
	isWildcard = (wildcardCheck == 0) ? 1 : 0;

	if (isWildcard)
		return 0;

	BYTE num = (BYTE)strtol(ascii, NULL, 16);
	return num;
}

/// <summary>Tries to match a byte pattern of a given size with a region of program
/// memory given a wildcard mask. Must match byte for byte from the start of
/// memory to the end of memory.</summary>
/// <param name="memory">The memory to try and match with pattern, byte
/// for byte, starting at index 0 and ending at index patternSize.</param>
/// <param name="pattern">The byte pattern to try and match with memory.</param>
/// <param name="wildcardMask">A series of bytes that must be at least of length
/// patternSize and contains a 1 if the byte at that index within the pattern
/// can match anything, or a 0 if the byte at that index within pattern must match
/// the byte in memory.</param>
/// <param name="patternSize">The size of the memory, pattern and wildcardMask arrays.
/// All of these arrays must be at least of this size.</param>
/// <returns>TRUE if the entire pattern matches the entire block of memory, accounting
/// for wildcards in the wildcardMask, FALSE otherwise.</returns>
BYTE MemoryManager::TryPatternMatch(const BYTE* memory, const BYTE* pattern,
	const BYTE* wildcardMask, int patternSize) {
	for (int i = 0; i < patternSize; i++) {
		if (pattern[i] != memory[i] && wildcardMask[i] == 0)
			return FALSE;
	}
	return TRUE;
}

/// <summary>Scans a specific module for a byte pattern provided in string
/// format.</summary>
/// <param name="moduleToScan">A MODULEENTRY32 describing the module to scan.
/// This is used to get the module start address and size so we scan only this
/// module.</param>
/// <param name="pattern"><para>An std::string that describes the byte pattern
/// to search for. The string must adhere to the following rules:</para>
/// <para>1: length must be divisible by 2.</para>
/// <para>2: must contain only characters between 0-9, A-F (can be either upper
/// or lowercase) and ?</para>
/// <para>3: must be arranged into text describing bytes IN ORDER e.g.
/// "DEAD12BEEF05" would describe the sequence of bytes DE AD 12 BE EF 05.</para>
/// <para>The usage of two question marks (??) in a row means that the particular
/// byte being described will match ANY byte. For example, "BE??EF" will match
/// both BEADEF and BE00EF, and any other combination with the middle byte.</para></param>
/// <returns>Returns the memory address of the start of that pattern if
/// found, NULL if it is not found.</returns>
DWORD MemoryManager::ScanMemoryForPattern(MODULEENTRY32 moduleToScan, std::string pattern) {
	if (pattern.length() % 2 != 0)
		throw;

	size_t patternLen = pattern.length() / 2;
	BYTE* wildcardMask = new BYTE[patternLen]; // is 1 wherever a wildcard exists
	BYTE* patternBytes = new BYTE[patternLen]; // the byte values to search for
	char* patternCStr = new char[pattern.length() + 1];

	// work out the pattern
	strcpy_s(patternCStr, pattern.length() + 1, pattern.c_str());
	int i = 0;
	char asciiHexNum[3];

	// we checked the pattern length at the start of the function and
	// exited if it wasn't divisible by 2, therefore we should always
	// hit the required NULL terminator.
	for (char* curChar = patternCStr; *curChar != NULL; curChar += 2) {
		memcpy(asciiHexNum, curChar, 2);
		asciiHexNum[2] = NULL; // create the num string

		patternBytes[i] = ASCIIHexNumToNum(asciiHexNum, wildcardMask[i]);
		i++;
	}

	// do the actual scanning
	DWORD matchAddress = NULL;
	for (BYTE* curByte = moduleToScan.modBaseAddr;
		curByte < (moduleToScan.modBaseAddr + moduleToScan.modBaseSize);
		curByte++) {
		if (TryPatternMatch(curByte, patternBytes, wildcardMask, patternLen)) {
			matchAddress = (DWORD)curByte;
			break;
		}
	}

	// cleanup
	delete[] patternCStr;
	delete[] wildcardMask;

	return matchAddress;
}


/// <summary>
/// Patch a single byte at a specified address to have a new specific byte value.
/// Returns the old value of the byte.
/// </summary>
/// <param name="addressToPatch">The address of the byte to patch.</param>
/// <param name="newByteValue">The value to place in the byte.</param>
/// <returns>The value in the byte before it was patched.</returns>
BYTE MemoryManager::PatchByte(DWORD addressToPatch, BYTE newByteValue) {
	BYTE overwrittenByte = 0x0;

	BYTE* byteToWrite = (BYTE*)addressToPatch;
	overwrittenByte = *byteToWrite;
	*byteToWrite = newByteValue;

	return overwrittenByte;
}

/// <summary>
/// Patch bytes at the given address in such a way that it creates the asm
/// instruction 'jmp ds:[jumpTarget]'. Requires 10 bytes.
/// </summary>
/// <param name="addressToPatch">The address of the bytes to patch.</param>
/// <param name="jumpTarget">The address of the target for the jump.</param>
/// <param name="insertionAddress">The address at which the patched bytes
/// will eventually end up, in case of calling PatchBytesWithJump on
/// a local array before patching it into program memory later. Will be
/// ignored if null (0).</param>
/// <returns>The number of bytes patched by this function.</returns>
DWORD MemoryManager::PatchBytesWithJump(DWORD addressToPatch, DWORD jumpTarget, DWORD insertionAddress) {
	BYTE* pByteAddressToPatch = reinterpret_cast<BYTE*>(addressToPatch);
	pByteAddressToPatch[0] = 0xFF; // opcode for jmp [mem], when combined with correct r/m byte
	pByteAddressToPatch[1] = 0x25; // r/m byte - mod = 00 reg/opcode = 100 r/m = 101

	// copy the address at which to find the jump address
	// note that if an insertion address is supplied we use that
	// instead of the address to patch
	// this allows the function to double as a byte preparation
	// function that can copy byte strings into arrays to pre-compile patches
	*reinterpret_cast<DWORD *>(addressToPatch + 2) =
		(insertionAddress == NULL) ? (addressToPatch + 6)
		: (insertionAddress + 6);

	*reinterpret_cast<DWORD *>(addressToPatch + 6) = jumpTarget; // copy the jump address
	// the final form of the memory is this:
	// +0 (0xFF 0x25) - the opcode / instruction description
	// +2 (0x?? 0x?? 0x?? 0x??) - address pointing to +6, where we hold the jump address
	// +6 (0x?? 0x?? 0x?? 0x??) - the jump address at which to jump
	// the final form of the instruction is: jmp ds:[addressToPatch+0x6]
	return 10;
}

/// <summary>
/// Patch some bytes in the target process to create a jump to the target address.
/// </summary>
/// <param name="jumpTarget">Function to set as patch jump target.</param>
/// <param name="addressToPatch">Address in process memory to patch.</param>
/// <param name="numBytesToPatch">Number of bytes to overwrite. Must be at least 10 bytes.</param>
/// <param name="pBytesOverwritten">Optional: An array of bytes in which to return the
/// overwritten bytes. Must have size >= numBytesToPatch if provided.</param>
/// <returns>0 if completed successfully, system error code if not.</returns>
DWORD MemoryManager::PatchJump(DWORD addressToPatch, int numBytesToPatch, DWORD jumpTarget, BYTE* &pBytesOverwritten) {
	BYTE* bytesToPatch = new BYTE[numBytesToPatch];
	// patch our memory to create our jump in our byte list
	DWORD bytesAlreadyWritten = PatchBytesWithJump(
		reinterpret_cast<DWORD>(bytesToPatch), jumpTarget, addressToPatch
		);
	// add any extra nops on the end
	memset(reinterpret_cast<void*>(bytesToPatch + bytesAlreadyWritten),
		0x90, numBytesToPatch - bytesAlreadyWritten);
	// patch the actual memory address with our list of bytes
	DWORD result = Patch(addressToPatch, numBytesToPatch, bytesToPatch, pBytesOverwritten);
	delete[] bytesToPatch;
	return result;
}

/// <summary>
/// Patch some bytes in the target process with the specified bytes.
/// </summary>
/// <param name="addressToPatch">Address in process memory to patch.</param>
/// <param name="numBytesToPatch">Number of bytes to overwrite.</param>
/// <param name="pBytesOverwritten">An array of bytes in which to return the
/// overwritten bytes. Must have size >= numBytesToPatch.</param>
/// <param name="bytesTouse">The bytes to copy into the address.</param>
/// <returns>0 if completed successfully, system error code if not.</returns>
DWORD MemoryManager::Patch(DWORD addressToPatch, int numBytesToPatch, BYTE* bytesToUse, BYTE* &pBytesOverwritten) {
	ULONG hold = NULL, hold2 = NULL;
	BOOL result = 0;
	result = VirtualProtect(reinterpret_cast<void*>(addressToPatch),
		numBytesToPatch, PAGE_EXECUTE_READWRITE, &hold);
	if (result == 0)
		return GetLastError();

	// copy the bytes we're writing over into the supplied buffer
	memcpy(pBytesOverwritten, reinterpret_cast<void*>(addressToPatch), numBytesToPatch);
	// copy the bytes into the memory address
	memcpy(reinterpret_cast<void*>(addressToPatch), bytesToUse, numBytesToPatch);

	result = VirtualProtect(reinterpret_cast<void*>(addressToPatch),
		numBytesToPatch, hold, &hold2);
	if (result == 0)
		return GetLastError();
	return 0;
}

/// <summary>Overload for an optional parameter pBytesOverwritten.</summary>
DWORD MemoryManager::Patch(DWORD addressToPatch, int numBytesToPatch, BYTE* bytesToUse) {
	BYTE* thrownAwayBytesOverwritten = new BYTE[numBytesToPatch];
	DWORD result = Patch(addressToPatch, numBytesToPatch, bytesToUse, thrownAwayBytesOverwritten);
	delete[] thrownAwayBytesOverwritten;
	return result;
}

/// <summary>Get the module info for the specified module in the
/// current process.</summary>
/// <param name="moduleName">The module name about which to retrieve
/// information as a std::wstring.</param>
/// <returns>A MODULEENTRY32 describing the module.</returns>
MODULEENTRY32 MemoryManager::GetModuleInfo(std::wstring moduleName) {
	DWORD pid = GetCurrentProcessId();

	HANDLE hSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 Mod32 = { 0 };

	if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid)) == INVALID_HANDLE_VALUE)
		return Mod32;

	Mod32.dwSize = sizeof(MODULEENTRY32);
	while (Module32Next(hSnap, &Mod32)) {
		if (moduleName.compare(Mod32.szModule) == 0) {
			CloseHandle(hSnap);
			return Mod32;
		}
	}

	// We didn't find a match, return empty module
	CloseHandle(hSnap);
	memset(&Mod32, 0, sizeof(Mod32));
	return Mod32;
}

/// <summary>Finds a pointer to the D3D device vftable. THIS WILL BLOCK
/// until the D3D module can be located in memory, and then search
/// for the pointer.</summary>
/// <param name="d3dModule">The MODULEENTRY32 describing the D3D module
/// returned to the caller.</param>
/// <returns>The pointer to the vftable.</returns>
DWORD* MemoryManager::FindD3DDeviceVftablePointer(MODULEENTRY32& d3dModule) {
	// Get the memory location of the D3D device	
	d3dModule = MemoryManager::GetModuleInfo(D3D_DLL_NAME);
	while (d3dModule.dwSize == 0) { // wait for d3d to load
		Sleep(500);
		d3dModule = MemoryManager::GetModuleInfo(D3D_DLL_NAME);
	}

	// Find the pointer to the vftable
	DWORD* pDeviceVftable = *reinterpret_cast<DWORD**>(
		MemoryManager::ScanMemoryForPattern(d3dModule, "C706????????8986????????8986")
		+ 2);
	return pDeviceVftable;
}