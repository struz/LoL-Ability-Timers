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