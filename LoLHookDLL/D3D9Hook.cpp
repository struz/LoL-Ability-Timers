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
#include "D3D9Hook.h"
#include "MemoryManager.h"
#include "DataAccessor.h"

/// <summary>Standard constructor for a D3D9Hook.</summary>
/// <param name="vftableIndex">The index in a LPDIRECT3DDEVICE9
/// virtual function table of the function to hook with this instance
/// of the class.</param>
/// <param name="numBytesToPatch">The number of bytes to patch
/// while detouring the function. Must be at least 10.</param>
/// <param name="replacementFundAddr">The address of the function
/// to replace the target function with.</param>
D3D9Hook::D3D9Hook(int vftableIndex, int numBytesToPatch,
		DWORD replacementFuncAddr) : IHook() {
	CallRiotLog("Running constructor on D3D9Hook base class.");

	if (numBytesToPatch < 10) throw;

	// Get the memory location of the D3D device vftable
	mpDeviceVftable = MemoryManager::FindD3DDeviceVftablePointer(mD3DModule);
	DWORD pVfuncPresent = *(mpDeviceVftable + vftableIndex); // go to the correct pointer
	mFuncAddr = pVfuncPresent; // store the address for use by our hooked version

	mReplacementFuncAddr = replacementFuncAddr; // store the replacement func

	mBytesReplaced = new BYTE[numBytesToPatch]; // setup our unpatching mechanism
	mNumBytesToPatch = numBytesToPatch;
	CallRiotLog("Finished running constructor on D3D9Hook base class.");
}

D3D9Hook::~D3D9Hook() {
	if (mBytesReplaced)
		delete[] mBytesReplaced;
}

/// <summary>Perform a hook on the stored function of the default d3d device.</summary>
/// <returns>0 if completed successfully, system error code if not.</returns>
DWORD D3D9Hook::DoHook() {
	DWORD result = 1;
	if (!IsHooked()) {
		result = MemoryManager::PatchJump(mFuncAddr, mNumBytesToPatch, mReplacementFuncAddr, mBytesReplaced);
		if (result == 0)
			SetHooked(TRUE);
	}
	return result;
}

/// <summary>Unhook the hook on the stored function, if one exists, of the default d3d device.</summary>
/// <returns>0 if completed successfully, system error code if not.</returns>
DWORD D3D9Hook::DoUnhook() {
	DWORD result = 1;
	if (IsHooked()) {
		result = MemoryManager::Patch(mFuncAddr, mNumBytesToPatch, mBytesReplaced);
		SetHooked(FALSE);
	}
	return result;
}