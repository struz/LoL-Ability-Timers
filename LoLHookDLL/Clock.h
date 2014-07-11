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
#include "ObjectAccessor.h"
class RiotGameClockAccessor :
	public ObjectAccessor {
public:
	RiotGameClockAccessor(DWORD baseMemoryAddress) :
		ObjectAccessor(baseMemoryAddress) { }
	~RiotGameClockAccessor() { }
	// Note that the line below accesses from one clock class into the next as they
	// are stored contiguously in memory. This is mirroring how riot's code
	// accessed the data, but there is no guarantee it stays like this over
	// future patches. Time value is in seconds.
	/// <returns>The time in seconds as used by Riot's simulation code, i.e. for spell
	/// cooldown calculation, etc. Different to what is displayed at the top right.</returns>
	float GetSimulationTime() { return *reinterpret_cast<float*>(mBaseMemoryAddress + 0x2C); }
};

class TimeClockClientAccessor :
	public ObjectAccessor {
public:
	TimeClockClientAccessor(DWORD baseMemoryAddress) :
		ObjectAccessor(baseMemoryAddress) { }
	~TimeClockClientAccessor() { }
	/// <returns>The number of seconds elapsed in the timer at the top right
	/// of the scren.</returns>
	float GetGameTime() { return *reinterpret_cast<float*>(mBaseMemoryAddress + 0x4); }
	// Just guessing, but this seems like the total time since startup, including load time
	float GetTotalTime() { return *reinterpret_cast<float*>(mBaseMemoryAddress + 0x8); }
};