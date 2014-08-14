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
#include <string>

#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")

/*	This file is intended to be a convenient way for all projects
in this solution to access offsets provided by the OffsetFinder. */

typedef unsigned int(*offset_func)(std::string, unsigned int);

/// <summary>Class used to associate offset names with functions to
/// find them.</summary>
class LoLOffset
{
private:
	std::string offsetName; // name of the offset, used to identify it
	offset_func offsetFinder; // function used to find the offset
public:
	LoLOffset(std::string name, offset_func func) {
		offsetName = name;
		offsetFinder = func;
	}
	std::string GetOffsetName() { return offsetName; }
	offset_func GetOffsetFinder() { return offsetFinder; }
	~LoLOffset() {};
};

#define NUM_OBJAIBASE_OFFSETS 6
// The different offsets we calculate for an obj_AI_Base
extern LoLOffset objAIBaseOffsets[NUM_OBJAIBASE_OFFSETS];

#define OFFSET_FILE_NAME "gameoffsets.xml"
#define WOFFSET_FILE_NAME L"gameoffsets.xml"