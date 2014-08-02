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

#include "stdafx.h"
#include "OffsetMap.h"
#include "pugixml.hpp"
#include "OffsetFuncs.h"
#include "MemoryManager.h"
#include "Shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

// Helper function to calculate the path at which the XML file resides
TCHAR* GetPathNameFromFileName(TCHAR* path) {
	TCHAR* curChar = path + (lstrlenW(path) - 1);

	// Loop from the end to the beginning of the string or until
	// we find a path separator
	for (; curChar > path; curChar--) {
		if (curChar[0] == '\\') {
			curChar[1] = NULL;
			break;
		}
	}
	return path;
}

/// <summary>Loads the offset map from the offset file.</summary>
void OffsetMap::LoadOffsetMap() {
	// Set this here because there isn't any really good way to recover
	// from an error during the load anyway. The game will likely crash
	// but we will have at least logged the problem.
	offsetMapLoaded = true;

	pugi::xml_document doc;

	// Do some manipulation to see where the DLL was loaded from
	// and read the offset file from the same folder
	MODULEENTRY32 module = MemoryManager::GetModuleInfo(THIS_DLL_NAME);
	TCHAR* modulePath = GetPathNameFromFileName(module.szExePath);
	TCHAR* totalPath = new TCHAR[lstrlenW(modulePath) + lstrlenW(WOFFSET_FILE_NAME) + 1];
	StrCpy(totalPath, modulePath);
	StrCat(totalPath, WOFFSET_FILE_NAME);

	CallRiotLog("Totalpath = %ls", totalPath);

	bool result = doc.load_file(totalPath);
	if (result) {
		pugi::xml_node base = doc.child("offsets");
		for (pugi::xml_node_iterator it = base.begin(); it != base.end(); ++it) {
			// Find the node with our map's name
			if (strcmp(mapName.c_str(), it->name()) == 0) {
				// Iterate over each of the attributes and insert the offset
				for (pugi::xml_attribute_iterator ait = it->attributes_begin();
						ait != it->attributes_end(); ++ait) {
					offsetMap[ait->name()] = ait->as_uint();
				}
				return;
			}
		}
	}
	CallRiotLog("Error loading offset map named '%s'", mapName.c_str());
}

/// <summary>Gets an offset value from the map by using the provided
/// offset name as a key.</summary>
/// <param name="offset">The name of the offset to get.</param>
/// <retuirns>The offset value for that offset name.</returns>
unsigned int OffsetMap::GetOffsetValue(std::string offset) {
	if (!offsetMapLoaded)
		LoadOffsetMap();

	return offsetMap.at(offset);
}