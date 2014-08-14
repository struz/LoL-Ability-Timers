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
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "ByteSearcher.h"
#include "FileHelper.h"
#include "pugixml.hpp"
#include "OffsetFuncs.h"

/// <summary>Calculates and creates an offsets XML file that describes the
/// offsets used to access different class attributes in League of Legends.
/// </summary>
/// <param name="filename">The filename to write to.</param>
/// <param name="programBase">The basing for the offsets. Will usually
/// need to be 0x400000.</param>
void CreateOffsetsFile(std::string filename, unsigned int programBase) {
	std::cout << "Creating offsets file..." << std::endl;

	pugi::xml_document doc;

	pugi::xml_node offsets = doc.append_child("offsets");

	pugi::xml_node objAIBase = offsets.append_child("obj_ai_base");
	for (int i = 0; i < NUM_OBJAIBASE_OFFSETS; i++) {
		LoLOffset o = objAIBaseOffsets[i];

		if (o.GetOffsetFinder() == NULL)
			continue; // skip ones which have no offset finder provided
		
		unsigned int offset;
		try {
			offset = o.GetOffsetFinder()(filename, programBase);
		}
		catch (std::exception& e) {
			std::cerr << "Error getting offset '" << o.GetOffsetName() << 
				"': " << e.what() << std::endl;
		}

		objAIBase.append_attribute(o.GetOffsetName().c_str()) =
			o.GetOffsetFinder()(filename, programBase);
	}

	std::cout << "Saving result: " << doc.save_file(OFFSET_FILE_NAME) << std::endl;
}

/// <summary>Validate command line arguments to the program.</summary>
/// <returns>True if arguments are valid, false otherwise.</summary>
bool CheckArguments(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Error: No filename provided. Please provide the path to \
			the League of Legends executable file on the command \
			line as follows: OffsetFinder.exe [League of Legends.exe path]"
			<< std::endl;
		return false;
	}

	// We just use the Windows API for this, rather than anything more cross
	// platform for now.
	if (!PathFileExistsA(argv[1])) {
		std::cerr << "Error: The file provided does not exist: " << argv[1];
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	bool argStatus = CheckArguments(argc, argv);
	if (!argStatus)
		return 1;

	// Grab the filename from the argument
	std::string filename = std::string(argv[1]);

	CreateOffsetsFile(filename, 0x400000);
	return 0;
}

