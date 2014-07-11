#pragma once
#include "D3D9Hook.h"
class PresentHook : public D3D9Hook
{
private:
public:
	PresentHook();
	virtual ~PresentHook();
	/// <summary>Simply calls superclass function.</summary>
	virtual DWORD DoHook() { return D3D9Hook::DoHook(); }
	/// <summary>Simply calls superclass function.</summary>
	virtual DWORD DoUnhook() { return D3D9Hook::DoUnhook(); }
};

extern PresentHook* gpPresentHook;

#define D3D_PRESENT_VFTABLEINDEX 17
#define D3D_PRESENT_BYTESTOPATCH 10