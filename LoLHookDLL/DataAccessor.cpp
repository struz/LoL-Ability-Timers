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
#include "DataAccessor.h"
#include "MemoryManager.h"

// static initialization
DataAccessor* DataAccessor::gpDataAccessor = NULL;

/// <summary>Returns an array of ObjAIAccessor objects that contains
/// every hero listed on the scoreboard of the game. Note that these
/// returned objects must be cleaned up by the CALLER, i.e.
/// looping through and deleting every element in the array.</summary>
/// <param name="pObjAIArray">An array of ObjAIAccessor pointers to fill
/// with the heroes from the scoreboard. Callers responsibility to make
/// the array large enough.</param>
/// <returns>1 if successful, 0 otherwise.</returns>
BOOL DataAccessor::GetScoreboardHeroes(ObjAIAccessor** pObjAIArray) {
	DWORD* pHeroArray = *reinterpret_cast<DWORD**>(
		(DWORD)mModuleInfo.modBaseAddr + mArrayOfChampions
	);
	for (DWORD i = 0; i < GetNumScoreboardHeroes(); i++) {
		pObjAIArray[i] = new ObjAIAccessor(pHeroArray[i]);
	}
	return TRUE;
}

/// <summary>Get whether the scoreboard is currently being displayed.</summary>
/// <returns>Returns a boolean value indicating if the scoreboard
/// is being shown. True for yes, false for no.</returns>
bool DataAccessor::IsScoreboardShown() {
	// This involves a series of pointer referencing which I will
	// explain here in hopes of maintaining this across patches
	// being easier:
	// mpMenu_GUI is a pointer to a Menu_GUI class
	// firstPtr is a pointer to another interim class/struct found at
	// mpMenu_GUI + offset
	// secondPtr is a pointer to a second interim class/struct found at
	// the base of the struct at firstPtr
	// at secondPtr + offset we finally find the HudFlash class
	// for the Scoreboard.swf that we are after.
	// then we return the bool at the HudFlash class plus the
	// required offset
	DWORD menuGUI = *reinterpret_cast<DWORD*>((DWORD)mModuleInfo.modBaseAddr + mpMenu_GUI);
	DWORD hudFlashClass = *reinterpret_cast<DWORD*>(menuGUI + 0x88);
	return *reinterpret_cast<bool*>(hudFlashClass + 0x98);
}

/// <summary>Scans program memory for the offsets used to access game data in League of
/// Legends. Will write to a r3d log using the riot logging function if it cannot find
/// a given offset, but this will fail if it could not find the r3d logging function
/// offset itself.</summary>
/// <returns>Always true.</returns>
bool DataAccessor::GetOffsetsDynamically() {
	DWORD memoryAddress = MemoryManager::ScanMemoryForPattern(mModuleInfo,
		"558BEC83E4F88B55148D45188B4D0850FF7510FF750CE8????????83C40C8BE55DC3");
	// this is now using the entire function signature for R3DLog

	// === LEGACY COMMENT KEPT FOR PROCESS KNOWLEDGE ===
	// what this next line does is turn the relative jump (E8 ?? ?? ?? ??)
	// used to access the riot log function into an absolute jump by adding it
	// to the next instruction address (EIP goes to next instruction address + relative value)
	// then we subtract this absolute value from the base address so that we
	// have the offset (we don't have to do this but it keeps the interface standard
	// with the other values here)
	//mRiotLogOffset = (*reinterpret_cast<DWORD*>(memoryAddress) +
	//	(memoryAddress + 4)) - reinterpret_cast<DWORD>(mModuleInfo.modBaseAddr);
	/// === END LEGACY COMMENT ===

	mRiotLogOffset = memoryAddress - (DWORD)mModuleInfo.modBaseAddr;
	pRiotLog = (riot_log)((DWORD)mModuleInfo.modBaseAddr + mRiotLogOffset);

	// we got the logging function first so that we can use it to log
	// debug info for the rest of the memory scans

	memoryAddress = MemoryManager::ScanMemoryForPattern(mModuleInfo,
		"6A2050E8????????83C4148D45D0B9????????5068????????E8????????A1");
	if (memoryAddress == NULL) {
		GetRiotLog()(3, 1, 0, "Error creating DataAccessor: could not find offset for display clock.");
		return false;
	}
	memoryAddress += 31; // 31 bytes from the match (pattern len)
	mDisplayClockOffset = *reinterpret_cast<DWORD*>(memoryAddress) - reinterpret_cast<DWORD>(mModuleInfo.modBaseAddr);

	memoryAddress = MemoryManager::ScanMemoryForPattern(mModuleInfo,
		"8B0D????????83EC08D905????????83C120DD1C248B01");
	if (memoryAddress == NULL) {
		GetRiotLog()(3, 1, 0, "Error creating DataAccessor: could not find offset for simulation clock.");
		return false;
	}
	memoryAddress += 2; // 2 bytes from the match
	mSimulationClockOffset = *reinterpret_cast<DWORD*>(memoryAddress) - reinterpret_cast<DWORD>(mModuleInfo.modBaseAddr);

	memoryAddress = MemoryManager::ScanMemoryForPattern(mModuleInfo,
		"8B15????????33FF8BB3F400000085D27E11");
	if (memoryAddress == NULL) {
		GetRiotLog()(3, 1, 0, "Error creating DataAccessor: could not find offset for array of global objects (size).");
		return false;
	}
	memoryAddress += 2; // 2 bytes from the match
	mArrayOfGlobalObjects_Size = *reinterpret_cast<DWORD*>(memoryAddress) - reinterpret_cast<DWORD>(mModuleInfo.modBaseAddr);

	// previous DWORD is global object array
	mArrayOfGlobalObjects = mArrayOfGlobalObjects_Size - 4;

	memoryAddress = MemoryManager::ScanMemoryForPattern(mModuleInfo,
		"8945E48945E88945ECE8????????8B3D");
	if (memoryAddress == NULL) {
		GetRiotLog()(3, 1, 0, "Error creating DataAccessor: could not find offset for array of champions.");
		return false;
	}
	memoryAddress += 16; // 16 bytes from the match
	mArrayOfChampions = *reinterpret_cast<DWORD*>(memoryAddress) - reinterpret_cast<DWORD>(mModuleInfo.modBaseAddr);

	// next DWORD is champs array end address
	mArrayOfChampionsEnd = mArrayOfChampions + 4;

	memoryAddress = MemoryManager::ScanMemoryForPattern(mModuleInfo,
		"D84DF0D95DE0F30F1055E08B0D");
	if (memoryAddress == NULL) {
		GetRiotLog()(3, 1, 0, "Error creating DataAccessor: could not find offset for player champion.");
		return false;
	}
	memoryAddress += 13; // 13 bytes from the match
	mPlayerChampion = *reinterpret_cast<DWORD*>(memoryAddress) - reinterpret_cast<DWORD>(mModuleInfo.modBaseAddr);

	memoryAddress = MemoryManager::ScanMemoryForPattern(mModuleInfo,
		"8B0D????????8B01FF50046A018BC88B10FF5230833D");
	if (memoryAddress == NULL) {
		GetRiotLog()(3, 1, 0, "Error creating DataAccessor: could not find offset for the Menu_GUI.");
		return false;
	}
	memoryAddress += 22; // 22 bytes from the match
	mpMenu_GUI = *reinterpret_cast<DWORD*>(memoryAddress) - reinterpret_cast<DWORD>(mModuleInfo.modBaseAddr);

	GetRiotLog()(3, 1, 0, "Successfully retrieved DataAccessor addresses.");

	return true;
}