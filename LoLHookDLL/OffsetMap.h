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
class OffsetMap
{
private:
	std::string mapName; // the name of the xml node to read from the offset file

	bool offsetMapLoaded = false; // used for lazy initialization of the map
	std::map<std::string, unsigned int> offsetMap; // the map of names to offsets

	void LoadOffsetMap();
public:
	/// <param name="offsetClass">The name of the xml node to read the offset
	/// details from.</param>
	OffsetMap(std::string offsetClass) {	
		mapName = offsetClass;
	};
	~OffsetMap() { };

	unsigned int GetOffsetValue(std::string offset);
};

