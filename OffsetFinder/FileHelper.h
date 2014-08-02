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
class FileHelper
{
private:
	FileHelper() {};
	static unsigned long long ReadValueFromFile(std::string filename,
		size_t addressToRead, size_t numBytesToRead);
public:
	~FileHelper() {};
	static unsigned int ReadIntFromFile(std::string filename,
		size_t addressToRead);
	static unsigned char ReadByteFromFile(std::string filename,
		size_t addressToRead);
	static long long ScanFileForBytes(std::string filename, const char* bytes,
		size_t bytesLen);
	static long long ScanFileForString(std::string filename, std::string str);
	static long long FindReferenceToAddress(std::string filename,
		unsigned int addr, long baseAddr);
};

