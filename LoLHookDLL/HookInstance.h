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
class IHook {
private:
	BOOL mIsHooked = FALSE; // whether the current hook is applied
protected:
	virtual void SetHooked(BOOL value) { mIsHooked = value; }
public:
	virtual ~IHook() { };
	virtual DWORD DoHook() = 0; // do the specific hook
	virtual DWORD DoUnhook() = 0; // unhook the specific hook
	virtual BOOL IsHooked() { return mIsHooked; }
};