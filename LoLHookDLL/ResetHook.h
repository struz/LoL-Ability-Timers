#pragma once
#include "D3D9Hook.h"
class ResetHook : public D3D9Hook
{
private:
public:
	ResetHook();
	virtual ~ResetHook();
	/// <summary>Simply calls parent function.</summary>
	virtual DWORD DoHook() { return D3D9Hook::DoHook(); }
	/// <summary>Simply calls parent function.</summary>
	virtual DWORD DoUnhook() { return D3D9Hook::DoUnhook(); }
};

extern ResetHook* gpResetHook;

#define D3D_RESET_VFTABLEINDEX 16
#define D3D_RESET_BYTESTOPATCH 10