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
#include "PresentHook.h"
#include "MemoryManager.h"
#include "D3DDisplayManager.h"
#include "DataAccessor.h"

// Global instance of our present hook, required for our
// hooked version of Present to hook and unhook each time it is called
PresentHook* gpPresentHook = NULL;

/// <summary><para>Hooked version of LPDIRECT3DDEVICE9::Present(). Thus it
/// takes the same parameters (plus a "this" pointer as the first
/// argument, implicit in the real definition). Must unhook and
/// rehook itself each time it is called to return the required
/// functionality to the caller.</para></summary>
/// <param name="pDevice">LPDIRECT3DDEVICE9 "this" pointer, i.e.
/// the calling device.</param>
/// <param name="pSourceRect">See MSDN doc for
/// LPDIRECT3DDEVICE9::Present()</param>
/// <returns>See MSDN doc for LPDIRECT3DDEVICE9::Present()</returns>
HRESULT __stdcall HookedD3DPresent(LPDIRECT3DDEVICE9 pDevice,
		CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride,
		CONST RGNDATA* pDirtyRegion) {
	HRESULT result;

	// Restore the old function
	gpPresentHook->DoUnhook();
	// Call the original function
	result = pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	// Restore our patch
	gpPresentHook->DoHook();

	return result;
}

/// <summary>Class constructor for a PresentHook. Simply
/// logs its creation and calls parent constructor.</summary>
PresentHook::PresentHook() : D3D9Hook(D3D_PRESENT_VFTABLEINDEX,
		D3D_PRESENT_BYTESTOPATCH, (DWORD)HookedD3DPresent) {
	CallRiotLog("Running constructor on PresentHook class.");
	CallRiotLog("PresentHook class created successfully.");
}


PresentHook::~PresentHook() {

}