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

#include "OffsetFuncs.h"
#include "FileHelper.h"

// ===== obj_AI_Base finders =====
unsigned int FindObjAIBaseSkinName(std::string filename,
		unsigned int programBase) {
	std::string bytes = "  SkinName =";
	long long addr = FileHelper::ScanFileForString(
		filename,
		bytes
	);
	long long refAddr = FileHelper::FindReferenceToAddress(
		filename,
		(unsigned int)addr,
		programBase
	);

	return FileHelper::ReadIntFromFile(filename, (size_t)refAddr - 11);
}
unsigned int FindObjAIBaseGold(std::string filename,
		unsigned int programBase) {
	std::string bytes = "  Gold =";
	long long addr = FileHelper::ScanFileForString(
		filename,
		bytes
	);
	long long refAddr = FileHelper::FindReferenceToAddress(
		filename,
		(unsigned int)addr,
		programBase
	);

	return FileHelper::ReadIntFromFile(filename, (size_t)refAddr - 13);
}
unsigned int FindObjAIBaseSpellbook(std::string filename,
		unsigned int programBase) {
	std::string bytes = "  AutoAttackTargetingFlags =";
	long long addr = FileHelper::ScanFileForString(
		filename,
		bytes
	);
	long long refAddr = FileHelper::FindReferenceToAddress(
		filename,
		(unsigned int)addr,
		programBase
	);

	return FileHelper::ReadIntFromFile(filename, (size_t)refAddr + 0x1C7);
}
unsigned int FindObjAIBaseName(std::string filename,
		unsigned int programBase) {
	std::string bytes = "  Name =";
	long long addr = FileHelper::ScanFileForString(
		filename,
		bytes
	);
	long long refAddr = FileHelper::FindReferenceToAddress(
		filename,
		(unsigned int)addr,
		programBase
	);

	return FileHelper::ReadByteFromFile(filename, (size_t)refAddr - 19);
}
unsigned int FindObjAIBaseTeam(std::string filename,
		unsigned int programBase) {
	std::string bytes = "  Team =";
	long long addr = FileHelper::ScanFileForString(
		filename,
		bytes
	);
	long long refAddr = FileHelper::FindReferenceToAddress(
		filename,
		(unsigned int)addr,
		programBase
	);

	return FileHelper::ReadByteFromFile(filename, (size_t)refAddr - 3);
}
unsigned int FindObjAIBaseAlive(std::string filename,
		unsigned int programBase) {
	std::string bytes = "  Dead =";
	long long addr = FileHelper::ScanFileForString(
		filename,
		bytes
	);
	long long refAddr = FileHelper::FindReferenceToAddress(
		filename,
		(unsigned int)addr,
		programBase
	);

	return FileHelper::ReadByteFromFile(filename, (size_t)refAddr - 11);
}
// ===== end obj_AI_Base finders =====

LoLOffset objAIBaseOffsets[NUM_OBJAIBASE_OFFSETS] = {
	LoLOffset("skinname", FindObjAIBaseSkinName),
	LoLOffset("gold", FindObjAIBaseGold),
	LoLOffset("spellbook", FindObjAIBaseSpellbook),
	LoLOffset("name", FindObjAIBaseName),
	LoLOffset("team", FindObjAIBaseTeam),
	LoLOffset("alive", FindObjAIBaseAlive)
};