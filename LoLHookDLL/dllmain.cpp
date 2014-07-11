// dllmain.cpp : Defines the entry point for the DLL application.
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