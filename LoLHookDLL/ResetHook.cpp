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
#include "ResetHook.h"
#include "MemoryManager.h"
#include "D3DDisplayManager.h"
#include "DataAccessor.h"

// Global instance of our reset hook, required for our
// hooked version of Reset to hook and unhook each time it is called
ResetHook* gpResetHook = NULL;

/// <summary><para>Hooked version of LPDIRECT3DDEVICE9::Reset(). Thus it
/// takes the same parameters (plus a "this" pointer as the first
/// argument, implicit in the real definition). Must unhook and
/// rehook itself each time it is called to return the required
/// functionality to the caller.</para>
/// <para>The purpose of this function is to reset the resources
/// used by the global D3DDisplayManager instance. If they are
/// not reset when the device's reset function is called then
/// the application will crash.</para></summary>
/// <param name="pDevice">LPDIRECT3DDEVICE9 "this" pointer, i.e.
/// the calling device.</param>
/// <param name="pPresentationParameters">See MSDN doc for
/// LPDIRECT3DDEVICE9::Reset()</param>
/// <returns>See MSDN doc for LPDIRECT3DDEVICE9::Reset()</returns>
HRESULT __stdcall HookedD3DReset(LPDIRECT3DDEVICE9 pDevice,
		D3DPRESENT_PARAMETERS* pPresentationParameters) {
	HRESULT result;

	CallRiotLog("Reset() called.");
	if (gpDisplayManager)
		gpDisplayManager->DeviceReset();

	// Restore the old function
	gpResetHook->DoUnhook();
	// Call the original function
	result = pDevice->Reset(pPresentationParameters);
	// Restore our patch
	gpResetHook->DoHook();
	CallRiotLog("Reset() returned: %d", result);

	return result;
}

/// <summary>Class constructor for a ResetHook. Simply
/// logs its creation and calls parent constructor.</summary>
ResetHook::ResetHook() : D3D9Hook(D3D_RESET_VFTABLEINDEX,
		D3D_RESET_BYTESTOPATCH, (DWORD)HookedD3DReset) {
	CallRiotLog("Running constructor on ResetHook class.");
	CallRiotLog("ResetHook class created successfully.");
}

ResetHook::~ResetHook()
{

}