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
#include "EndSceneHook.h"
#include "DataAccessor.h"
#include "D3DDisplayManager.h"

// Global instance of our EndScene hook, required for our
// hooked version of EndScene to hook and unhook each time it is called
EndSceneHook* gpEndSceneHook = NULL;

/// <summary><para>Hooked version of LPDIRECT3DDEVICE9::EndScene(). Thus it
/// takes the same parameters (plus a "this" pointer as the first
/// argument, implicit in the real definition). Must unhook and
/// rehook itself each time it is called to return the required
/// functionality to the caller.</para>
/// <para>The purpose of this function is to draw an overlay on
/// top of what has already been drawn before ending the scene
/// by calling the original function.</para></summary>
/// <param name="pDevice">LPDIRECT3DDEVICE9 "this" pointer, i.e.
/// the calling device.</param>
/// <returns>See MSDN doc for LPDIRECT3DDEVICE9::EndScene()</returns>
HRESULT __stdcall HookedD3DEndScene(LPDIRECT3DDEVICE9 pDevice) {
	DWORD result;

	// === BEGIN OUR STUFF ===
	// We have to draw our stuff before we present, can't be after
	if (!gpDisplayManager) {
		gpDisplayManager = new D3DDisplayManager();
	}

	//// Only draw once this array has heroes and once the game clock starts
	if (DataAccessor::GetInstance()->GetNumScoreboardHeroes() &&
			DataAccessor::GetInstance()->GetDisplayTime() > 2 &&
			DataAccessor::GetInstance()->IsScoreboardShown())
		gpDisplayManager->DrawChampionCooldownsFromCache(pDevice);
	// === END OUR STUFF ===

	// Restore the old function
	gpEndSceneHook->DoUnhook();
	// Call the original function
	result = pDevice->EndScene();
	// Restore our patch
	gpEndSceneHook->DoHook();

	return result;
}

/// <summary>Class constructor for a EndSceneHook. Simply
/// logs its creation and calls parent constructor.</summary>
EndSceneHook::EndSceneHook() : D3D9Hook(D3D_ENDSCENE_VFTABLEINDEX,
	D3D_ENDSCENE_BYTESTOPATCH, (DWORD)HookedD3DEndScene) {
	CallRiotLog("Running constructor on EndSceneHook class.");
	CallRiotLog("EndSceneHook class created successfully.");
}


EndSceneHook::~EndSceneHook()
{
}
