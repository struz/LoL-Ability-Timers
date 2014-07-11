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
#include "MemoryManager.h"
#include "ResetHook.h"

// Main function for this DLL
extern "C" __declspec(dllexport) int CALLBACK InitDll(LPCWSTR args) {
	// Set up any global objects if they don't already exist
	if (!gpResetHook)
		gpResetHook = new ResetHook();
	gpResetHook->DoHook(); // hook D3D::Reset

	if (!gpEndSceneHook)
		gpEndSceneHook = new EndSceneHook();
	gpEndSceneHook->DoHook(); // hook D3D::Present
	return 1;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			return TRUE;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return FALSE;
}