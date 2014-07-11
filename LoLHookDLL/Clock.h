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