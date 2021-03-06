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