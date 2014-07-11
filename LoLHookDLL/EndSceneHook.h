#pragma once
#include "D3D9Hook.h"
class EndSceneHook : public D3D9Hook
{
public:
	EndSceneHook();
	~EndSceneHook();
	/// <summary>Simply calls superclass function.</summary>
	virtual DWORD DoHook() { return D3D9Hook::DoHook(); }
	/// <summary>Simply calls superclass function.</summary>
	virtual DWORD DoUnhook() { return D3D9Hook::DoUnhook(); }
};

extern EndSceneHook* gpEndSceneHook;

#define D3D_ENDSCENE_VFTABLEINDEX 42
#define D3D_ENDSCENE_BYTESTOPATCH 10